#ifndef QKEYTOOLS_H
#define QKEYTOOLS_H

#include "qkeytools_global.h"
#include <QWidget>
#include <QMouseEvent>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QPlainTextEdit>
#include <QTextBrowser>
#include <QtSql>
#include <QPushButton>
#include <QTimer>

#include <QPropertyAnimation>

#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
#include <QDesktopWidget>
#endif

#if (QT_VERSION < QT_VERSION_CHECK(5,7,0))
#define nullptr 0
#endif

#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
#include <QScreen>
#endif

// define Animation Run time
const qint16 AnimationTime = 50;

namespace Ui {
class QkeyTools;
}

class QKEYTOOLSSHARED_EXPORT QkeyTools : public QWidget
{
    Q_OBJECT

#if (QT_VERSION >= QT_VERSION_CHECK(5,7,0))
    Q_PROPERTY(qint8 position READ position WRITE setPosition NOTIFY positionChanged)
    Q_PROPERTY(qint8 style READ style WRITE setStyle NOTIFY styleChanged)
    Q_PROPERTY(qint8 fontSize READ btnFontSize WRITE setBtnFontSize NOTIFY btnFontSizeChanged)
    Q_PROPERTY(qint8 labSize READ labSize WRITE setLabSize NOTIFY labSizeChanged)
    Q_PROPERTY(quint16 width READ width WRITE setWidth NOTIFY widthChanged)
    Q_PROPERTY(quint16 height READ height WRITE setHeight NOTIFY heightChanged)
    Q_PROPERTY(quint16 mainwindowOffset READ mainwindowOffset WRITE setMainwindowOffset NOTIFY mainwindowOffsetChanged)
    Q_PROPERTY(QString candidateWordsPath READ candidateWordsPath WRITE setCandidateWordsPath NOTIFY candidateWordsPathChanged)
    Q_PROPERTY(Language currentLanguage READ currentLanguage WRITE setCurrentLanguage NOTIFY currentLanguageChanged)
#endif

public:
    ~QkeyTools();

    enum Language {
        English,
        Capital,
        Chinese,
        Chinese_TW,
        Chinese_HK
    };
    Q_ENUM(Language)

    enum Position {
        CONTROL = 0, // 显示位置在控件的下方
        CENTER, //居中显示
        BOTTOM, //屏幕底部
        ProcessBottom,
        Embedded //同主程序显示位置对齐,嵌入式平台尽量设置此方式
    };
    Q_ENUM(Position)

    enum Style {
        BLUE = 0,           //淡蓝色
        DEV,                    //dev风格
        BLACK,               //黑色
        BROWN,            //灰黑色
        LIGHTGRAY,      //浅灰色
        DARKGRAY,      //深灰色
        GRAY,                 //灰色
        LIGHTYELLOW    //浅黄色
    };
    Q_ENUM(Style)

    //单例模式,保证一个程序只存在一个输入法实例对象
    static QkeyTools *getInstance();


    Q_INVOKABLE void Init(QkeyTools::Position position, QkeyTools::Style style, qint8 btnFontSize, qint8 labFontSize);

    // 设置键盘显示位置
    Q_INVOKABLE void setPosition(qint8 position);
    Q_INVOKABLE Position position();

    // 设置键盘颜色样式
    Q_INVOKABLE void setStyle(qint8 style);
    Q_INVOKABLE Style style();

    // 设置按钮字体的大小
    Q_INVOKABLE void setBtnFontSize(qint8 size);
    Q_INVOKABLE qint8 btnFontSize();

    // 设置待选字的大小
    Q_INVOKABLE void setLabSize(qint8 size);
    Q_INVOKABLE qint8 labSize();

    // 设置键盘的宽度
    Q_INVOKABLE void setWidth(quint16 width);
    Q_INVOKABLE quint16 width();

    //  设置键盘的高度
    Q_INVOKABLE void setHeight(quint16 height);
    Q_INVOKABLE quint16 height();

    //  设置输入法方式
    Q_INVOKABLE void setCurrentLanguage(Language language);
    Q_INVOKABLE Language currentLanguage();

    //  设置中文字库的路径
    Q_INVOKABLE void setCandidateWordsPath(QString path);
    Q_INVOKABLE QString candidateWordsPath();

    //  设置主窗口和软键盘的相对距离
    Q_INVOKABLE void setMainwindowOffset(quint16 offset);
    Q_INVOKABLE quint16 mainwindowOffset();

    /**
    * @name: setMainWindowObject
    * @descption: 用于键盘BOTTOMTOP方式显示时使用
    * 通过此函数可以实现键盘把主窗口位置往上移动
    * @author: zhangh
    * @date: 2019-05-14
    * @param: [QObject] obj 主窗口对象
    */
    void setMainWindowObject(QWidget *obj);

signals:
    void positionChanged(qint8 position);
    void styleChanged(qint8 style);
    void btnFontSizeChanged(qint8 size);
    void labSizeChanged(qint8 size);
    void widthChanged(quint16 width);
    void heightChanged(quint16 height);
    void currentLanguageChanged(Language language);
    void mainwindowOffsetChanged(quint16 offset);
    void candidateWordsPathChanged(QString candidateWordsPath);

protected:
    //事件过滤器,处理鼠标在汉字标签处单击操作
    bool eventFilter(QObject *obj, QEvent *event);
    //鼠标拖动事件
    void mouseMoveEvent(QMouseEvent *e);
    //鼠标按下事件
    void mousePressEvent(QMouseEvent *e);
    //鼠标松开事件
    void mouseReleaseEvent(QMouseEvent *);

private slots:
    //焦点改变事件槽函数处理
    void focusChanged(QWidget *oldWidget, QWidget *nowWidget);
    //输入法面板按键处理
    void btn_clicked();

    //定时器处理退格键
    void reClicked();

    void keyAnimationFinished();

    void windowAnimationFinished();

private:
    Ui::QkeyTools *ui;
    explicit QkeyTools(QWidget *parent = 0);
    static QkeyTools *_instance;     //实例对象

    int deskWidth;                  //桌面宽度
    int deskHeight;                 //桌面高度

    QPoint mousePoint;              //鼠标拖动自定义标题栏时的坐标
    bool mousePressed;              //鼠标是否按下

    bool isPress;                   //是否长按退格键
    QPushButton *btnPress;          //长按按钮
    QTimer *timerPress;             //退格键定时器
    bool checkPress();              //校验当前长按的按钮

    bool isFirst;                   //是否首次加载
    void InitForm();                //初始化窗体数据
    void InitProperty();            //初始化属性
    void ChangeStyle();             //改变样式
    void ChangeFont();              //改变字体大小
    void ShowPanel();               //显示输入法面板

    QWidget *currentWidget;         //当前焦点的对象
    QLineEdit *currentLineEdit;     //当前焦点的单行文本框
    QTextEdit *currentTextEdit;     //当前焦点的多行文本框
    QPlainTextEdit *currentPlain;   //当前焦点的富文本框
    QTextBrowser *currentBrowser;   //当前焦点的文本浏览框

    QString currentEditType;        //当前焦点控件的类型

    Position m_position;        //当前输入法面板位置类型
    Style m_style;           //当前输入法面板样式
    qint8 m_btnFontSize;                //当前输入法面板按钮字体大小
    qint8 m_labFontSize;                //当前输入法面板标签字体大小
    quint16 m_width;                // 当前键盘的宽度
    quint16 m_height;               // 当前键盘的高度
    Language m_currentLanguage;
    QString m_candidateWordsPath;
    quint16 m_mainwindowOffset;

    QWidget *m_mainwindow; // 主窗口对象，用于键盘BOTTOMTOP方式显示时使用
    QPropertyAnimation m_keyAnimation;  // 键盘动画
    QPropertyAnimation m_MainWindowAnimation;  //  主窗口动画
    QRect m_mainwindowPosition; //用于记录主窗口的默认位置信息
    bool m_isSetStyle ;  //用于记录是否已经设置了窗体的样式
    bool m_ishide ;  // 用于记录是否需要隐藏键盘


    void insertValue(QString value);//插入值到当前焦点控件
    void deleteValue();             //删除当前焦点控件的一个字符

    void changeType(Language type = English);  //改变输入法类型
    void changeLetter(bool isUpper);//改变字母大小写
    /**
    * @name: changeStyle
    * @descption: 改变输入法面板样式
    * @author: zhangh
    * @date: 2019-05-16
    * @param:  [widgetTopColor] [widgetBottomColor] [btnTopColor]  [btnBottomColor]  [btnBorderColor]  [textColor]
    * @param:  [面板顶部颜色]   [面板底部颜色]     [按钮顶部颜色]     [按钮底部颜色]   [按钮边框颜色]  [文字颜色]
    * @return:
    */
    void changeStyle(QString widgetTopColor, QString widgetBottomColor,
                     QString btnTopColor, QString btnBottomColor,
                     QString btnBorderColor, QString textColor);

    QList<QLabel *> labCh;           //汉字标签数组
    QStringList allPY;              //所有拼音链表
    QStringList currentPY;          //当前拼音链表
    int currentPY_index;            //当前拼音索引
    int currentPY_count;            //当前拼音数量

    void selectLanguage(Language language);         //查询汉字
    void showCandidateWords();            //显示查询到的汉字
    void setCandidateWords(int index);    //设置当前汉字
    void clearCandidateWords();           //清空当前汉字信息

    void initAnimation();  // 初始化动画参数
    void showAnimation();  // 键盘出现动画
    void hideAnimation();  // 键盘隐藏动画
    void resetAllWidget(); // 重置所有控件位置
};

#endif // QKEYTOOLS_H
