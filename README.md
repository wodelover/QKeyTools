# QKeyTools

**1、此代码是基于Qt平台下的软件盘实现，适合用于Qt4和Qt5.8以下的Qt版本；**

**2、主要使用的场景：**
 2.1、嵌入式平台；
 2.2、桌面软键盘；
 2.3、自定义输入软键盘；

**3、实现的功能：
3.1、能够自定义键盘的大小；
3.2、能够指定键盘的各种样式；
3.3、能够指定键盘的显示位置（提供按钮正下方、屏幕中间、屏幕底部）
3.4、针对嵌入式平台的键盘显示方式；
  
**4、效果实现：
4.1、中文键盘实现
![中文键盘](https://github.com/wodelover/QKeyTools/blob/master/chinese.png)
  
**4.2、控件位置区域显示
![控件键盘](https://github.com/wodelover/QKeyTools/blob/master/control.png)
  
**4.3、不同样式显示
![控件键盘](https://github.com/wodelover/QKeyTools/blob/master/style1.png)

**5、如何使用：
5.1、首先下载 QkeyTools 这个模块；
5.2、然后以子目录工程库的方式将 QkeyTools 添加到你的项目；
5.3、在main.cpp中添加如下代码：
**（1）嵌入式平台：
```
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
```
**（2）windows或者linux平台
```
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
    QkeyTools::getInstance()->setWidth(1024);
    QkeyTools::getInstance()->setHeight(300);
    QkeyTools::getInstance()->Init(QkeyTools::Embedded, QkeyTools::LIGHTYELLOW, 20, 20);

    return a.exec();
}
```
**备注： 两个平台最大的差异是嵌入式平台需要设置主程序句柄才可以正常运行，否则会出错，请格外注意
```QkeyTools::getInstance()->setMainWindowObject(&w);```

**联系方式：543985125@qq.com

