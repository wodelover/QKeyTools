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
    // 设置键盘的字体大小
    QFont sfont;
    sfont.setPointSize(16);
    qApp->setFont(sfont);

    // 设置键盘的父对象、宽度、高度、以及默认的样式设置
//    QkeyTools::getInstance()->setMainWindowObject(&w);
    QkeyTools::getInstance()->setWidth(800);
    QkeyTools::getInstance()->setHeight(300);
    QkeyTools::getInstance()->Init(QkeyTools::BOTTOM, QkeyTools::GRAY, 10, 10);

    return a.exec();
}
