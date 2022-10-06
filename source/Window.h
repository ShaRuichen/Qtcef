#ifndef QTCEF_WINDOW_H
#define QTCEF_WINDOW_H

#include "Qtcef.h"

namespace Qtcef
{
    class Client;

    class Window : public QMainWindow
    {
        Q_OBJECT
    public:
        struct Data
        {
            CefRefPtr<CefBrowser> browser;
            Window *devTool{};
            int id{};
            bool isDevTool{};
            bool willClose{};
        };
    public:
        static Window *create(const QUrl &url, const QStringList &bridgeNames = {}, const QJsonObject &data = {}, QWidget *parent = {});
        void dispatchEvent(const QString &type, const QJsonObject &event);
    public:
        [[nodiscard]] auto data() const { return _data; }
        [[nodiscard]] auto handle() const { return reinterpret_cast<CefWindowHandle>(winId()); }
        [[nodiscard]] auto browserHost() const { return _data->browser->GetHost(); }
        void resizeCef();
        void showDevTool();
    signals:
        void loaded();
    protected:
        void resizeEvent(QResizeEvent *event) override;
        void closeEvent(QCloseEvent *event) override;
    private:
        explicit Window(QWidget *parent);
        ~Window() override;
    private:
        static const CefBrowserSettings _browserSettings;
        Data * const _data;
    };
}

#endif // QTCEF_WINDOW_H
