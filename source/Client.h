#ifndef QTCEF_CLIENT_H
#define QTCEF_CLIENT_H

#include "Qtcef.h"

namespace Qtcef
{
    class Window;

    class Client final
            : public CefClient
            , public CefKeyboardHandler
            , public CefLifeSpanHandler
            , public CefLoadHandler
    {
    public:
        explicit Client(Window *window);
        bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
                                      CefRefPtr<CefFrame> frame,
                                      CefProcessId sourceProcess,
                                      CefRefPtr<CefProcessMessage> message) final;
        CefRefPtr<CefKeyboardHandler> GetKeyboardHandler() final { return this; }
        bool OnPreKeyEvent(CefRefPtr<CefBrowser> browser,
                           const CefKeyEvent &event,
                           CefEventHandle osEvent,
                           bool *isKeyboardShortcut) final;
        CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() final { return this; }
        void OnAfterCreated(CefRefPtr<CefBrowser> browser) final;
        bool DoClose(CefRefPtr<CefBrowser> browser) final;
        CefRefPtr<CefLoadHandler> GetLoadHandler() final { return this; }
        void OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode) final;
    private:
        Window *_window;
    IMPLEMENT_REFCOUNTING(Client);
    };
}

#endif // QTCEF_CLIENT_H
