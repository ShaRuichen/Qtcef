#include "Handler.h"

using namespace Qtcef;

Handler::Handler(QObject *object, const QMetaMethod &method)
        : _object{object}
        , _method{method}
        , _arguments{}
{
    const auto parameterCount = method.parameterCount();
    Q_ASSERT_X(parameterCount <= 10, "Qtcef", "The parameter count of registered method must be less than or equal to 10.");
    auto i = 0;
    for (; i < parameterCount; i++) {
        _arguments[i] = Argument::get(method.parameterType(i));
    }
    for (; i < 10; i++) {
        _arguments[i] = Argument::voidArgument();
    }
    _returnArgument = ReturnArgument::get(method.returnType());
}

bool Handler::Execute(const CefString &name,
                      CefRefPtr<CefV8Value> object,
                      const CefV8ValueList &arguments,
                      CefRefPtr<CefV8Value> &returnValue,
                      CefString &exception)
{
    if (arguments.size() < _method.parameterCount()) {
        exception = u"Arguments is too few.";
        return true;
    }
    for (auto i = 0; i < _method.parameterCount(); i++) {
        _arguments[i]->set(arguments[i]);
    }
    _method.invoke(_object,
                   _returnArgument->generic(),
                   _arguments[0]->generic(),
                   _arguments[1]->generic(),
                   _arguments[2]->generic(),
                   _arguments[3]->generic(),
                   _arguments[4]->generic(),
                   _arguments[5]->generic(),
                   _arguments[6]->generic(),
                   _arguments[7]->generic(),
                   _arguments[8]->generic(),
                   _arguments[9]->generic());
    _returnArgument->get(returnValue);
    return true;
}
