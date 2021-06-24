#include "qkeytools.h"
#include "ui_qkeytools.h"

#include <QCoreApplication>
#include <QFileInfo>
#include <QFontMetrics>

QkeyTools *QkeyTools::_instance = 0;

QkeyTools::QkeyTools(QWidget *parent) :
    QWidget(parent)
    , ui(new Ui::QkeyTools)
    , m_position(UnderWidget)
    , m_style(GRAY)
    , m_btnFontSize(8)
    , m_labFontSize(8)
    , m_width(800)
    , m_height(250)
    , m_currentLanguage(English)
    , m_mainwindowOffset(10)
{
    ui->setupUi(this);

    this->setWindowFlags(Qt::WindowStaysOnTopHint);

    setCandidateWordsPath(QApplication::applicationDirPath() +  "/CandidateWords.db");
    this->InitProperty();
    this->InitForm();
    this->ChangeStyle();
}

QkeyTools::~QkeyTools()
{
    delete ui;
}

QkeyTools *QkeyTools::getInstance()
{
    if (!_instance) {
        _instance = new QkeyTools;
    }
    return _instance;
}

void QkeyTools::Init(QkeyTools::Position position, QkeyTools::Style style, qint8 btnFontSize, qint8 labFontSize)
{
    // [0] 配置键盘界面
    this->setBtnFontSize(btnFontSize);
    this->setLabSize(labFontSize);
    this->setStyle(style);
    this->setPosition(position);

    // [1] 初始化动画设置
    initAnimation();
}

void QkeyTools::setPosition(qint8 position)
{
    if (position != m_position) {
        m_position = static_cast<Position>(position);
        emit positionChanged(m_position);
    }
}

QkeyTools::Position QkeyTools::position()
{
    return m_position;
}

void QkeyTools::setStyle(qint8 style)
{
    if (style != m_style) {
        m_style = static_cast<Style>(style);
        emit styleChanged(m_style);
        this->ChangeStyle();
    }
}

QkeyTools::Style QkeyTools::style()
{
    return m_style;
}

void QkeyTools::setBtnFontSize(qint8 size)
{
    if (m_btnFontSize != size) {
        m_btnFontSize = size;
        emit btnFontSizeChanged(m_btnFontSize);
        this->ChangeFont();
    }
}

qint8 QkeyTools::btnFontSize()
{
    return m_btnFontSize;
}

void QkeyTools::setLabSize(qint8 size)
{
    if (size != m_labFontSize) {
        m_labFontSize = size;
        emit labSizeChanged(m_labFontSize);
        this->ChangeFont();
    }
}

qint8 QkeyTools::labSize()
{
    return m_labFontSize;
}

void QkeyTools::setWidth(quint16 width)
{
    if (width != m_width) {
        m_width = width;
        emit widthChanged(m_width);
    }
}

quint16 QkeyTools::width()
{
    return m_width;
}

void QkeyTools::setHeight(quint16 height)
{
    if (height != m_height) {
        m_height = height;
        emit heightChanged(m_height);
    }
}

quint16 QkeyTools::height()
{
    return m_height;
}

void QkeyTools::setCurrentLanguage(Language language)
{
    if (language != m_currentLanguage) {
        m_currentLanguage = static_cast<Language>(language);
        emit currentLanguageChanged(m_currentLanguage);
    }
}

QkeyTools::Language QkeyTools::currentLanguage()
{
    return m_currentLanguage;
}

void QkeyTools::setCandidateWordsPath(QString path)
{
    QFileInfo fileInfo(path);
    if (!fileInfo.exists()) {
        QString msg = "Database isn't exists,set CandidateWordsPath is: " + path;
        qWarning() << msg;
        return;
    }

    if (path != m_candidateWordsPath) {
        m_candidateWordsPath = path;
        emit candidateWordsPathChanged(m_candidateWordsPath);

        QSqlDatabase DbConn;
        DbConn = QSqlDatabase::addDatabase("QSQLITE", m_candidateWordsPath);
        DbConn.setDatabaseName(m_candidateWordsPath);
        if (!DbConn.open()) {
            qWarning("OPen CandidateWords Database failed,DB file is right?");
        }
    }
}

QString QkeyTools::candidateWordsPath()
{
    return m_candidateWordsPath;
}

void QkeyTools::setMainwindowOffset(quint16 offset)
{
    if (m_mainwindowOffset != offset) {
        m_mainwindowOffset = offset;
        emit mainwindowOffsetChanged(m_mainwindowOffset);
    }
}

quint16 QkeyTools::mainwindowOffset()
{
    return m_mainwindowOffset;
}

void QkeyTools::setMainWindowObject(QWidget *obj)
{
    m_mainwindow = obj;
    m_mainwindowPosition = m_mainwindow->geometry();
}

void QkeyTools::mouseMoveEvent(QMouseEvent *e)
{
    if (m_position != Embedded) {
        if (mousePressed && (e->buttons() == Qt::LeftButton)) {
            this->move(e->globalPos() - mousePoint);
            e->accept();
        }
    }
}

void QkeyTools::mousePressEvent(QMouseEvent *e)
{
    if (m_position != Embedded) {
        if (e->button() == Qt::LeftButton) {
            mousePressed = true;
            mousePoint = e->globalPos() - this->pos();
            e->accept();
        }
    }
}

void QkeyTools::mouseReleaseEvent(QMouseEvent *e)
{
    Q_UNUSED(e)
    if (m_position != Embedded) {
        mousePressed = false;
    }
}

void QkeyTools::showEvent(QShowEvent *event)
{
    Q_UNUSED(event)
    QFontMetrics fm(qApp->font());
#if (QT_VERSION <= QT_VERSION_CHECK(5,11,0))
    int min_width = fm.width(ui->CloseBtn->text());
    int min_height = fm.height();
#else
    int min_width = fm.horizontalAdvance(ui->CloseBtn->text());
    int min_height = fm.height() * 2;
#endif
    int max_width = min_width * 2;
    int max_height = min_height * 1.5;
    ui->PageUpBtn->setMinimumSize(min_width, min_height);
    ui->PageUpBtn->setMaximumSize(max_width, max_height);
    ui->PageDownBtn->setMinimumSize(min_width, min_height);
    ui->PageDownBtn->setMaximumSize(max_width, max_height);
    ui->CloseBtn->setMinimumSize(min_width, min_height);
    ui->CloseBtn->setMaximumSize(max_width, max_height);
    event->accept();
}

void QkeyTools::InitForm()
{
    this->mousePressed = false;
    this->setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);

#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
    QDesktopWidget w;
    deskWidth = w.availableGeometry().width();
    deskHeight = w.availableGeometry().height();
#endif

#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
    QScreen *screens = QGuiApplication::screens().first();
    deskWidth = screens->availableSize().width();
    deskHeight = screens->availableSize().height();
#endif

    isFirst = true;
    isPress = false;
    timerPress = new QTimer(this);
    connect(timerPress, SIGNAL(timeout()), this, SLOT(reClicked()));

    currentWidget = nullptr;
    currentLineEdit = nullptr;
    currentTextEdit = nullptr;
    currentPlain = nullptr;
    currentBrowser = nullptr;
    currentEditType = "";

    //如果需要更改输入法初始模式,改变currentType这个变量即可
    changeType();

    QList<QPushButton *> btn = this->findChildren<QPushButton *>();
    foreach (QPushButton *b, btn) {
        connect(b, SIGNAL(clicked()), this, SLOT(handleButtonClicked()));
    }

    //绑定全局改变焦点信号槽
    connect(qApp, SIGNAL(focusChanged(QWidget *, QWidget *)),
            this, SLOT(focusChanged(QWidget *, QWidget *)));
    //绑定按键事件过滤器
    qApp->installEventFilter(this);
}

void QkeyTools::InitProperty()
{
    ui->btnOther1->setProperty("btnOther", true);
    ui->btnOther2->setProperty("btnOther", true);
    ui->btnOther3->setProperty("btnOther", true);
    ui->btnOther4->setProperty("btnOther", true);
    ui->btnOther5->setProperty("btnOther", true);
    ui->btnOther6->setProperty("btnOther", true);
    ui->btnOther7->setProperty("btnOther", true);
    ui->btnOther8->setProperty("btnOther", true);
    ui->btnOther9->setProperty("btnOther", true);
    ui->btnOther10->setProperty("btnOther", true);
    ui->btnOther11->setProperty("btnOther", true);
    ui->btnOther12->setProperty("btnOther", true);
    ui->btnOther13->setProperty("btnOther", true);
    ui->btnOther14->setProperty("btnOther", true);
    ui->btnOther15->setProperty("btnOther", true);
    ui->btnOther16->setProperty("btnOther", true);
    ui->btnOther17->setProperty("btnOther", true);
    ui->btnOther18->setProperty("btnOther", true);
    ui->btnOther19->setProperty("btnOther", true);
    ui->btnOther20->setProperty("btnOther", true);
    ui->btnOther21->setProperty("btnOther", true);

    ui->btnDot->setProperty("btnOther", true);
    ui->SpaceBtn->setProperty("btnOther", true);
    ui->DeleteBtn->setProperty("btnOther", true);

    ui->btn0->setProperty("btnNum", true);
    ui->btn1->setProperty("btnNum", true);
    ui->btn2->setProperty("btnNum", true);
    ui->btn3->setProperty("btnNum", true);
    ui->btn4->setProperty("btnNum", true);
    ui->btn5->setProperty("btnNum", true);
    ui->btn6->setProperty("btnNum", true);
    ui->btn7->setProperty("btnNum", true);
    ui->btn8->setProperty("btnNum", true);
    ui->btn9->setProperty("btnNum", true);
    ui->btn00->setProperty("btnNum", true);

    ui->btna->setProperty("btnLetter", true);
    ui->btnb->setProperty("btnLetter", true);
    ui->btnc->setProperty("btnLetter", true);
    ui->btnd->setProperty("btnLetter", true);
    ui->btne->setProperty("btnLetter", true);
    ui->btnf->setProperty("btnLetter", true);
    ui->btng->setProperty("btnLetter", true);
    ui->btnh->setProperty("btnLetter", true);
    ui->btni->setProperty("btnLetter", true);
    ui->btnj->setProperty("btnLetter", true);
    ui->btnk->setProperty("btnLetter", true);
    ui->btnl->setProperty("btnLetter", true);
    ui->btnm->setProperty("btnLetter", true);
    ui->btnn->setProperty("btnLetter", true);
    ui->btno->setProperty("btnLetter", true);
    ui->btnp->setProperty("btnLetter", true);
    ui->btnq->setProperty("btnLetter", true);
    ui->btnr->setProperty("btnLetter", true);
    ui->btns->setProperty("btnLetter", true);
    ui->btnt->setProperty("btnLetter", true);
    ui->btnu->setProperty("btnLetter", true);
    ui->btnv->setProperty("btnLetter", true);
    ui->btnw->setProperty("btnLetter", true);
    ui->btnx->setProperty("btnLetter", true);
    ui->btny->setProperty("btnLetter", true);
    ui->btnz->setProperty("btnLetter", true);

    labCh.append(ui->labCh0);
    labCh.append(ui->labCh1);
    labCh.append(ui->labCh2);
    labCh.append(ui->labCh3);
    labCh.append(ui->labCh4);
    labCh.append(ui->labCh5);
    labCh.append(ui->labCh6);
    labCh.append(ui->labCh7);
    labCh.append(ui->labCh8);
    labCh.append(ui->labCh9);
    for (int i = 0; i < 10; i++) {
        labCh[i]->installEventFilter(this);
    }
}

void QkeyTools::ShowPanel(QWidget *nowWidget)
{
    setVisible(true);
    // 设置键盘显示位置
    switch (m_position) {
    case UnderWidget: { // 控件下方,左右相对程序对齐
        QRect rect = nowWidget->rect();
        QPoint pos = QPoint(rect.left(), rect.bottom() + 2);
        pos = nowWidget->mapToGlobal(pos);
        QPoint pos1 = QPoint((deskWidth - m_width) / 2, deskHeight / 2 - m_height / 2);
        this->setGeometry(pos1.x(), pos.y(), m_width, m_height);
        break;
    }
    case ScreenCenter: { // 程序中间
        QPoint pos = QPoint((deskWidth - m_width) / 2, deskHeight / 2 - m_height / 2);
        this->setGeometry(pos.x(), pos.y(), m_width, m_height);
        break;
    }
    case ScreenBottom: { // 屏幕底部
        m_keyAnimation.setStartValue(QRect((deskWidth - m_width) / 2, deskHeight, m_width, m_height));
        m_keyAnimation.setEndValue(QRect((deskWidth - m_width) / 2, deskHeight - m_height, m_width, m_height));
        showAnimation();
        break;
    }
    case Embedded: { //覆盖在主程序的底部
        if (m_mainwindow == nullptr) {
            // 如果没有设置主程序对象
            m_keyAnimation.setStartValue(QRect((deskWidth - m_width) / 2, deskHeight, m_width, m_height));
            m_keyAnimation.setEndValue(QRect((deskWidth - m_width) / 2, deskHeight - m_height, m_width, m_height));
            qDebug() << __FILE__ << __LINE__ << "main window is nullptr on platform: Embedded";
            showAnimation();
        } else {
            // 用于保存主窗口对象的位置大小信息
            QRect mainwindowRec;
            mainwindowRec = m_mainwindow->geometry();
            int x = mainwindowRec.x();
            int width = mainwindowRec.width();
            int height = mainwindowRec.height();

            //  获取当前选中控件的位置信息
            int nowUnderWidgetH = nowWidget->geometry().height();

            //  采用循环计算当前控件相对于主程序的位置
            QWidget *temp = nowWidget;
            int  nowUnderWidgetYOffset = nowWidget->geometry().y() + m_mainwindowOffset;
            while (temp->parent()) {
                nowUnderWidgetYOffset += static_cast<QWidget *>(temp->parent())->geometry().y();
                temp = static_cast<QWidget *>(temp->parent());
            }

            // 如果控件被键盘遮挡
            if (nowUnderWidgetH + nowUnderWidgetYOffset > deskHeight - m_height) {
                m_MainWindowAnimation.setStartValue(mainwindowRec);
                m_MainWindowAnimation.setEndValue(QRect(x, m_height - nowUnderWidgetH - nowUnderWidgetYOffset, width, height));
            } else {
                m_MainWindowAnimation.setStartValue(mainwindowRec);
                m_MainWindowAnimation.setEndValue(mainwindowRec);
            }

            //                m_keyAnimation.setStartValue(QRect(x +(width - m_width)/2, y + height, m_width, m_height));
            //                m_keyAnimation.setEndValue(QRect(x +(width - m_width)/2, y + height - m_height, m_width, m_height));
            m_keyAnimation.setStartValue(QRect((deskWidth - m_width) / 2, deskHeight, m_width, m_height));
            m_keyAnimation.setEndValue(QRect((deskWidth - m_width) / 2, deskHeight - m_height, m_width, m_height));
            showAnimation();
        }
        break;
    }
    case ProcessBottom: {
        if (m_mainwindow == nullptr) {
            // 如果没有设置主程序对象
            m_keyAnimation.setStartValue(QRect((deskWidth - m_width) / 2, deskHeight, m_width, m_height));
            m_keyAnimation.setEndValue(QRect((deskWidth - m_width) / 2, deskHeight - m_height, m_width, m_height));
            qDebug() << __FILE__ << __LINE__ << "main window is nullptr on platform: Embedded";
            showAnimation();
        }

        //  获取当前选中控件的位置信息
        int nowUnderWidgetH = nowWidget->geometry().height();

        //  采用循环计算当前控件相对于主程序的位置
        QWidget *temp = nowWidget;
        int  nowUnderWidgetYOffset = nowWidget->geometry().y();
        while (temp->parent()) {
            nowUnderWidgetYOffset += static_cast<QWidget *>(temp->parent())->geometry().y();
            temp = static_cast<QWidget *>(temp->parent());
        }
        // 用于保存主窗口对象的位置大小信息
        QRect mainwindowRec;
        m_mainwindow->setGeometry(m_mainwindowPosition);
        mainwindowRec = m_mainwindow->geometry();
        int x = m_mainwindowPosition.x();
        int width = m_mainwindowPosition.width();
        int height = m_mainwindowPosition.height();

        // 如果控件被键盘遮挡
        nowUnderWidgetYOffset = nowUnderWidgetYOffset - temp->y();
        if (nowUnderWidgetYOffset > temp->height() - m_height) {
            int yy = m_height - nowUnderWidgetYOffset + nowUnderWidgetH + m_mainwindowOffset;
            m_MainWindowAnimation.setStartValue(mainwindowRec);
            m_MainWindowAnimation.setEndValue(QRect(x, yy, width, height));
        } else {
            m_MainWindowAnimation.setStartValue(m_mainwindowPosition);
            m_MainWindowAnimation.setEndValue(m_mainwindowPosition);
        }
        m_keyAnimation.setStartValue(QRect(temp->x() + (temp->width() - m_width) / 2, temp->y() + temp->height(), m_width, m_height));
        m_keyAnimation.setEndValue(QRect(temp->x() + (temp->width() - m_width) / 2, temp->y() + temp->height() - m_height + nowUnderWidgetH, m_width, m_height));
        showAnimation();
    }
    break;
    }
}

//事件过滤器,用于识别鼠标单击汉字标签处获取对应汉字
bool QkeyTools::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress) {
        if (obj == ui->labCh0) {
            setCandidateWords(0);
        } else if (obj == ui->labCh1) {
            setCandidateWords(1);
        } else if (obj == ui->labCh2) {
            setCandidateWords(2);
        } else if (obj == ui->labCh3) {
            setCandidateWords(3);
        } else if (obj == ui->labCh4) {
            setCandidateWords(4);
        } else if (obj == ui->labCh5) {
            setCandidateWords(5);
        } else if (obj == ui->labCh6) {
            setCandidateWords(6);
        } else if (obj == ui->labCh7) {
            setCandidateWords(7);
        } else if (obj == ui->labCh8) {
            setCandidateWords(8);
        } else if (obj == ui->labCh9) {
            setCandidateWords(9);
        } else if (currentEditType != "" && obj != ui->CloseBtn) {
            this->show();
        }
        longPressBtn = static_cast<QPushButton *>(obj);
        if (checkPress()) {
            isPress = true;
            timerPress->start(500);
        }
        return false;
    } else if (event->type() == QEvent::MouseButtonRelease) {
        longPressBtn = static_cast<QPushButton *>(obj);
        if (checkPress()) {
            isPress = false;
            timerPress->stop();
        }
        return false;
    } else if (event->type() == QEvent::KeyPress) {
        //如果输入法窗体不可见,则不需要处理
        if (!isVisible()) {
            return QWidget::eventFilter(obj, event);
        }

        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        //Shift切换输入法模式,esc键关闭输入法面板,空格取第一个汉字,退格键删除
        //中文模式下回车键取拼音,中文模式下当没有拼音时可以输入空格
        if (keyEvent->key() == Qt::Key_Space) {
            if (ui->labPY->text() != "") {
                setCandidateWords(0);
                return true;
            } else {
                return false;
            }
        } else if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter) {
            insertValue(ui->labPY->text());
            ui->labPY->setText("");
            selectLanguage(m_currentLanguage);
            return true;
        } else if (keyEvent->key() == Qt::Key_Shift) {
            ui->LanguageTypeBtn->click();
            return true;
        } else if (keyEvent->key() == Qt::Key_Escape) {
            ui->CloseBtn->click();
            return true;
        } else if (keyEvent->key() == Qt::Key_Backspace) {
            ui->DeleteBtn->click();
            return true;
        } else if (keyEvent->text() == "+" || keyEvent->text() == "=") {
            if (ui->labPY->text() != "") {
                ui->PageDownBtn->click();
                return true;
            } else {
                return false;
            }
        } else if (keyEvent->text() == "-" || keyEvent->text() == "_") {
            if (ui->labPY->text() != "") {
                ui->PageUpBtn->click();
                return true;
            } else {
                return false;
            }
        } else if (keyEvent->key() == Qt::Key_CapsLock) {
            if (m_currentLanguage != Capital) {
                setCurrentLanguage(Capital);
            } else {
                setCurrentLanguage(English);
            }
            changeType(m_currentLanguage);
            return true;
        } else {
            if (currentEditType == "QWidget") {
                return false;
            }
            QString key;
            if (m_currentLanguage == Chinese) {
                key = keyEvent->text();
            } else if (m_currentLanguage == English) {
                key = keyEvent->text().toLower();
            } else if (m_currentLanguage == Capital) {
                key = keyEvent->text().toUpper();
            }
            QList<QPushButton *> btn = this->findChildren<QPushButton *>();
            foreach (QPushButton *b, btn) {
                if (b->text() == key) {
                    b->click();
                    return true;
                }
            }
        }
        return false;
    }
    return QWidget::eventFilter(obj, event);
}

bool QkeyTools::checkPress()
{
    //只有属于输入法键盘的合法按钮才继续处理
    bool num_ok = longPressBtn->property("btnNum").toBool();
    bool other_ok = longPressBtn->property("btnOther").toBool();
    bool letter_ok = longPressBtn->property("btnLetter").toBool();
    if (num_ok || other_ok || letter_ok) {
        return true;
    }
    return false;
}

void QkeyTools::reClicked()
{
    if (isPress) {
        timerPress->setInterval(30);
        longPressBtn->click();
    }
}

void QkeyTools::keyAnimationFinished()
{
    if (m_ishide) {
        this->setVisible(false);
    } else {
        this->update();
    }
}

void QkeyTools::windowAnimationFinished()
{
    if (m_position == Embedded || m_position == ProcessBottom) {
        m_mainwindow->update();
    }
}

void QkeyTools::focusChanged(QWidget *oldWidget, QWidget *nowWidget)
{
    Q_UNUSED(oldWidget)
    if (nowWidget != nullptr && !this->isAncestorOf(nowWidget)) {
        //在Qt5和linux系统中(嵌入式linux除外),当输入法面板关闭时,焦点会变成无,然后焦点会再次移到焦点控件处
        //这样导致输入法面板的关闭按钮不起作用,关闭后马上有控件获取焦点又显示.
        //为此,增加判断,当焦点是从有对象转为无对象再转为有对象时不要显示.
        //这里又要多一个判断,万一首个窗体的第一个焦点就是落在可输入的对象中,则要过滤掉
#ifndef __arm__
        if (oldWidget == nullptr && !isFirst) {
            return;
        }
#endif

        isFirst = false;

        if (nowWidget->inherits("QLineEdit")) {
            currentLineEdit = static_cast<QLineEdit *>(nowWidget);
            currentEditType = "QLineEdit";
        } else if (nowWidget->inherits("QTextEdit")) {
            currentTextEdit = static_cast<QTextEdit *>(nowWidget);
            currentEditType = "QTextEdit";
        } else if (nowWidget->inherits("QPlainTextEdit")) {
            currentPlain = static_cast<QPlainTextEdit *>(nowWidget);
            currentEditType = "QPlainTextEdit";
        } else if (nowWidget->inherits("QTextBrowser")) {
            currentBrowser = static_cast<QTextBrowser *>(nowWidget);
            currentEditType = "QTextBrowser";
        } else if (nowWidget->inherits("QComboBox")) {
            QComboBox *cbox = static_cast<QComboBox *>(nowWidget);
            //只有当下拉选择框处于编辑模式才可以输入
            if (cbox->isEditable()) {
                currentLineEdit = cbox->lineEdit() ;
                currentEditType = "QLineEdit";
            }
        } else if (nowWidget->inherits("QSpinBox") ||
                   nowWidget->inherits("QDoubleSpinBox") ||
                   nowWidget->inherits("QDateEdit") ||
                   nowWidget->inherits("QTimeEdit") ||
                   nowWidget->inherits("QDateTimeEdit")) {
            currentWidget = nowWidget;
            currentEditType = "QWidget";
        } else {
            // 非输入框控件获取到焦点
            //需要将输入法切换到最初的原始状态--小写,同时将之前的对象指针置为零
            currentWidget = nullptr;
            currentLineEdit = nullptr;
            currentTextEdit = nullptr;
            currentPlain = nullptr;
            currentBrowser = nullptr;
            currentEditType = "";
            changeType();
            resetAllWidget();
            return;
        }

        resetAllWidget();
        ShowPanel(nowWidget);
    }
}

void QkeyTools::changeType(Language type)
{
    // 设置当前输入模式
    setCurrentLanguage(type);

    switch (m_currentLanguage) {
    case English: {
        changeLetter(false);
        ui->LanguageTypeBtn->setText(tr("英文"));
        ui->btnOther12->setText("/");
        ui->btnOther14->setText(":");
        ui->btnOther17->setText(",");
        ui->btnOther18->setText("\\");
        ui->btnOther21->setText("\"");
        break;
    }

    case Capital: {
        changeLetter(true);
        ui->LanguageTypeBtn->setText(tr("大写"));
        ui->btnOther12->setText("/");
        ui->btnOther14->setText(":");
        ui->btnOther17->setText(",");
        ui->btnOther18->setText("\\");
        ui->btnOther21->setText("\"");
        break;
    }

    case Chinese: {
        changeLetter(false);
        ui->LanguageTypeBtn->setText(tr("中文"));
        ui->btnOther12->setText("。");
        ui->btnOther14->setText("：");
        ui->btnOther17->setText("，");
        ui->btnOther18->setText("；");
        ui->btnOther21->setText("“”");
        break;
    }
    default: {
        ui->LanguageTypeBtn->setText(tr("未知"));
        break;
    }
    }

    //每次切换到新模式,都要执行清空之前中文模式下的信息
    clearCandidateWords();
    ui->labPY->setText("");
}

void QkeyTools::changeLetter(bool isUpper)
{
    QList<QPushButton *> btn = this->findChildren<QPushButton *>();
    foreach (QPushButton *b, btn) {
        if (b->property("btnLetter").toBool()) {
            if (isUpper) {
                b->setText(b->text().toUpper());
            } else {
                b->setText(b->text().toLower());
            }
        }
    }
}

void QkeyTools::selectLanguage(Language language)
{
    m_currentLanguage = language;

    clearCandidateWords();
    QSqlDatabase db = QSqlDatabase::database(m_candidateWordsPath);//连接键盘中文词库数据库
    QSqlQuery query(db);
    QString currentPY = ui->labPY->text();
    QString sql = "select [word] from [pinyin] where [pinyin]='" + currentPY + "';";
    query.exec(sql);
    //逐个将查询到的字词加入汉字队列
    while (query.next()) {
        QString result = query.value(0).toString();
        QStringList text = result.split(" ");
        foreach (QString txt, text) {
            if (txt.length() > 0) {
                allPY.append(txt);
                currentPY_count++;
            }
        }
    }
    showCandidateWords();
}

void QkeyTools::showCandidateWords()
{
    //每个版面最多显示10个汉字
    int count = 0;
    currentPY.clear();
    for (int i = 0; i < 10; i++) {
        labCh[i]->setText("");
    }

    for (int i = currentPY_index; i < currentPY_count; i++) {
        if (count == 10) {
            break;
        }
        QString txt = QString("%1.%2").arg(count).arg(allPY[currentPY_index]);
        currentPY.append(allPY[currentPY_index]);
        labCh[count]->setText(txt);
        count++;
        currentPY_index++;
    }
    //qDebug() << "currentPY_index:" << currentPY_index << "currentPY_count:" << currentPY_count;
}

void QkeyTools::handleButtonClicked()
{
    //如果当前焦点控件类型为空,则返回不需要继续处理
    if (currentEditType == "") {
        return;
    }

    QPushButton *btn = static_cast<QPushButton *>(sender());
    QString objectName = btn->objectName();
    if (objectName == "LanguageTypeBtn") {
        switch (m_currentLanguage) {
        case English: setCurrentLanguage(Capital); break;
        case Capital: setCurrentLanguage(Chinese); break;
        case Chinese: setCurrentLanguage(English); break;
        default: setCurrentLanguage(English); break;
        }
        changeType(m_currentLanguage);
    } else if (objectName == "DeleteBtn") {
        switch (m_currentLanguage) {
        case Chinese: { //如果当前是中文模式,则删除对应拼音,删除完拼音之后再删除对应文本输入框的内容
            QString txt = ui->labPY->text();
            int len = txt.length();
            if (len > 0) {
                ui->labPY->setText(txt.left(len - 1));
                selectLanguage(Chinese);
            } else {
                deleteValue();
            }
            break;
        }
        default: {
            deleteValue();
            break;
        }
        }
    } else if (objectName == "PageUpBtn") {
        if (currentPY_index >= 20) {
            //每次最多显示10个候选字,所以每次向前的时候索引要减20
            if (currentPY_index % 10 == 0) {
                currentPY_index -= 20;
            } else {
                currentPY_index = currentPY_count - (currentPY_count % 10) - 10;
            }
        } else {
            currentPY_index = 0;
        }
        showCandidateWords();
    } else if (objectName == "PageDownBtn") {
        if (currentPY_index < currentPY_count - 1) {
            showCandidateWords();
        }
    } else if (objectName == "CloseBtn") { // 点击关闭按钮
        switch (m_position) {
        case Embedded:
        case ProcessBottom: {
            hideAnimation();
            m_mainwindow->setFocus();
            break;
        }
        default: this->close(); break;
        }
    } else if (objectName == "SpaceBtn") {
        insertValue(" ");
    } else {
        QString value = btn->text();
        //如果是&按钮，因为对应&被过滤,所以真实的text为去除前面一个&字符
        if (objectName == "btnOther7") {
            value = value.right(1);
        }
        switch (m_currentLanguage) {
        case English:
        case Capital: {
            insertValue(value);
            break;
        }
        case Chinese:
        case Chinese_HK:
        case Chinese_TW: {//中文模式下,不允许输入特殊字符,单击对应数字按键取得当前索引的汉字
            if (btn->property("btnOther").toBool()) {
                if (ui->labPY->text().length() == 0) {
                    insertValue(value);
                }
            } else if (btn->property("btnNum").toBool()) {
                if (ui->labPY->text().length() == 0) {
                    insertValue(value);
                } else if (objectName.startsWith("btn")) {
                    int index = objectName.split("btn").last().toInt();
                    setCandidateWords(index);
                }
            } else if (btn->property("btnLetter").toBool()) {
                ui->labPY->setText(ui->labPY->text() + value);
                selectLanguage(Chinese);
            }
            break;
        }
        }
    }
}

void QkeyTools::insertValue(QString value)
{
    if (currentEditType == "QLineEdit") {
        currentLineEdit->insert(value);
    } else if (currentEditType == "QTextEdit") {
        currentTextEdit->insertPlainText(value);
    } else if (currentEditType == "QPlainTextEdit") {
        currentPlain->insertPlainText(value);
    } else if (currentEditType == "QTextBrowser") {
        currentBrowser->insertPlainText(value);
    } else if (currentEditType == "QWidget") {
        QKeyEvent keyPress(QEvent::KeyPress, 0, Qt::NoModifier, QString(value));
        QApplication::sendEvent(currentWidget, &keyPress);
    }
}

void QkeyTools::deleteValue()
{
    if (currentEditType == "QLineEdit") {
        currentLineEdit->backspace();
    } else if (currentEditType == "QTextEdit") {
        //获取当前QTextEdit光标,如果光标有选中,则移除选中字符,否则删除光标前一个字符
        QTextCursor cursor = currentTextEdit->textCursor();
        if (cursor.hasSelection()) {
            cursor.removeSelectedText();
        } else {
            cursor.deletePreviousChar();
        }
    } else if (currentEditType == "QPlainTextEdit") {
        //获取当前QTextEdit光标,如果光标有选中,则移除选中字符,否则删除光标前一个字符
        QTextCursor cursor = currentPlain->textCursor();
        if (cursor.hasSelection()) {
            cursor.removeSelectedText();
        } else {
            cursor.deletePreviousChar();
        }
    } else if (currentEditType == "QTextBrowser") {
        //获取当前QTextEdit光标,如果光标有选中,则移除选中字符,否则删除光标前一个字符
        QTextCursor cursor = currentBrowser->textCursor();
        if (cursor.hasSelection()) {
            cursor.removeSelectedText();
        } else {
            cursor.deletePreviousChar();
        }
    } else if (currentEditType == "QWidget") {
        QKeyEvent keyPress(QEvent::KeyPress, Qt::Key_Delete, Qt::NoModifier, QString());
        QApplication::sendEvent(currentWidget, &keyPress);
    }
}

void QkeyTools::setCandidateWords(int index)
{
    if (index < 0)
        return;
    int count = currentPY.count();
    if (count > index) {
        insertValue(currentPY[index]);
        //添加完一个汉字后,清空当前汉字信息,等待重新输入
        clearCandidateWords();
        ui->labPY->setText("");
    }
}

void QkeyTools::clearCandidateWords()
{
    //清空汉字,重置索引
    for (int i = 0; i < 10; i++) {
        labCh[i]->setText("");
    }
    allPY.clear();
    currentPY.clear();
    currentPY_index = 0;
    currentPY_count = 0;
}

void QkeyTools::initAnimation()
{
    // [0] 设置键盘动画属性
    m_keyAnimation.setTargetObject(this);
    m_keyAnimation.setPropertyName("geometry");
    m_keyAnimation.setDuration(AnimationTime);

    // [1] 设置主窗口动画属性
    if (Embedded == m_position || m_position == ProcessBottom) {
        m_MainWindowAnimation.setTargetObject(m_mainwindow);
        m_MainWindowAnimation.setPropertyName("geometry");
        m_MainWindowAnimation.setDuration(AnimationTime);
    }

    // [2] 连接信号槽
    connect(&m_keyAnimation, SIGNAL(finished()), this, SLOT(keyAnimationFinished()));
    connect(&m_MainWindowAnimation, SIGNAL(finished()), this, SLOT(windowAnimationFinished()));
}

void QkeyTools::showAnimation()
{
    m_ishide = false;
    if (m_position == Embedded || m_position == ProcessBottom) {
        m_MainWindowAnimation.setDirection(QAbstractAnimation::Forward);
        m_keyAnimation.setDirection(QAbstractAnimation::Forward);
        m_MainWindowAnimation.start();
        m_keyAnimation.start();
    } else {
        m_keyAnimation.setDirection(QAbstractAnimation::Forward);
        m_keyAnimation.start();
    }
}

void QkeyTools::hideAnimation()
{
    m_ishide = true;
    if (m_position == Embedded || m_position == ProcessBottom) {
        m_MainWindowAnimation.setDirection(QAbstractAnimation::Backward);
        m_keyAnimation.setDirection(QAbstractAnimation::Backward);
        m_MainWindowAnimation.start();
        m_keyAnimation.start();
    } else {
        m_keyAnimation.setDirection(QAbstractAnimation::Forward);
        m_keyAnimation.start();
    }
}

void QkeyTools::resetAllWidget()
{
    // [0] 显示主程序默认位置
    if (Embedded == m_position) {
        m_mainwindow->setGeometry(m_mainwindowPosition);
    }

    // [1] 隐藏键盘
    this->close();

    // [2] 设置主窗口的默认动画位置
    m_MainWindowAnimation.setStartValue(m_mainwindowPosition);
    m_MainWindowAnimation.setEndValue(m_mainwindowPosition);

    // [3] 设置样式表
    if (!m_isSetStyle) {
        ChangeStyle();
        m_isSetStyle = true;
    }
}

void QkeyTools::ChangeStyle()
{
    switch (m_style) {
    case BLUE: {
        changeStyle("#DEF0FE", "#C0DEF6", "#DEF0FE", "#F8F8FF", "#C0DCF2", "#386487");
        break;
    }

    case DEV: {
        changeStyle("#C0D3EB", "#BCCFE7", "#C0D3EB", "#F0F8FF", "#B4C2D7", "#324C6C");
        break;
    }

    case GRAY: {
        changeStyle("#E4E4E4", "#A2A2A2", "#E4E4E4", "#4D4D4D", "#A9A9A9", "#000000");
        break;
    }

    case LIGHTGRAY: {
        changeStyle("#EEEEEE", "#E5E5E5", "#EEEEEE", "#B0C4DE", "#D4D0C8", "#000000");
        break;
    }

    case DARKGRAY: {
        changeStyle("#A9A9A9", "#696969", "#A9A9A9", "#DCDCDC", "#A9ACB5", "#FFFFFF");
        break;
    }

    case BLACK: {
        changeStyle("#4D4D4D", "#292929", "#4D4D4D", "#DCDCDC", "#D9D9D9", "#CACAD0");
        break;
    }

    case BROWN: {
        changeStyle("#F4A460", "#D2691E", "#F4A460", "#8B4513", "#D2691E", "#FFFFFF");
        break;
    }

    case LIGHTYELLOW: {
        changeStyle("#FFFFE0", "#FFFACD", "#FFFFE0", "#F0E68C", "#F0E68C", "#000000");
        break;
    }
    }
}

void QkeyTools::ChangeFont()
{
    QFont btnFont(this->font().family(), m_btnFontSize);
    QFont labFont(this->font().family(), m_labFontSize);

    QList<QPushButton *> btns = ui->widgetMain->findChildren<QPushButton *>();
    foreach (QPushButton *btn, btns) {
        btn->setFont(btnFont);
    }

    QList<QLabel *> labs = ui->widgetTop->findChildren<QLabel *>();
    foreach (QLabel *lab, labs) {
        lab->setFont(labFont);
    }
    ui->PageUpBtn->setFont(labFont);
    ui->PageDownBtn->setFont(labFont);
    ui->CloseBtn->setFont(labFont);
}

void QkeyTools::changeStyle(QString widgetTopColor, QString widgetBottomColor, QString btnTopColor, QString btnBottomColor, QString btnBorderColor, QString textColor)
{
    QStringList qss;
    qss.append(QString("QWidget#QkeyTools{background:qlineargradient(spread:pad,x1:0,y1:0,x2:0,y2:1,stop:0 %1,stop:1 %2);}")
               .arg(widgetTopColor).arg(widgetBottomColor));
    qss.append("QPushButton{padding:5px;border-radius:3px;}");

    qss.append(QString("QPushButton:pressed{background:qlineargradient(spread:pad,x1:0,y1:0,x2:0,y2:1,stop:0 %1,stop:1 %2);}")
               .arg(btnTopColor).arg(btnBottomColor));

    qss.append(QString("QLabel,QPushButton{color:%1;}").arg(textColor));

    qss.append(QString("QPushButton#PageUpBtn,QPushButton#PageDownBtn,"
                       "QPushButton#CloseBtn{padding:5px;}"));

    qss.append(QString("QPushButton{border:1px solid %1;}")
               .arg(btnBorderColor));

    qss.append(QString("QLineEdit{border:1px solid %1;border-radius:5px;"
                       "padding:2px;background:none;selection-background-color:%2;"
                       "selection-color:%3;}")
               .arg(btnBorderColor).arg(widgetBottomColor).arg(widgetTopColor));
    this->setStyleSheet(qss.join(""));
}
