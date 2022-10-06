#include "Qtcef.h"

using namespace Qtcef;

CefRefPtr<CefV8Value> Qtcef::qtToV8(const QJsonValue &value)
{
    switch (value.type()) {
        case QJsonValue::Null: return CefV8Value::CreateNull();
        case QJsonValue::Bool: return CefV8Value::CreateBool(value.toBool());
        case QJsonValue::Double: return CefV8Value::CreateDouble(value.toDouble());
        case QJsonValue::String: return CefV8Value::CreateString(TO_CEF(value.toString()));
        case QJsonValue::Array: return qtToV8(value.toArray());
        case QJsonValue::Object: return qtToV8(value.toObject());
        default: return CefV8Value::CreateUndefined();
    }
}
CefRefPtr<CefV8Value> Qtcef::qtToV8(const QJsonArray &array)
{
    const auto count = static_cast<int>(array.count());
    auto v8 = CefV8Value::CreateArray(count);
    for (auto i = 0; i < count; i++) {
        v8->SetValue(i, qtToV8(array[i]));
    }
    return v8;
}
CefRefPtr<CefV8Value> Qtcef::qtToV8(const QJsonObject &object)
{
    auto v8 = CefV8Value::CreateObject(nullptr, nullptr);
    for (auto iterator = object.begin(); iterator != object.end(); iterator++) {
        v8->SetValue(TO_CEF(iterator.key()), qtToV8(iterator.value()), V8_PROPERTY_ATTRIBUTE_NONE);
    }
    return v8;
}

CefRefPtr<CefValue> Qtcef::qtToCef(const QJsonValue &value)
{
    auto cef = CefValue::Create();
    switch (value.type()) {
        case QJsonValue::Null: cef->SetNull(); break;
        case QJsonValue::Bool: cef->SetBool(value.toBool()); break;
        case QJsonValue::Double: cef->SetDouble(value.toDouble()); break;
        case QJsonValue::String: cef->SetString(TO_CEF(value.toString())); break;
        case QJsonValue::Array: cef->SetList(qtToCef(value.toArray())); break;
        case QJsonValue::Object: cef->SetDictionary(qtToCef(value.toObject())); break;
        default: break;
    }
    return cef;
}
CefRefPtr<CefListValue> Qtcef::qtToCef(const QJsonArray &array)
{
    auto list = CefListValue::Create();
    for (qsizetype i = 0; i < array.count(); i++) {
        list->SetValue(i, qtToCef(array[i]));
    }
    return list;
}
CefRefPtr<CefDictionaryValue> Qtcef::qtToCef(const QJsonObject &object)
{
    auto dictionary = CefDictionaryValue::Create();
    for (auto iterator = object.begin(); iterator != object.end(); iterator++) {
        dictionary->SetValue(TO_CEF(iterator.key()), qtToCef(iterator.value()));
    }
    return dictionary;
}

CefRefPtr<CefV8Value> Qtcef::cefToV8(const CefRefPtr<CefValue> &value)
{
    switch (value->GetType()) {
        case VTYPE_NULL: return CefV8Value::CreateNull();
        case VTYPE_BOOL: return CefV8Value::CreateBool(value->GetBool());
        case VTYPE_INT: return CefV8Value::CreateInt(value->GetInt());
        case VTYPE_DOUBLE: return CefV8Value::CreateDouble(value->GetDouble());
        case VTYPE_STRING: return CefV8Value::CreateString(value->GetString());
        case VTYPE_LIST: return cefToV8(value->GetList());
        case VTYPE_DICTIONARY: return cefToV8(value->GetDictionary());
        default: return CefV8Value::CreateUndefined();
    }
}
CefRefPtr<CefV8Value> Qtcef::cefToV8(const CefRefPtr<CefListValue> &list)
{
    const auto count = static_cast<int>(list->GetSize());
    auto array = CefV8Value::CreateArray(count);
    for (auto i = 0; i < count; i++) {
        array->SetValue(i, cefToV8(list->GetValue(i)));
    }
    return array;
}
CefRefPtr<CefV8Value> Qtcef::cefToV8(const CefRefPtr<CefDictionaryValue> &dictionary)
{
    auto object = CefV8Value::CreateObject(nullptr, nullptr);
    CefDictionaryValue::KeyList keys;
    dictionary->GetKeys(keys);
    for (const auto &key : keys) {
        object->SetValue(key, cefToV8(dictionary->GetValue(key)), V8_PROPERTY_ATTRIBUTE_NONE);
    }
    return object;
}
