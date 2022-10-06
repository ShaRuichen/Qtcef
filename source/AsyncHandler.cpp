#include "AsyncHandler.h"
#include "Arguments.h"

using namespace Qtcef;

AsyncHandler::Data *AsyncHandler::_freeData;

struct AsyncHandler::Data
{
    QFutureWatcher<Data *> watcher;
    const AsyncHandler *handler;
    Argument *arguments[10];
    ReturnArgument *returnArgument;
    CefRefPtr<CefV8Value> resolve;
    CefRefPtr<CefV8Context> context;
    Data *nextFreeData;
    Data();
};

AsyncHandler::Data::Data()
        : watcher{}
        , handler{}
        , arguments{}
        , returnArgument{}
        , nextFreeData{}
{
    QObject::connect(&watcher, &QFutureWatcher<Data *>::finished, [this]{ finish(this); });
}

AsyncHandler::AsyncHandler(QObject *object, const QMetaMethod &method)
        : _object{object}
        , _method{method}
{
}

bool AsyncHandler::Execute(const CefString &name,
                           CefRefPtr<CefV8Value> object,
                           const CefV8ValueList &arguments,
                           CefRefPtr<CefV8Value> &returnValue,
                           CefString &exception)
{
    auto data = _freeData;
    if (data == nullptr) {
        data = new Data;
    } else {
        _freeData = data->nextFreeData;
    }
    data->handler = this;
    auto i = 0;
    for (; i < _method.parameterCount(); i++) {
        data->arguments[i] = Argument::create(_method.parameterType(i));
        data->arguments[i]->set(arguments[i]);
    }
    for (; i < 10; i++) {
        data->arguments[i] = Argument::voidArgument();
    }
    data->returnArgument = ReturnArgument::create(_method.returnType());
    const auto future = QtConcurrent::run(run, data);
    data->watcher.setFuture(future);
    CefRefPtr<CefV8Value> evalValue;
    CefRefPtr<CefV8Exception> evalException;
    CefV8Context::GetCurrentContext()->Eval("(()=>{let a,b=new Promise(c=>a=c);return[a,b]})()", "Qtcef", 0, evalValue, evalException);
    data->resolve = evalValue->GetValue(0);
    data->context = CefV8Context::GetCurrentContext();
    returnValue = evalValue->GetValue(1);
    return true;
}

AsyncHandler::Data *AsyncHandler::run(Data *data)
{
    const auto handler = data->handler;
    handler->_method.invoke(handler->_object, Qt::DirectConnection, *data->returnArgument, *data->arguments[0], *data->arguments[1], *data->arguments[2], *data->arguments[3], *data->arguments[4], *data->arguments[5], *data->arguments[6], *data->arguments[7], *data->arguments[8], *data->arguments[9]);
    return data;
}

void AsyncHandler::finish(Data *data)
{
    CefRefPtr<CefV8Value> result;
    data->context->Enter();
    data->returnArgument->get(result);
    data->resolve->ExecuteFunction(nullptr, {result});
    data->context->Exit();
}
