#include "RendererApp.h"
#include "App.h"
#include "AppHandler.h"
#include "AsyncHandler.h"
#include "Bridge.h"
#include "Handler.h"

using namespace Qtcef;

constexpr auto eventCode =
u"(()=>{let events=new Map;"
    "return[function(type,listener){"
        "let listeners=events.get(type);"
        "if(!listeners){"
            "listeners=new Set,"
            "events.set(type,listeners)}"
        "listeners.add(listener)},"
    "function(type,listener){"
        "let listeners=events.get(type);"
        "if(listeners){"
            "listeners.delete(listener)}},"
    "function(type,event){"
        "let listeners=events.get(type);"
        "if(listeners){"
            "for(let listener of listeners){"
                "listener(event)}}}]})()";

void RendererApp::OnBrowserCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefDictionaryValue> extraInfo)
{
    _bridgeNames = extraInfo->GetList(u"bridgeNames")->Copy();
    _data = extraInfo->GetDictionary(u"data")->Copy(false);
    _callback = base::BindRepeating(&RendererApp::doEventLoop, this);
    CefPostTask(TID_RENDERER, _callback);
}

void RendererApp::OnContextCreated(CefRefPtr<CefBrowser> browser,
                                   CefRefPtr<CefFrame> frame,
                                   CefRefPtr<CefV8Context> context)
{
    const auto global = context->GetGlobal();
    Q_ASSERT_X(!global->HasValue(u"qtcef"), "Qtcef", "The javascript context can not has \"qtcef\" global variable");
    auto qtcef = CefV8Value::CreateObject(nullptr, nullptr);
    global->SetValue(u"qtcef", qtcef, V8_PROPERTY_ATTRIBUTE_NONE);
    {
        if (_data->GetSize() != 0) {
            const auto data = cefToV8(_data);
            qtcef->SetValue(u"data", data, V8_PROPERTY_ATTRIBUTE_NONE);
        }
    }
    {
        CefRefPtr<CefV8Value> evalValue;
        CefRefPtr<CefV8Exception> evalException;
        context->Eval(eventCode, u"Qtcef", 0, evalValue, evalException);
        qtcef->SetValue(u"addEventListener", evalValue->GetValue(0), V8_PROPERTY_ATTRIBUTE_NONE);
        qtcef->SetValue(u"removeEventListener", evalValue->GetValue(1), V8_PROPERTY_ATTRIBUTE_NONE);
        const auto dispatchEvent = evalValue->GetValue(2);
        qtcef->SetValue(u"dispatchEvent", dispatchEvent, V8_PROPERTY_ATTRIBUTE_NONE);
        Bridge::setDispatchEvent(dispatchEvent, context);
    }
    const auto appObject = CefV8Value::CreateObject(nullptr, nullptr);
    qtcef->SetValue(u"App", appObject, V8_PROPERTY_ATTRIBUTE_NONE);
    const auto metaApp = App::instance()->metaObject();
    _appHandler = new AppHandler{frame, metaApp->methodCount() - App::staticMetaObject.methodCount()};
    for (auto i = App::staticMetaObject.methodCount(); i < metaApp->methodCount(); i++) {
        const auto metaMethod = metaApp->method(i);
        if (metaMethod.methodType() != QMetaMethod::Method || strcmp(metaMethod.tag(), "QTCEF_NOT_BRIDGE") == 0) continue;
        _appHandler->addMethod(metaMethod);
        const CefString name = metaMethod.name().constData();
        const auto function = CefV8Value::CreateFunction(name, _appHandler);
        appObject->SetValue(name, function, V8_PROPERTY_ATTRIBUTE_NONE);
    }
    for (size_t i = 0; i < _bridgeNames->GetSize(); i++) {
        const auto bridgeName = FROM_CEF(_bridgeNames->GetString(i));
        const auto metaBridge = Bridge::metaBridges().value(bridgeName);
        if (metaBridge == nullptr) continue;
        const auto bridge = Bridge::instance(metaBridge);
        auto bridgeObject = CefV8Value::CreateObject(nullptr, nullptr);
        qtcef->SetValue(TO_CEF(bridgeName), bridgeObject, V8_PROPERTY_ATTRIBUTE_NONE);
        for (auto j = Bridge::staticMetaObject.methodCount(); j < metaBridge->methodCount(); j++) {
            const auto metaMethod = metaBridge->method(j);
            const auto tag = metaMethod.tag();
            if (metaMethod.methodType() != QMetaMethod::Method || strcmp(tag, "QTCEF_NOT_BRIDGE") == 0) continue;
            const CefString name = metaMethod.name().data();
            CefRefPtr<CefV8Handler> handler;
            if (strcmp(tag, "QTCEF_ASYNC") == 0) {
                handler = new AsyncHandler{bridge, metaMethod};
            } else {
                handler = new Handler{bridge, metaMethod};
            }
            const auto function = CefV8Value::CreateFunction(name, handler);
            bridgeObject->SetValue(name, function, V8_PROPERTY_ATTRIBUTE_NONE);
        }
    }
}

void RendererApp::doEventLoop()
{
    while (_loop.processEvents()) {}
    QThread::msleep(10);
    CefPostTask(TID_RENDERER, _callback);
}

bool RendererApp::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
                                           CefRefPtr<CefFrame> frame,
                                           CefProcessId sourceProcess,
                                           CefRefPtr<CefProcessMessage> message)
{
    return _appHandler->receiveMessage(message);
}
