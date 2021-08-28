#include "olympicmainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QSplashScreen>
#include <QTimer>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QPixmap pixmap(":/image/res/splash.jpg");
    QSplashScreen splash(pixmap);
    splash.show();
    QEventLoop eventloop;
    QTimer::singleShot(1000, &eventloop, SLOT(quit()));
    splash.showMessage("正在加载主界面");
    QTimer::singleShot(1000, &eventloop, SLOT(quit()));
    splash.showMessage("正在加载表格组件");
    eventloop.exec();

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "olympic_qt_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }
    OlympicMainWindow w;
    w.show();
    return a.exec();
}
