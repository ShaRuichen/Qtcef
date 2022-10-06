#include "AppHandler.h"

using namespace Qtcef;

static void boolV8ToCef(const CefRefPtr<CefV8Value> &v8, const CefRefPtr<CefValue> &cef)
{
    cef->SetBool(v8->GetBoolValue());
}
static void intV8ToCef(const CefRefPtr<CefV8Value> &v8, const CefRefPtr<CefValue> &cef)
{
    cef->SetInt(v8->GetIntValue());
}
static void doubleV8ToCef(const CefRefPtr<CefV8Value> &v8, const CefRefPtr<CefValue> &cef)
{
    cef->SetDouble(v8->GetDoubleValue());
}
static void stringV8ToCef(const CefRefPtr<CefV8Value> &v8, const CefRefPtr<CefValue> &cef)
{
    cef->SetString(v8->GetStringValue());
}

static void voidCefToV8(const CefRefPtr<CefValue> &cef, CefRefPtr<CefV8Value> &v8)
{
    v8 = CefV8Value::CreateUndefined();
}
static void boolCefToV8(const CefRefPtr<CefValue> &cef, CefRefPtr<CefV8Value> &v8)
{
    v8 = CefV8Value::CreateBool(cef->GetBool());
}
static void intCefToV8(const CefRefPtr<CefValue> &cef, CefRefPtr<CefV8Value> &v8)
{
    v8 = CefV8Value::CreateInt(cef->GetInt());
}
static void doubleCefToV8(const CefRefPtr<CefValue> &cef, CefRefPtr<CefV8Value> &v8)
{
    v8 = CefV8Value::CreateDouble(cef->GetDouble());
}
static void stringCefToV8(const CefRefPtr<CefValue> &cef, CefRefPtr<CefV8Value> &v8)
{
    v8 = CefV8Value::CreateString(cef->GetString());
}
static void valueCefToV8(const CefRefPtr<CefValue> &cef, CefRefPtr<CefV8Value> &v8);
static void listCefToV8(const CefRefPtr<CefValue> &cef, CefRefPtr<CefV8Value> &v8)
{
    const auto list = cef->GetList();
    const auto count = static_cast<int>(list->GetSize());
    v8 = CefV8Value::CreateArray(count);
    for (auto i = 0; i < count; i++) {
        CefRefPtr<CefV8Value> value;
        valueCefToV8(list->GetValue(i), value);
        v8->SetValue(i, value);
    }
}
static void dictionaryCefToV8(const CefRefPtr<CefValue> &cef, CefRefPtr<CefV8Value> &v8)
{
    const auto dictionary = cef->GetDictionary();
    v8 = CefV8Value::CreateObject(nullptr, nullptr);
    CefDictionaryValue::KeyList keys;
    dictionary->GetKeys(keys);
    for (const auto &key : keys) {
        CefRefPtr<CefV8Value> value;
        valueCefToV8(dictionary->GetValue(key), value);
        v8->SetValue(key, value, V8_PROPERTY_ATTRIBUTE_NONE);
    }
}
static void valueCefToV8(const CefRefPtr<CefValue> &cef, CefRefPtr<CefV8Value> &v8)
{
    switch (cef->GetType()) {
        case VTYPE_NULL: CefV8Value::CreateNull(); break;
        case VTYPE_BOOL: boolCefToV8(cef, v8); break;
        case VTYPE_INT: intCefToV8(cef, v8); break;
        case VTYPE_DOUBLE: doubleCefToV8(cef, v8); break;
        case VTYPE_LIST: listCefToV8(cef, v8); break;
        case VTYPE_DICTIONARY: dictionaryCefToV8(cef, v8); break;
        default: v8 = CefV8Value::CreateUndefined(); break;
    }
}

AppHandler::AppHandler(const CefRefPtr<CefFrame> &frame, int methodCount)
        : _frame{frame}
        , _freePromises{}
{
    _methodsData.reserve(methodCount);
}

void AppHandler::addMethod(const QMetaMethod &metaMethod)
{
    _methodsData.insert(metaMethod.name(), new MethodData{metaMethod});
}

bool AppHandler::Execute(const CefString &name,
                         CefRefPtr<CefV8Value> object,
                         const CefV8ValueList &arguments,
                         CefRefPtr<CefV8Value> &returnValue,
                         CefString &exception)
{
    const auto methodData = _methodsData.value(FROM_CEF(name));
    if (methodData == nullptr) return false;
    const auto &metaMethod = methodData->metaMethod;
    const auto parameterCount = metaMethod.parameterCount();
    if (arguments.size() < static_cast<size_t>(parameterCount)) {
        exception = u"Argument count is too small.";
        return true;
    }
    if (methodData->v8ToCefHandlers == nullptr) {
        methodData->v8ToCefHandlers = new V8ToCefHandler[parameterCount];
        for (auto i = 0; i < parameterCount; i++) {
            auto &handler = methodData->v8ToCefHandlers[i];
            switch (metaMethod.parameterType(i)) {
                case QMetaType::Bool: handler = boolV8ToCef; break;
                case QMetaType::Int: handler = intV8ToCef; break;
                case QMetaType::Double: handler = doubleV8ToCef; break;
                case QMetaType::QString: handler = stringV8ToCef; break;
            }
        }
        auto &handler = methodData->cefToV8Handler;
        switch (metaMethod.returnType()) {
            case QMetaType::Void: handler = voidCefToV8; break;
            case QMetaType::Bool: handler = boolCefToV8; break;
            case QMetaType::Int: handler = intCefToV8; break;
            case QMetaType::Double: handler = doubleCefToV8; break;
            case QMetaType::QString: handler = stringCefToV8; break;
            case QMetaType::QJsonArray: handler = listCefToV8; break;
            case QMetaType::QJsonObject: handler = dictionaryCefToV8; break;
        }
    }
    const auto message = CefProcessMessage::Create(name);
    const auto argumentList = message->GetArgumentList();
    for (auto i = 0; i < parameterCount; i++) {
        const auto value = CefValue::Create();
        methodData->v8ToCefHandlers[i](arguments[i], value);
        argumentList->SetValue(i, value);
    }
    _frame->SendProcessMessage(PID_BROWSER, message);
    auto promise = _freePromises;
    if (promise != nullptr) {
        _freePromises = promise->next;
    } else {
        promise = new Promise;
    }
    if (methodData->promises == nullptr) {
        methodData->promises = promise->previous = promise->next = promise;
    } else {
        methodData->promises->previous = methodData->promises->previous->next = promise;
        promise->previous = methodData->promises->previous;
        promise->next = methodData->promises;
    }
    CefRefPtr<CefV8Value> evalValue;
    CefRefPtr<CefV8Exception> evalException;
    CefV8Context::GetCurrentContext()->Eval("(()=>{let a,b=new Promise(c=>a=c);return[a,b]})()", "Qtcef", 0, evalValue, evalException);
    promise->resolve = evalValue->GetValue(0);
    promise->context = CefV8Context::GetCurrentContext();
    returnValue = evalValue->GetValue(1);
    return true;
}

bool AppHandler::receiveMessage(const CefRefPtr<CefProcessMessage> &message)
{
    const auto methodData = _methodsData.value(FROM_CEF(message->GetName()));
    if (methodData == nullptr) return false;
    const auto promise = methodData->promises;
    if (promise == nullptr) return false;
    if (promise->next == promise) {
        methodData->promises = nullptr;
    } else {
        methodData->promises = promise->next;
        promise->next->previous = promise->previous;
        promise->previous->next = promise->next;
    }
    CefRefPtr<CefV8Value> value;
    methodData->cefToV8Handler(message->GetArgumentList()->GetValue(0), value);
    promise->resolve->ExecuteFunctionWithContext(promise->context, nullptr, {value});
    promise->resolve = nullptr;
    promise->context = nullptr;
    promise->next = _freePromises;
    _freePromises = promise;
    return true;
}
