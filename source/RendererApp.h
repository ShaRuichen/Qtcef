#ifndef QTCEF_RENDERER_APP_H
#define QTCEF_RENDERER_APP_H

#include "Qtcef.h"

namespace Qtcef
{
    class AppHandler;

    class RendererApp : public CefApp, public CefRenderProcessHandler
    {
    public:
        CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() override { return this; }
        void OnBrowserCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefDictionaryValue> extraInfo) override;
        void OnContextCreated(CefRefPtr<CefBrowser> browser,
                              CefRefPtr<CefFrame> frame,
                              CefRefPtr<CefV8Context> context) override;
        bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
                                      CefRefPtr<CefFrame> frame,
                                      CefProcessId sourceProcess,
                                      CefRefPtr<CefProcessMessage> message) final;
    private:
        void doEventLoop();
    private:
        CefRefPtr<CefListValue> _bridgeNames;
        CefRefPtr<CefDictionaryValue> _data;
        QEventLoop _loop;
        base::RepeatingCallback<void()> _callback;
        AppHandler *_appHandler;
    IMPLEMENT_REFCOUNTING(RendererApp);
    };
}

#endif // QTCEF_RENDERER_APP_H
