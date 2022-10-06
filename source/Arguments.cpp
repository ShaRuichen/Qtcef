#include "Arguments.h"

namespace Qtcef
{
    class VoidArgument final : public Argument
    {
    public:
        static auto instance() { return &_instance; }
        VoidArgument() : Argument{nullptr, nullptr} {}
        void set(const CefRefPtr<CefV8Value> &value) final {}
        void set(const CefRefPtr<CefValue> &value) final {}
    private:
        static VoidArgument _instance;
    };

    class BoolArgument final : public Argument
    {
    public:
        static auto instance() { return &_instance; }
        BoolArgument() : Argument{"bool", &_value}, _value{} {}
        void set(const CefRefPtr<CefV8Value> &value) final { _value = value->GetBoolValue(); }
        void set(const CefRefPtr<CefValue> &value) final { _value = value->GetBool(); }
    private:
        static BoolArgument _instance;
        bool _value;
    };

    class IntArgument final : public Argument
    {
    public:
        static auto instance() { return &_instance; }
        IntArgument() : Argument{"int", &_value}, _value{} {}
        void set(const CefRefPtr<CefV8Value> &value) final { _value = value->GetIntValue(); }
        void set(const CefRefPtr<CefValue> &value) final { _value = value->GetInt(); }
    private:
        static IntArgument _instance;
        int _value;
    };

    class DoubleArgument final : public Argument
    {
    public:
        static auto instance() { return &_instance; }
        DoubleArgument() : Argument{"double", &_value}, _value{} {}
        void set(const CefRefPtr<CefV8Value> &value) final { _value = value->GetDoubleValue(); }
        void set(const CefRefPtr<CefValue> &value) final { _value = value->GetDouble(); }
    private:
        static DoubleArgument _instance;
        double _value;
    };

    class StringArgument final : public Argument
    {
    public:
        static auto instance() { return &_instance; }
        StringArgument() : Argument{"QString", &_value}, _value{} {}
        void set(const CefRefPtr<CefV8Value> &value) final { _value = FROM_CEF(value->GetStringValue()); }
        void set(const CefRefPtr<CefValue> &value) final { _value = FROM_CEF(value->GetString()); }
    private:
        static StringArgument _instance;
        QString _value;
    };

    class VoidReturnArgument final : public ReturnArgument
    {
    public:
        static auto instance() { return &_instance; }
        VoidReturnArgument() : ReturnArgument{nullptr, nullptr} {}
        void get(CefRefPtr<CefV8Value> &value) const final { value = CefV8Value::CreateUndefined(); }
        void get(const CefRefPtr<CefValue> &value) const final {}
    private:
        static VoidReturnArgument _instance;
    };

    class BoolReturnArgument final : public ReturnArgument
    {
    public:
        static auto instance() { return &_instance; }
        BoolReturnArgument() : ReturnArgument{"bool", &_value}, _value{} {}
        void get(CefRefPtr<CefV8Value> &value) const final { value = CefV8Value::CreateBool(_value); }
        void get(const CefRefPtr<CefValue> &value) const final { value->SetBool(_value); }
    private:
        static BoolReturnArgument _instance;
        bool _value;
    };

    class IntReturnArgument final : public ReturnArgument
    {
    public:
        static auto instance() { return &_instance; }
        IntReturnArgument() : ReturnArgument{"int", &_value}, _value{} {}
        void get(CefRefPtr<CefV8Value> &value) const final { value = CefV8Value::CreateInt(_value); }
        void get(const CefRefPtr<CefValue> &value) const final { value->SetInt(_value); }
    private:
        static IntReturnArgument _instance;
        int _value;
    };

    class DoubleReturnArgument final : public ReturnArgument
    {
    public:
        static auto instance() { return &_instance; }
        DoubleReturnArgument() : ReturnArgument{"double", &_value}, _value{} {}
        void get(CefRefPtr<CefV8Value> &value) const final { value = CefV8Value::CreateDouble(_value); }
        void get(const CefRefPtr<CefValue> &value) const final { value->SetDouble(_value); }
    private:
        static DoubleReturnArgument _instance;
        double _value;
    };

    class StringReturnArgument final : public ReturnArgument
    {
    public:
        static auto instance() { return &_instance; }
        StringReturnArgument() : ReturnArgument{"QString", &_value}, _value{} {}
        void get(CefRefPtr<CefV8Value> &value) const final { value = CefV8Value::CreateString(TO_CEF(_value)); }
        void get(const CefRefPtr<CefValue> &value) const final { value->SetString(TO_CEF(_value)); }
    private:
        static StringReturnArgument _instance;
        QString _value;
    };

    class ArrayReturnArgument final : public ReturnArgument
    {
    public:
        static auto instance() { return &_instance; }
        ArrayReturnArgument() : ReturnArgument{"QJsonArray", &_value}, _value{} {}
        void get(CefRefPtr<CefV8Value> &value) const final { value = qtToV8(_value); }
        void get(const CefRefPtr<CefValue> &value) const final { value->SetList(qtToCef(_value)); }
    private:
        static ArrayReturnArgument _instance;
        QJsonArray _value;
    };

    class ObjectReturnArgument final : public ReturnArgument
    {
    public:
        static auto instance() { return &_instance; }
        ObjectReturnArgument() : ReturnArgument{"QJsonObject", &_value}, _value{} {}
        void get(CefRefPtr<CefV8Value> &value) const final { value = qtToV8(_value); }
        void get(const CefRefPtr<CefValue> &value) const final { value->SetDictionary(qtToCef(_value)); }
    private:
        static ObjectReturnArgument _instance;
        QJsonObject _value;
    };
}

using namespace Qtcef;

constexpr auto ArgumentErrorMessage = "The parameter type of registered method must be one of bool, int, double or QString.";
constexpr auto ReturnArgumentErrorMessage = "The return type of registered method must be one of void, bool, int, double, QString, QJsonArray or QJsonObject.";

VoidArgument VoidArgument::_instance;
BoolArgument BoolArgument::_instance;
IntArgument IntArgument::_instance;
DoubleArgument DoubleArgument::_instance;
StringArgument StringArgument::_instance;
VoidReturnArgument VoidReturnArgument::_instance;
BoolReturnArgument BoolReturnArgument::_instance;
IntReturnArgument IntReturnArgument::_instance;
DoubleReturnArgument DoubleReturnArgument::_instance;
StringReturnArgument StringReturnArgument::_instance;
ArrayReturnArgument ArrayReturnArgument::_instance;
ObjectReturnArgument ObjectReturnArgument::_instance;

Argument *Argument::voidArgument()
{
    return VoidArgument::instance();
}

Argument *Argument::get(int type)
{
    switch (type) {
        case QMetaType::Bool: return BoolArgument::instance();
        case QMetaType::Int: return IntArgument::instance();
        case QMetaType::Double: return DoubleArgument::instance();
        case QMetaType::QString: return StringArgument::instance();
        default:
            Q_ASSERT_X(false, "Qtcef", ArgumentErrorMessage);
            return nullptr;
    }
}

Argument *Argument::create(int type)
{
    switch (type) {
        case QMetaType::Bool: return new BoolArgument;
        case QMetaType::Int: return new IntArgument;
        case QMetaType::Double: return new DoubleArgument;
        case QMetaType::QString: return new StringArgument;
        default:
            Q_ASSERT_X(false, "Qtcef", ArgumentErrorMessage);
            return nullptr;
    }
}

Argument::Argument(const char *name, const void *data)
        : QGenericArgument{name, data}
{
}

ReturnArgument *ReturnArgument::get(int type)
{
    switch (type) {
        case QMetaType::Void: return VoidReturnArgument::instance();
        case QMetaType::Bool: return BoolReturnArgument::instance();
        case QMetaType::Int: return IntReturnArgument::instance();
        case QMetaType::Double: return DoubleReturnArgument::instance();
        case QMetaType::QString: return StringReturnArgument::instance();
        case QMetaType::QJsonArray: return ArrayReturnArgument::instance();
        case QMetaType::QJsonObject: return ObjectReturnArgument::instance();
        default:
            Q_ASSERT_X(false, "Qtcef", ReturnArgumentErrorMessage);
            return nullptr;
    }
}

ReturnArgument *ReturnArgument::create(int type)
{
    switch (type) {
        case QMetaType::Void: return VoidReturnArgument::instance();
        case QMetaType::Bool: return new BoolReturnArgument;
        case QMetaType::Int: return new IntReturnArgument;
        case QMetaType::Double: return new DoubleReturnArgument;
        case QMetaType::QString: return new StringReturnArgument;
        case QMetaType::QJsonArray: return new ArrayReturnArgument;
        case QMetaType::QJsonObject: return new ObjectReturnArgument;
        default:
            Q_ASSERT_X(false, "Qtcef", ReturnArgumentErrorMessage);
            return nullptr;
    }
}

ReturnArgument::ReturnArgument(const char *name, void *data)
        : QGenericReturnArgument{name, data}
{
}
