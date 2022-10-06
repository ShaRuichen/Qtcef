#ifndef QTCEF_APP_H
#define QTCEF_APP_H

#include "Qtcef.h"

namespace Qtcef
{
    class App : public QObject
    {
        Q_OBJECT
        Q_DISABLE_COPY(App)
    public:
        static App *instance() { return _instance; }
        static const auto &metaMethods() { return _metaMethods; }
        int execute(int &argc, char *argv[]);
    protected:
        virtual void onQtInitialized() {}
        virtual void onCefInitialized() {}
    protected:
        static bool qtcef_register(const QMetaObject *metaObject);
        App() = default;
    private:
        static App *_instance;
        static CefMainArgs _mainArgs;
        static QHash<QString, QMetaMethod> _metaMethods;
        friend class BrowserApp;
    };
}

#endif // QTCEF_APP_H
