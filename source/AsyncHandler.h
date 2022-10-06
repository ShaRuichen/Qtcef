#ifndef QTCEF_ASYNC_HANDLER_H
#define QTCEF_ASYNC_HANDLER_H

#include "Qtcef.h"

namespace Qtcef
{
    class AsyncHandler final : public CefV8Handler
    {
    public:
        AsyncHandler(QObject *object, const QMetaMethod &method);
        bool Execute(const CefString& name,
                     CefRefPtr<CefV8Value> object,
                     const CefV8ValueList& arguments,
                     CefRefPtr<CefV8Value>& returnValue,
                     CefString& exception) final;
    private:
        struct Data;
        static Data *_freeData;
        static Data *run(Data *data);
        static void finish(Data *data);
    private:
        QObject * const _object;
        const QMetaMethod _method;
    IMPLEMENT_REFCOUNTING(AsyncHandler);
    };
}

#endif // QTCEF_ASYNC_HANDLER_H
