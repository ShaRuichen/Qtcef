#ifndef QTCEF_HANDLER_H
#define QTCEF_HANDLER_H

#include "Qtcef.h"
#include "Arguments.h"

namespace Qtcef
{
    class Handler : public CefV8Handler
    {
    public:
        Handler(QObject *object, const QMetaMethod &method);
        bool Execute(const CefString& name,
                     CefRefPtr<CefV8Value> object,
                     const CefV8ValueList& arguments,
                     CefRefPtr<CefV8Value>& returnValue,
                     CefString& exception) override;
    private:
        QObject * const _object;
        const QMetaMethod _method;
        Argument *_arguments[10];
        ReturnArgument *_returnArgument;
    IMPLEMENT_REFCOUNTING(Handler);
    };
}

#endif // QTCEF_HANDLER_H
