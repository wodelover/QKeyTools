# **QKeyTools**

**1、此代码是基于Qt平台下的软件盘实现，适合用于Qt4和Qt5.8以下的Qt版本；**
**注意：目前测试最低版本为Qt4.8.7，建议使用高版本的Qt**
**注意：中文使用，需要将默认的中文字体库放在程序的同一级目录下才可以**

**2、主要使用的场景：**<br>
  2.1、嵌入式平台；<br>
  2.2、桌面软键盘；<br>
  2.3、自定义输入软键盘；<br>

**3、实现的功能：**<br>
   3.1、能够自定义键盘的大小；<br>
   3.2、能够指定键盘的各种样式；<br>
   3.3、能够指定键盘的显示位置（提供按钮正下方、屏幕中间、屏幕底部）<br>
   3.4、针对嵌入式平台的键盘显示方式；<br>
  
**4、效果实现：**<br>
4.1、中文键盘实现
![中文键盘](https://github.com/wodelover/QKeyTools/blob/master/chinese.png)<br>
  
4.2、控件位置区域显示
![控件键盘](https://github.com/wodelover/QKeyTools/blob/master/control.png)<br>
  
4.3、不同样式显示
![控件键盘](https://github.com/wodelover/QKeyTools/blob/master/style1.png)<br>

**5、如何使用：**<br>
5.1、首先下载 QkeyTools 这个模块；<br>
5.2、然后以子目录工程库的方式将 QkeyTools 添加到你的项目；<br>
5.3、在main.cpp中添加如下代码：<br>
（1）嵌入式平台：<br>
```c++
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
    QkeyTools::getInstance()->setMainWindowObject(&w);
    QkeyTools::getInstance()->setWidth(800);
    QkeyTools::getInstance()->setHeight(300);
    QkeyTools::getInstance()->Init(QkeyTools::BOTTOM, QkeyTools::GRAY, 10, 10);

    return a.exec();
}
```
（2）windows或者linux平台<br>
```c++
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
**备注： 两个平台最大的差异是嵌入式平台需要设置主程序句柄才可以正常运行，否则会出错，请格外注意**
```QkeyTools::getInstance()->setMainWindowObject(&w);```

**联系方式：543985125@qq.com**

