#ifndef QTCEF_ARGUMENT_DATA_H
#define QTCEF_ARGUMENT_DATA_H

#include "Qtcef.h"

namespace Qtcef
{
    CefRefPtr<CefV8Value> qtToV8(const QJsonObject &object);

    class Argument : public QGenericArgument
    {
    public:
        static Argument *voidArgument();
        static Argument *get(int type);
        static Argument *create(int type);
        [[nodiscard]] const auto &generic() const { return static_cast<const QGenericArgument &>(*this); }
        virtual void set(const CefRefPtr<CefV8Value> &value) = 0;
        virtual void set(const CefRefPtr<CefValue> &value) = 0;
    protected:
        Argument(const char *name, const void *data);
    };

    class ReturnArgument : public QGenericReturnArgument
    {
    public:
        static ReturnArgument *get(int type);
        static ReturnArgument *create(int type);
        [[nodiscard]] const auto &generic() const { return static_cast<const QGenericReturnArgument &>(*this); }
        virtual void get(CefRefPtr<CefV8Value> &value) const = 0;
        virtual void get(const CefRefPtr<CefValue> &value) const = 0;
    protected:
        ReturnArgument(const char *name, void *data);
    };
}

#endif // QTCEF_ARGUMENT_DATA_H
