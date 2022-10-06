#include "Bridge.h"
#include "Arguments.h"

using namespace Qtcef;

QHash<QString, const QMetaObject *> Bridge::_metaBridges;
QHash<const QMetaObject *, Bridge *> Bridge::_bridges;
CefRefPtr<CefV8Value> Bridge::_dispatchEvent;
CefRefPtr<CefV8Context> Bridge::_context;

Bridge *Bridge::instance(const QMetaObject *metaObject)
{
    auto &bridge = _bridges[metaObject];
    if (bridge != nullptr) return bridge;
    const auto object = metaObject->newInstance();
    Q_ASSERT_X(object != nullptr, "Qtcef::Bridge", "Registered Bridge must has a Q_INVOKABLE constructor with 0 parameter.");
    bridge = qobject_cast<Bridge *>(object);
    Q_ASSERT_X(bridge != nullptr, "Qtcef::Bridge", "Class registered by QTCEF_BRIDGE must be a subclass of Qtcef::Bridge.");
    return bridge;
}

void Bridge::setDispatchEvent(const CefRefPtr<CefV8Value> &function, const CefRefPtr<CefV8Context> &context)
{
    _dispatchEvent = function;
    _context = context;
}

void Bridge::dispatchEvent(const QString &type, const QJsonObject &event)
{
    if (CefCurrentlyOn(TID_RENDERER)) {
        internalDispatchEvent(type, event);
    } else {
        CefPostTask(TID_RENDERER, base::BindOnce(Bridge::internalDispatchEvent, type, event));
    }
}

bool Bridge::qtcef_register(const QMetaObject *metaObject)
{
    const QString name = metaObject->className();
    Q_ASSERT_X(!_metaBridges.contains(name), "Qtcef::Bridge", "Can not register a Bridge more than once.");
    _metaBridges.insert(name, metaObject);
    return false;
}

void Bridge::internalDispatchEvent(const QString &type, const QJsonObject &event)
{
    _context->Enter();
    _dispatchEvent->ExecuteFunction(nullptr, {CefV8Value::CreateString(TO_CEF(type)), qtToV8(event)});
    _context->Exit();
}
