#include "Client.h"
#include "App.h"
#include "Arguments.h"
#include "Window.h"

using namespace Qtcef;

Client::Client(Window *window)
    : _window{window}
{
}

bool Client::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
                                      CefRefPtr<CefFrame> frame,
                                      CefProcessId sourceProcess,
                                      CefRefPtr<CefProcessMessage> message)
{
    const auto name = message->GetName();
    const auto &metaMethods = App::metaMethods();
    const auto metaMethodIter = metaMethods.find(FROM_CEF(name));
    if (metaMethodIter == metaMethods.end()) return false;
    const auto &metaMethod = metaMethodIter.value();
    const auto argumentList = message->GetArgumentList();
    Argument *arguments[10];
    auto i = 0;
    for (; i < metaMethod.parameterCount(); i++) {
        arguments[i] = Argument::create(metaMethod.parameterType(i));
        arguments[i]->set(argumentList->GetValue(i));
    }
    for (; i < 10; i++) {
        arguments[i] = Argument::voidArgument();
    }
    ReturnArgument *returnArgument = ReturnArgument::get(metaMethod.returnType());
    metaMethod.invoke(App::instance(),
                      returnArgument->generic(),
                      arguments[0]->generic(),
                      arguments[1]->generic(),
                      arguments[2]->generic(),
                      arguments[3]->generic(),
                      arguments[4]->generic(),
                      arguments[5]->generic(),
                      arguments[6]->generic(),
                      arguments[7]->generic(),
                      arguments[8]->generic(),
                      arguments[9]->generic());
    const auto returnMessage = CefProcessMessage::Create(name);
    const auto returnArguments = returnMessage->GetArgumentList();
    const auto returnValue = CefValue::Create();
    returnArgument->get(returnValue);
    returnArguments->SetValue(0, returnValue);
    frame->SendProcessMessage(sourceProcess, returnMessage);
    return true;
}

inline bool isCTRLDown(const CefKeyEvent &event)
{
#if defined(OS_MAC)
    if (event.modifiers & EVENTFLAG_COMMAND_DOWN) return true;
#else
    if (event.modifiers & EVENTFLAG_CONTROL_DOWN) return true;
#endif
    return false;
}

bool Client::OnPreKeyEvent(CefRefPtr<CefBrowser> browser,
                           const CefKeyEvent &event,
                           MSG *osEvent,
                           bool *isKeyboardShortcut)
{
#ifdef QT_DEBUG
    if (_window->data()->isDevTool) return false;
    if (event.windows_key_code == 'R') {
        if (isCTRLDown(event)) {
            _window->data()->browser->Reload();
        }
    } else if (event.windows_key_code == 'I') {
        if (isCTRLDown(event) && event.modifiers & EVENTFLAG_SHIFT_DOWN) {
            _window->showDevTool();
        }
    }
#endif
    return false;
}

void Client::OnAfterCreated(CefRefPtr<CefBrowser> browser)
{
    _window->data()->browser = browser;
    _window->resizeCef();
}

bool Client::DoClose(CefRefPtr<CefBrowser> browser)
{
    _window->data()->willClose = true;
    return false;
}

void Client::OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode)
{
    emit _window->loaded();
}
