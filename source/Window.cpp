#include "Window.h"
#include "Client.h"

using namespace Qtcef;

const CefBrowserSettings Window::_browserSettings;

Window *Window::create(const QUrl &url, const QStringList &bridgeNames, const QJsonObject &data, QWidget *parent)
{
    const auto window = new Window{parent};
    CefWindowInfo info;
    info.SetAsChild(window->handle(), {});
    const auto cefExtraData = CefDictionaryValue::Create();
    const auto cefBridgeNames = CefListValue::Create();
    for (qsizetype i = 0; i < bridgeNames.count(); i++) {
        cefBridgeNames->SetString(i, TO_CEF(bridgeNames[i]));
    }
    cefExtraData->SetList(u"bridgeNames", cefBridgeNames);
    cefExtraData->SetDictionary(u"data", qtToCef(data));
    const CefString cefUrl = reinterpret_cast<const char16_t *>(url.toString().utf16());
    CefBrowserHost::CreateBrowser(info, new Client{window}, cefUrl, _browserSettings, cefExtraData, nullptr);

    return window;
}

void Window::dispatchEvent(const QString &type, const QJsonObject &event)
{
}

void Window::resizeCef()
{
    const auto virtualSize = size() * screen()->devicePixelRatio();
#ifdef Q_OS_WINDOWS
    MoveWindow(browserHost()->GetWindowHandle(), 0, 0, virtualSize.width(), virtualSize.height(), false);
#endif
}

void Window::showDevTool()
{
    auto devTool = _data->devTool;
    if (devTool == nullptr) {
        devTool = new Window{this};
        devTool->_data->isDevTool = true;
        CefWindowInfo info;
        info.SetAsChild(devTool->handle(), {});
        browserHost()->ShowDevTools(info, new Client{devTool}, _browserSettings, {});
        devTool->resize(800, 450);
        devTool->move(devTool->pos() - QPoint{400, 225});
        _data->devTool = devTool;
    }
    devTool->show();
    devTool->activateWindow();
}

void Window::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    if (_data->browser != nullptr) {
        resizeCef();
    }
}

void Window::closeEvent(QCloseEvent *event)
{
    if (_data->willClose) {
        const auto devTool = _data->devTool;
        if (devTool != nullptr) {
            devTool->_data->isDevTool = false;
        }
        _data->browser = nullptr;
        deleteLater();
        QMainWindow::closeEvent(event);
    } else {
        if (_data->isDevTool) {
            hide();
        } else {
            browserHost()->CloseBrowser(false);
        }
        event->ignore();
    }
}

Window::Window(QWidget *parent)
        : QMainWindow{parent}
        , _data{new Data}
{
}

Window::~Window()
{
    delete _data;
}
