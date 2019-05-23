#ifndef QKEYTOOLS_H
#define QKEYTOOLS_H

#include "qkeytools_global.h"
#include <QDialog>
#include <QMouseEvent>
#include <QLabel>
#include <QLineEdit>
//#include <QComboBox>
#include <QTextEdit>
#include <QPlainTextEdit>
#include <QTextBrowser>
#include <QtSql>
#include <QPushButton>
#include <QTimer>
#include <QQmlEngine>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>

// 定义动画运行时间
const qint16 AnimationTime = 50;

#ifdef Q_OS_WIN
// 设置中文词库路径
const QString  ChineseWordLibPath = "D:\\build-WSJTNTP-Desktop_Qt_5_12_2_MinGW_32_bit-Debug\\src\\chinesewordlib.db";
#endif

#ifdef Q_OS_UNIX
// 设置中文词库路径
const QString  ChineseWordLibPath = "/mnt/userdata/chinesewordlib.db";
#endif

namespace Ui {
class QkeyTools;
}

class QKEYTOOLSSHARED_EXPORT QkeyTools : public QDialog
{
    Q_OBJECT

    Q_PROPERTY(qint8 position READ position WRITE setPosition NOTIFY positionChanged)
    Q_PROPERTY(qint8 style READ style WRITE setStyle NOTIFY styleChanged)
    Q_PROPERTY(qint8 fontSize READ btnFontSize WRITE setBtnFontSize NOTIFY btnFontSizeChanged)
    Q_PROPERTY(qint8 labSize READ labSize WRITE setLabSize NOTIFY labSizeChanged)
    Q_PROPERTY(quint16 width READ width WRITE setWidth NOTIFY widthChanged)
    Q_PROPERTY(quint16 height READ height WRITE setHeight NOTIFY heightChanged)
    Q_PROPERTY(qint8 inputMode READ inputMode WRITE setInputMode NOTIFY inputModeChanged)
    Q_PROPERTY(QString chineseWordLibPath READ chineseWordLibPath WRITE setChineseWordLibPath)
    Q_PROPERTY(quint16 mainwindowOffset READ mainwindowOffset WRITE setMainwindowOffset NOTIFY mainwindowOffsetChanged)

public:
    ~QkeyTools();

    enum Position{
        CONTROL = 0, // 显示位置在控件的下方
        CENTER, //居中显示
        BOTTOM, //屏幕底部
        Embedded //同主程序显示位置对齐,嵌入式平台尽量设置此方式
    };

    enum Style{
        BLUE=0,             //淡蓝色
        DEV,                    //dev风格
        BLACK,               //黑色
        BROWN,            //灰黑色
        LIGHTGRAY,      //浅灰色
        DARKGRAY,      //深灰色
        GRAY,                 //灰色
        LIGHTYELLOW    //浅黄色
    };

    enum InputMode{
        MIN = 0,        // 小写
        MAX,              // 大写
        CHINESE       // 中文
    };

    Q_ENUM(Position)
    Q_ENUM(Style)

    //单例模式,保证一个程序只存在一个输入法实例对象
    static QkeyTools *getInstance() {
        if (!_instance) {
            _instance = new QkeyTools;
        }
        return _instance;
    }
    static QObject* QkeyTools_singletontype_provider(QQmlEngine *engine, QJSEngine *scriptEngine);

    Q_INVOKABLE void Init(QkeyTools::Position position, QkeyTools::Style style, qint8 btnFontSize, qint8 labFontSize);

    // 设置键盘显示位置
    Q_INVOKABLE void setPosition(qint8 _position);
    Q_INVOKABLE Position position();

    // 设置键盘颜色样式
    Q_INVOKABLE void setStyle(qint8 _style);
    Q_INVOKABLE Style style();

    // 设置按钮字体的大小
    Q_INVOKABLE void setBtnFontSize(qint8 size);
    Q_INVOKABLE qint8 btnFontSize();

    // 设置待选字的大小
    Q_INVOKABLE void setLabSize(qint8 size);
    Q_INVOKABLE qint8 labSize();

    // 设置键盘的宽度
    Q_INVOKABLE void setWidth(quint16 _width);
    Q_INVOKABLE quint16 width();

    //  设置键盘的高度
    Q_INVOKABLE void setHeight(quint16 _height);
    Q_INVOKABLE quint16 height();

    //  设置输入法方式
    Q_INVOKABLE void setInputMode(qint8 _mode);
    Q_INVOKABLE InputMode inputMode();

    //  设置中文字库的路径
    Q_INVOKABLE void setChineseWordLibPath(QString path);
    Q_INVOKABLE QString chineseWordLibPath();

    //  设置主窗口和软键盘的相对距离
    Q_INVOKABLE void setMainwindowOffset(quint16 offset);
    Q_INVOKABLE quint16 mainwindowOffset();
    /**
   * @name: setMainWindowObject
   * @descption: 用于键盘BOTTOMTOP方式显示时使用
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
    void inputModeChanged(qint8 mode);
    void mainwindowOffsetChanged(quint16 offset);
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
    explicit QkeyTools(QDialog *parent = nullptr);
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

    Position m_position = Position::CONTROL;        //当前输入法面板位置类型
    Style m_style = Style::GRAY;           //当前输入法面板样式
    qint8 m_btnFontSize = 10;                //当前输入法面板按钮字体大小
    qint8 m_labFontSize = 10;                //当前输入法面板标签字体大小
    quint16 m_width = 800;                // 当前键盘的宽度
    quint16 m_height = 250;               // 当前键盘的高度
    InputMode m_inputMode = InputMode::MIN;  // 设置当前输入法的输入方式
    QWidget *m_mainwindow; // 主窗口对象，用于键盘BOTTOMTOP方式显示时使用
    QPropertyAnimation m_keyAnimation;  // 键盘动画
    QPropertyAnimation m_MainWindowAnimation;  //  主窗口动画
    //    QParallelAnimationGroup m_groupAnimation;   //  动画组
    QRect m_mainwindowPosition; //用于记录主窗口的默认位置信息
    bool m_isSetStyle = false;  //用于记录是否已经设置了窗体的样式
    bool m_ishide = false;  // 用于记录是否需要隐藏键盘
    QString m_chineseWordLibPath;
    quint16 m_mainwindowOffset = 0;

    void insertValue(QString value);//插入值到当前焦点控件
    void deleteValue();             //删除当前焦点控件的一个字符

    void changeType(InputMode type = InputMode::MIN);  //改变输入法类型
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
    void selectChinese();           //查询汉字
    void showChinese();             //显示查询到的汉字
    void setChinese(int index);     //设置当前汉字
    void clearChinese();            //清空当前汉字信息

    void initAnimation();  // 初始化动画参数
    void showAnimation();  // 键盘出现动画
    void hideAnimation();  // 键盘隐藏动画
    void resetAllWidget(); // 重置所有控件位置
};

#endif // QKEYTOOLS_H
