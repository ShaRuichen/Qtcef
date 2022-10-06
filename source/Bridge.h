#ifndef QTCEF_BRIDGE_H
#define QTCEF_BRIDGE_H

#include "Qtcef.h"

namespace Qtcef
{
    class Bridge : public QObject
    {
        Q_OBJECT
    public:
        static const auto &metaBridges() { return _metaBridges; }
        static Bridge *instance(const QMetaObject *metaObject);
        static void setDispatchEvent(const CefRefPtr<CefV8Value> &function, const CefRefPtr<CefV8Context> &context);
    protected:
        static void dispatchEvent(const QString &type, const QJsonObject &event);
    protected:
        static bool qtcef_register(const QMetaObject *metaObject);
    private:
        static void internalDispatchEvent(const QString &type, const QJsonObject &event);
    private:
        static QHash<QString, const QMetaObject *> _metaBridges;
        static QHash<const QMetaObject *, Bridge *> _bridges;
        static CefRefPtr<CefV8Value> _dispatchEvent;
        static CefRefPtr<CefV8Context> _context;
    };
}

#endif // QTCEF_BRIDGE_H
