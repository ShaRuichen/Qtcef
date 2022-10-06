#include "App.h"
#include "RendererApp.h"

namespace Qtcef
{
    class BrowserApp : public CefApp, public CefBrowserProcessHandler
    {
    public:
        CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() override { return this; }
        void OnContextInitialized() override { App::instance()->onCefInitialized(); }
    IMPLEMENT_REFCOUNTING(BrowserApp);
    };
}

using namespace Qtcef;

App *App::_instance;
CefMainArgs App::_mainArgs;
QHash<QString, QMetaMethod> App::_metaMethods;

int App::execute(int &argc, char *argv[])
{
    const auto commandLine = CefCommandLine::CreateCommandLine();
#ifdef OS_WIN
    commandLine->InitFromString(GetCommandLine());
    _mainArgs = CefMainArgs{GetModuleHandle(nullptr)};
#else
    commandLine->InitFromArgv(argc, argv);
    _mainArgs = CefMainArgs{argc, argv};
#endif
    const auto processType = commandLine->GetSwitchValue(u"type");
    if (processType.empty()) {
        QApplication app{argc, argv};
        onQtInitialized();
        for (auto i = App::staticMetaObject.methodCount(); i < metaObject()->methodCount(); i++) {
            const auto metaMethod = metaObject()->method(i);
            _metaMethods.insert(metaMethod.name(), metaMethod);
        }
        CefEnableHighDPISupport();
        CefSettings settings;
        settings.no_sandbox = true;
        auto result = CefInitialize(_mainArgs, settings, new BrowserApp, nullptr);
        Q_ASSERT_X(result, "Qtcef::App", "Failed to initialize Cef.");
        connect(&app, &QApplication::aboutToQuit, CefShutdown);
        QTimer timer;
        connect(&timer, &QTimer::timeout, CefDoMessageLoopWork);
        timer.start(10);
        return QApplication::exec();
    } else if (processType == u"renderer") {
        QCoreApplication app{argc, argv};
        QTimer::singleShot(0, []
        {
            const auto exitCode = CefExecuteProcess(_mainArgs, new RendererApp, nullptr);
            QCoreApplication::exit(exitCode);
        });
        return QCoreApplication::exec();
    }
    return CefExecuteProcess(_mainArgs, nullptr, nullptr);
}

bool App::qtcef_register(const QMetaObject *metaObject)
{
    Q_ASSERT_X(_instance == nullptr, "Qtcef::App", "Can not register more than one App.");
    const auto object = metaObject->newInstance();
    Q_ASSERT_X(object != nullptr, "Qtcef::App", "Registered App must has a Q_INVOKABLE constructor with 0 parameter.");
    _instance = qobject_cast<App *>(object);
    Q_ASSERT_X(_instance != nullptr, "Qtcef::App", "Class registered by QTCEF_APP must be a subclass of Qtcef::App.");
    return false;
}

int main(int argc, char *argv[])
{
    const auto app = App::instance();
    Q_ASSERT_X(app != nullptr, "Qtcef", "Must register a App of Qtcef by QTCEF_APP.");
    return app->execute(argc, argv);
}
