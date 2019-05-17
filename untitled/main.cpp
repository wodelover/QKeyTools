#include "mainwindow.h"
#include <QApplication>
#include <QTextCodec>
#include <qkeytools.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;
    w.show();

#if (QT_VERSION <= QT_VERSION_CHECK(5,0,0))
    //QT5版本以上默认就是采用UTF-8编码
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForLocale(codec);
    QTextCodec::setCodecForCStrings(codec);
    QTextCodec::setCodecForTr(codec);
#endif
    QkeyTools::getInstance()->setMainWindowObject(&w);
    QkeyTools::getInstance()->setWidth(1024);
    QkeyTools::getInstance()->setHeight(300);
    QkeyTools::getInstance()->Init(QkeyTools::Embedded, QkeyTools::LIGHTYELLOW, 20, 20);

    return a.exec();
}
