#ifndef QTCEF_APP_HANDLER_H
#define QTCEF_APP_HANDLER_H

#include "Qtcef.h"

namespace Qtcef
{
    class AppHandler final : public CefV8Handler
    {
    public:
        AppHandler(const CefRefPtr<CefFrame> &frame, int methodCount);
        void addMethod(const QMetaMethod &metaMethod);
        bool Execute(const CefString& name,
                     CefRefPtr<CefV8Value> object,
                     const CefV8ValueList& arguments,
                     CefRefPtr<CefV8Value>& returnValue,
                     CefString& exception) final;
        bool receiveMessage(const CefRefPtr<CefProcessMessage> &message);
    private:
        struct Promise
        {
            CefRefPtr<CefV8Value> resolve;
            CefRefPtr<CefV8Context> context;
            Promise *previous{};
            Promise *next{};
        };
        using V8ToCefHandler = void (*)(const CefRefPtr<CefV8Value> &v8, const CefRefPtr<CefValue> &cef);
        using CefToV8Handler = void (*)(const CefRefPtr<CefValue> &cef, CefRefPtr<CefV8Value> &v8);
        struct MethodData
        {
            const QMetaMethod metaMethod;
            V8ToCefHandler *v8ToCefHandlers{};
            CefToV8Handler cefToV8Handler{};
            Promise *promises{};
        };
    private:
        const CefRefPtr<CefFrame> _frame;
        QHash<QString, MethodData *> _methodsData;
        Promise *_freePromises;
    IMPLEMENT_REFCOUNTING(AppHandler);
    };
}

#endif // QTCEF_APP_HANDLER_H
