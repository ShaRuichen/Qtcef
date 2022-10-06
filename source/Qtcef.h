#ifndef QTCEF_H
#define QTCEF_H

#include <include/cef_app.h>
#include <include/base/cef_callback.h>
#include <include/wrapper/cef_closure_task.h>
#include <QtConcurrent>
#include <QtWidgets>

#ifndef CEF_STRING_TYPE_UTF16
#error Qtcef need define UTF-16 string type in Cef.
#endif

#define TO_CEF(string) reinterpret_cast<const char16_t *>(string.utf16())
#define FROM_CEF(string) QString::fromUtf16(reinterpret_cast<const char16_t *>(string.GetStruct()->str))

namespace Qtcef
{
    CefRefPtr<CefV8Value> qtToV8(const QJsonValue &value);
    CefRefPtr<CefV8Value> qtToV8(const QJsonArray &array);
    CefRefPtr<CefV8Value> qtToV8(const QJsonObject &object);

    CefRefPtr<CefValue> qtToCef(const QJsonValue &value);
    CefRefPtr<CefListValue> qtToCef(const QJsonArray &array);
    CefRefPtr<CefDictionaryValue> qtToCef(const QJsonObject &object);

    CefRefPtr<CefV8Value> cefToV8(const CefRefPtr<CefValue> &value);
    CefRefPtr<CefV8Value> cefToV8(const CefRefPtr<CefListValue> &list);
    CefRefPtr<CefV8Value> cefToV8(const CefRefPtr<CefDictionaryValue> &dictionary);
}

#endif // QTCEF_H
