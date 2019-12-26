#include "qkeytools.h"
#include "ui_qkeytools.h"
#include <QCoreApplication>

QkeyTools *QkeyTools::_instance = 0;
QkeyTools::QkeyTools(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::QkeyTools)
{
    ui->setupUi(this);

    // [0] 需要将对应的中文字库拷贝到程序运行的相对路径下
    m_chineseWordLibPath = QCoreApplication::applicationDirPath() + "/chinesewordlib.db";

    m_inputMode = MIN;
    m_btnFontSize = 10;
    m_labFontSize = 10;
    m_width = 800;
    m_height = 250;
    m_mainwindowOffset = 10;
    m_position = CONTROL;
    m_style = GRAY;

    this->InitProperty();
    this->InitForm();
    this->ChangeStyle();
}

QkeyTools::~QkeyTools()
{
    delete ui;
}
#if (QT_VERSION >= QT_VERSION_CHECK(5,7,0))
QObject *QkeyTools::QkeyTools_singletontype_provider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)
    return getInstance();
}
#endif
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

void QkeyTools::setPosition(qint8 _position)
{
    if(_position!=m_position){
        m_position = static_cast<Position>(_position);
        emit positionChanged(m_position);
    }
}

QkeyTools::Position QkeyTools::position()
{
    return m_position;
}

void QkeyTools::setStyle(qint8 _style)
{
    if(_style!=m_style){
        m_style = static_cast<Style>(_style);
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
    if(m_btnFontSize!=size){
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
    if(size!=m_labFontSize){
        m_labFontSize = size;
        emit labSizeChanged(m_labFontSize);
        this->ChangeFont();
    }
}

qint8 QkeyTools::labSize()
{
    return m_labFontSize;
}

void QkeyTools::setWidth(quint16 _width)
{
    if(_width!=m_width){
        m_width = _width;
        emit widthChanged(m_width);
    }
}

quint16 QkeyTools::width()
{
    return m_width;
}

void QkeyTools::setHeight(quint16 _height)
{
    if(_height!=m_height){
        m_height = _height;
        emit heightChanged(m_height);
    }
}

quint16 QkeyTools::height()
{
    return m_height;
}

void QkeyTools::setInputMode(qint8 _mode)
{
    if(_mode!=m_inputMode){
        m_inputMode = static_cast<InputMode>( _mode );
        emit inputModeChanged(m_inputMode);
    }
}

QkeyTools::InputMode QkeyTools::inputMode()
{
    return m_inputMode;
}

void QkeyTools::setChineseWordLibPath(QString path)
{
    if(path!=m_chineseWordLibPath){
        m_chineseWordLibPath = path;
        QSqlDatabase DbConn;
        DbConn = QSqlDatabase::addDatabase("QSQLITE","QkeyTools");
        DbConn.setDatabaseName(m_chineseWordLibPath);
        DbConn.open();
    }
}

QString QkeyTools::chineseWordLibPath()
{
    return m_chineseWordLibPath;
}

void QkeyTools::setMainwindowOffset(quint16 offset)
{
    if(m_mainwindowOffset!=offset){
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
    if(m_position!=Embedded){
        if (mousePressed && (e->buttons() == Qt::LeftButton)) {
            this->move(e->globalPos() - mousePoint);
            e->accept();
        }
    }
}

void QkeyTools::mousePressEvent(QMouseEvent *e)
{
    if(m_position!=Embedded){
        if (e->button() == Qt::LeftButton) {
            mousePressed = true;
            mousePoint = e->globalPos() - this->pos();
            e->accept();
        }
    }
}

void QkeyTools::mouseReleaseEvent(QMouseEvent *)
{
    if(m_position!=Embedded){
        mousePressed = false;
    }
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
    deskWidth= screens->availableSize().width();
    deskHeight = screens->availableSize().height();
#endif
    QSqlDatabase DbConn;
    DbConn = QSqlDatabase::addDatabase("QSQLITE","QkeyTools");
    DbConn.setDatabaseName(m_chineseWordLibPath);
    DbConn.open();

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
    foreach (QPushButton * b, btn) {
        connect(b, SIGNAL(clicked()), this, SLOT(btn_clicked()));
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
    ui->btnSpace->setProperty("btnOther", true);
    ui->btnDelete->setProperty("btnOther", true);

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

void QkeyTools::ShowPanel()
{
    this->raise();
    this->setVisible(true);
    int width = ui->btn0->width();
    width = width > 40 ? width : 40;
    ui->btnPre->setMinimumWidth(width);
    ui->btnPre->setMaximumWidth(width);
    ui->btnNext->setMinimumWidth(width);
    ui->btnNext->setMaximumWidth(width);
    ui->btnClose->setMinimumWidth(width);
    ui->btnClose->setMaximumWidth(width);
}

//事件过滤器,用于识别鼠标单击汉字标签处获取对应汉字
bool QkeyTools::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress) {
        if (obj == ui->labCh0) {
            setChinese(0);
        } else if (obj == ui->labCh1) {
            setChinese(1);
        } else if (obj == ui->labCh2) {
            setChinese(2);
        } else if (obj == ui->labCh3) {
            setChinese(3);
        } else if (obj == ui->labCh4) {
            setChinese(4);
        } else if (obj == ui->labCh5) {
            setChinese(5);
        } else if (obj == ui->labCh6) {
            setChinese(6);
        } else if (obj == ui->labCh7) {
            setChinese(7);
        } else if (obj == ui->labCh8) {
            setChinese(8);
        } else if (obj == ui->labCh9) {
            setChinese(9);
        } else if (currentEditType != "" && obj != ui->btnClose) {
            ShowPanel();
        }
        btnPress = static_cast<QPushButton *>(obj);
        if (checkPress()) {
            isPress = true;
            timerPress->start(500);
        }
        return false;
    } else if (event->type() == QEvent::MouseButtonRelease) {
        btnPress = static_cast<QPushButton *>(obj);
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
                setChinese(0);
                return true;
            } else {
                return false;
            }
        } else if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter) {
            insertValue(ui->labPY->text());
            ui->labPY->setText("");
            selectChinese();
            return true;
        } else if (keyEvent->key() == Qt::Key_Shift) {
            ui->btnType->click();
            return true;
        } else if (keyEvent->key() == Qt::Key_Escape) {
            ui->btnClose->click();
            return true;
        } else if (keyEvent->key() == Qt::Key_Backspace) {
            ui->btnDelete->click();
            return true;
        } else if (keyEvent->text() == "+" || keyEvent->text() == "=") {
            if (ui->labPY->text() != "") {
                ui->btnNext->click();
                return true;
            } else {
                return false;
            }
        } else if (keyEvent->text() == "-" || keyEvent->text() == "_") {
            if (ui->labPY->text() != "") {
                ui->btnPre->click();
                return true;
            } else {
                return false;
            }
        } else if (keyEvent->key() == Qt::Key_CapsLock) {
            if (m_inputMode != MAX) {
                setInputMode(MAX);
            } else {
                setInputMode(MIN);
            }
            changeType(m_inputMode);
            return true;
        } else {
            if (currentEditType == "QWidget") {
                return false;
            }
            QString key;
            if (m_inputMode == CHINESE) {
                key = keyEvent->text();
            } else if (m_inputMode == MIN) {
                key = keyEvent->text().toLower();
            } else if (m_inputMode == MAX) {
                key = keyEvent->text().toUpper();
            }
            QList<QPushButton *> btn = this->findChildren<QPushButton *>();
            foreach (QPushButton * b, btn) {
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
    bool num_ok = btnPress->property("btnNum").toBool();
    bool other_ok = btnPress->property("btnOther").toBool();
    bool letter_ok = btnPress->property("btnLetter").toBool();
    if (num_ok || other_ok || letter_ok) {
        return true;
    }
    return false;
}

void QkeyTools::reClicked()
{
    if (isPress) {
        timerPress->setInterval(30);
        btnPress->click();
    }
}

void QkeyTools::keyAnimationFinished()
{
    if(m_ishide){
        this->setVisible(false);
    }else {
        this->update();
    }
}

void QkeyTools::windowAnimationFinished()
{
    if(m_position==Embedded || m_position == ProcessBottom){
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
            ShowPanel();
        }/* else if (nowWidget->inherits("QTextEdit")) {
            currentTextEdit = static_cast<QTextEdit *>(nowWidget);
            currentEditType = "QTextEdit";
            ShowPanel();
        } else if (nowWidget->inherits("QPlainTextEdit")) {
            currentPlain = static_cast<QPlainTextEdit *>(nowWidget);
            currentEditType = "QPlainTextEdit";
            ShowPanel();
        } else if (nowWidget->inherits("QTextBrowser")) {
            currentBrowser = static_cast<QTextBrowser *>(nowWidget);
            currentEditType = "QTextBrowser";
            ShowPanel();
        } else if (nowWidget->inherits("QComboBox")) {
            QComboBox *cbox = static_cast<QComboBox *>(nowWidget);
            //只有当下拉选择框处于编辑模式才可以输入
            if (cbox->isEditable()) {
                currentLineEdit = cbox->lineEdit() ;
                currentEditType = "QLineEdit";
                ShowPanel();
            }
        } else if (nowWidget->inherits("QSpinBox") ||
                   nowWidget->inherits("QDoubleSpinBox") ||
                   nowWidget->inherits("QDateEdit") ||
                   nowWidget->inherits("QTimeEdit") ||
                   nowWidget->inherits("QDateTimeEdit")) {
            currentWidget = nowWidget;
            currentEditType = "QWidget";
            ShowPanel();
        }*/ else {// 非输入框控件获取到焦点
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

        // 设置键盘显示位置
        switch (m_position) {
        case CONTROL:{ // 控件下方,左右相对程序对齐
            QRect rect = nowWidget->rect();
            QPoint pos = QPoint(rect.left(), rect.bottom() + 2);
            pos = nowWidget->mapToGlobal(pos);
            QPoint pos1 = QPoint((deskWidth - m_width) / 2, deskHeight / 2 - m_height / 2);
            this->setGeometry(pos1.x(), pos.y(), m_width, m_height);
            break;
        }
        case CENTER:{ // 程序中间
            QPoint pos = QPoint((deskWidth - m_width) / 2, deskHeight / 2 - m_height / 2);
            this->setGeometry(pos.x(), pos.y(), m_width, m_height);
            break;
        }
        case BOTTOM:{ // 屏幕底部
            m_keyAnimation.setStartValue(QRect((deskWidth - m_width)/2,deskHeight,m_width, m_height));
            m_keyAnimation.setEndValue(QRect((deskWidth - m_width)/2, deskHeight - m_height, m_width, m_height));
            showAnimation();
            break;
        }
        case Embedded:{ //覆盖在主程序的底部
            if(m_mainwindow==nullptr){
                // 如果没有设置主程序对象
                m_keyAnimation.setStartValue(QRect((deskWidth - m_width)/2,deskHeight,m_width, m_height));
                m_keyAnimation.setEndValue(QRect((deskWidth - m_width)/2, deskHeight - m_height, m_width, m_height));
                qDebug()<<__FILE__<<__LINE__<<"main window is nullptr on platform: Embedded";
                showAnimation();
            }else{
                // 用于保存主窗口对象的位置大小信息
                QRect mainwindowRec;
                mainwindowRec = m_mainwindow->geometry();
                int x = mainwindowRec.x();
                int width = mainwindowRec.width();
                int height = mainwindowRec.height();

                //  获取当前选中控件的位置信息
                int nowControlH = nowWidget->geometry().height();

                //  采用循环计算当前控件相对于主程序的位置
                QWidget *temp = nowWidget;
                int  nowControlYOffset = nowWidget->geometry().y() + m_mainwindowOffset;
                while (temp->parent()) {
                    nowControlYOffset += static_cast<QWidget*>(temp->parent())->geometry().y();
                    temp = static_cast<QWidget*>(temp->parent());
                }

                // 如果控件被键盘遮挡
                if(nowControlH + nowControlYOffset > deskHeight - m_height){
                    m_MainWindowAnimation.setStartValue(mainwindowRec);
                    m_MainWindowAnimation.setEndValue(QRect(x,m_height - nowControlH - nowControlYOffset,width,height));
                }else{
                    m_MainWindowAnimation.setStartValue(mainwindowRec);
                    m_MainWindowAnimation.setEndValue(mainwindowRec);
                }

                //                m_keyAnimation.setStartValue(QRect(x +(width - m_width)/2, y + height, m_width, m_height));
                //                m_keyAnimation.setEndValue(QRect(x +(width - m_width)/2, y + height - m_height, m_width, m_height));
                m_keyAnimation.setStartValue(QRect((deskWidth - m_width)/2,deskHeight,m_width, m_height));
                m_keyAnimation.setEndValue(QRect((deskWidth - m_width)/2, deskHeight - m_height, m_width, m_height));
                showAnimation();
            }
            break;
        }
        case ProcessBottom:{
            if(m_mainwindow==nullptr){
                // 如果没有设置主程序对象
                m_keyAnimation.setStartValue(QRect((deskWidth - m_width)/2,deskHeight,m_width, m_height));
                m_keyAnimation.setEndValue(QRect((deskWidth - m_width)/2, deskHeight - m_height, m_width, m_height));
                qDebug()<<__FILE__<<__LINE__<<"main window is nullptr on platform: Embedded";
                showAnimation();
            }

            //  获取当前选中控件的位置信息
            int nowControlH = nowWidget->geometry().height();

            //  采用循环计算当前控件相对于主程序的位置
            QWidget *temp = nowWidget;
            int  nowControlYOffset = nowWidget->geometry().y();
            while (temp->parent()) {
                nowControlYOffset += static_cast<QWidget*>(temp->parent())->geometry().y();
                temp = static_cast<QWidget*>(temp->parent());
            }
            // 用于保存主窗口对象的位置大小信息
            QRect mainwindowRec;
            m_mainwindow->setGeometry(m_mainwindowPosition);
            mainwindowRec = m_mainwindow->geometry();
            int x = m_mainwindowPosition.x();
            int width = m_mainwindowPosition.width();
            int height = m_mainwindowPosition.height();

            // 如果控件被键盘遮挡
            nowControlYOffset = nowControlYOffset - temp->y();
            if(nowControlYOffset > temp->height() - m_height){
                int yy = m_height - nowControlYOffset + nowControlH + m_mainwindowOffset;
//                qDebug()<<"yy:"<< yy ;
                m_MainWindowAnimation.setStartValue(mainwindowRec);
                m_MainWindowAnimation.setEndValue(QRect(x,
                yy,width,height));
            }else{
//                qDebug()<<"aaaa" ;
                m_MainWindowAnimation.setStartValue(m_mainwindowPosition);
                m_MainWindowAnimation.setEndValue(m_mainwindowPosition);
            }
            m_keyAnimation.setStartValue(QRect(temp->x() + (temp->width() - m_width)/2,temp->y() + temp->height(),m_width, m_height));
            m_keyAnimation.setEndValue(QRect(temp->x() + (temp->width() - m_width)/2,temp->y() + temp->height() - m_height + nowControlH, m_width, m_height));
            showAnimation();
        }
            break;
        }
    }
}

void QkeyTools::changeType(InputMode type)
{
    // 设置当前输入模式
    setInputMode(type);

    switch (m_inputMode) {
    case MIN:{
        changeLetter(false);
        ui->btnType->setText("小写");
        ui->btnOther12->setText("/");
        ui->btnOther14->setText(":");
        ui->btnOther17->setText(",");
        ui->btnOther18->setText("\\");
        ui->btnOther21->setText("\"");
        break;
    }

    case MAX:{
        changeLetter(true);
        ui->btnType->setText("大写");
        ui->btnOther12->setText("/");
        ui->btnOther14->setText(":");
        ui->btnOther17->setText(",");
        ui->btnOther18->setText("\\");
        ui->btnOther21->setText("\"");
        break;
    }

    case CHINESE:{
        changeLetter(false);
        ui->btnType->setText("中文");
        ui->btnOther12->setText("。");
        ui->btnOther14->setText("：");
        ui->btnOther17->setText("，");
        ui->btnOther18->setText("；");
        ui->btnOther21->setText("“");
        break;
    }
    }

    //每次切换到新模式,都要执行清空之前中文模式下的信息
    clearChinese();
    ui->labPY->setText("");
}

void QkeyTools::changeLetter(bool isUpper)
{
    QList<QPushButton *> btn = this->findChildren<QPushButton *>();
    foreach (QPushButton * b, btn) {
        if (b->property("btnLetter").toBool()) {
            if (isUpper) {
                b->setText(b->text().toUpper());
            } else {
                b->setText(b->text().toLower());
            }
        }
    }
}

void QkeyTools::selectChinese()
{
    clearChinese();
    QSqlDatabase db2 = QSqlDatabase::database("QkeyTools");//连接键盘中文词库数据库
    QSqlQuery query(db2);
    QString currentPY = ui->labPY->text();
    QString sql = "select [word] from [pinyin] where [pinyin]='" + currentPY + "';";
    query.exec(sql);
    //逐个将查询到的字词加入汉字队列
    while(query.next()) {
        QString result = query.value(0).toString();
        QStringList text = result.split(" ");
        foreach (QString txt, text) {
            if (txt.length() > 0) {
                allPY.append(txt);
                currentPY_count++;
            }
        }
    }
    showChinese();
}

void QkeyTools::showChinese()
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

void QkeyTools::btn_clicked()
{
    //如果当前焦点控件类型为空,则返回不需要继续处理
    if (currentEditType == "") {
        return;
    }

    QPushButton *btn = static_cast<QPushButton *>(sender());
    QString objectName = btn->objectName();
    if (objectName == "btnType") {
        if (m_inputMode == MIN) {
            setInputMode(MAX);
        } else if (m_inputMode == MAX) {
            setInputMode(CHINESE);
        } else if (m_inputMode == CHINESE) {
            setInputMode(MIN);
        }
        changeType(m_inputMode);
    } else if (objectName == "btnDelete") {
        //如果当前是中文模式,则删除对应拼音,删除完拼音之后再删除对应文本输入框的内容
        if (m_inputMode == CHINESE) {
            QString txt = ui->labPY->text();
            int len = txt.length();
            if (len > 0) {
                ui->labPY->setText(txt.left(len - 1));
                selectChinese();
            } else {
                deleteValue();
            }
        } else {
            deleteValue();
        }
    } else if (objectName == "btnPre") {
        if (currentPY_index >= 20) {
            //每次最多显示10个汉字,所以每次向前的时候索引要减20
            if (currentPY_index % 10 == 0) {
                currentPY_index -= 20;
            } else {
                currentPY_index = currentPY_count - (currentPY_count % 10) - 10;
            }
        } else {
            currentPY_index = 0;
        }
        showChinese();
    } else if (objectName == "btnNext") {
        if (currentPY_index < currentPY_count - 1) {
            showChinese();
        }
    } else if (objectName == "btnClose") { // 点击关闭按钮
        if(m_position==Embedded||m_position==ProcessBottom){
            hideAnimation();
            //            // 设置模态框运行
            //            this->setWindowModality(Qt::NonModal);

            // 设置焦点到主程序上，这样才能第二次点击的时候调用键盘
            m_mainwindow->setFocus();
        }else {
            //            this->setVisible(false);
            this->close();
        }
    } else if (objectName == "btnSpace") {
        insertValue(" ");
    } else {
        QString value = btn->text();
        //如果是&按钮，因为对应&被过滤,所以真实的text为去除前面一个&字符
        if (objectName == "btnOther7") {
            value = value.right(1);
        }
        //当前不是中文模式,则单击按钮对应text为传递参数
        if (m_inputMode != CHINESE) {
            insertValue(value);
        } else {
            //中文模式下,不允许输入特殊字符,单击对应数字按键取得当前索引的汉字
            if (btn->property("btnOther").toBool()) {
                if (ui->labPY->text().length() == 0) {
                    insertValue(value);
                }
            } else if (btn->property("btnNum").toBool()) {
                if (ui->labPY->text().length() == 0) {
                    insertValue(value);
                } else if (objectName == "btn0") {
                    setChinese(0);
                } else if (objectName == "btn1") {
                    setChinese(1);
                } else if (objectName == "btn2") {
                    setChinese(2);
                } else if (objectName == "btn3") {
                    setChinese(3);
                } else if (objectName == "btn4") {
                    setChinese(4);
                } else if (objectName == "btn5") {
                    setChinese(5);
                } else if (objectName == "btn6") {
                    setChinese(6);
                } else if (objectName == "btn7") {
                    setChinese(7);
                } else if (objectName == "btn8") {
                    setChinese(8);
                } else if (objectName == "btn9") {
                    setChinese(9);
                }
            } else if (btn->property("btnLetter").toBool()) {
                ui->labPY->setText(ui->labPY->text() + value);
                selectChinese();
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
        if(cursor.hasSelection()) {
            cursor.removeSelectedText();
        } else {
            cursor.deletePreviousChar();
        }
    } else if (currentEditType == "QPlainTextEdit") {
        //获取当前QTextEdit光标,如果光标有选中,则移除选中字符,否则删除光标前一个字符
        QTextCursor cursor = currentPlain->textCursor();
        if(cursor.hasSelection()) {
            cursor.removeSelectedText();
        } else {
            cursor.deletePreviousChar();
        }
    } else if (currentEditType == "QTextBrowser") {
        //获取当前QTextEdit光标,如果光标有选中,则移除选中字符,否则删除光标前一个字符
        QTextCursor cursor = currentBrowser->textCursor();
        if(cursor.hasSelection()) {
            cursor.removeSelectedText();
        } else {
            cursor.deletePreviousChar();
        }
    } else if (currentEditType == "QWidget") {
        QKeyEvent keyPress(QEvent::KeyPress, Qt::Key_Delete, Qt::NoModifier, QString());
        QApplication::sendEvent(currentWidget, &keyPress);
    }
}

void QkeyTools::setChinese(int index)
{
    int count = currentPY.count();
    if (count > index) {
        insertValue(currentPY[index]);
        //添加完一个汉字后,清空当前汉字信息,等待重新输入
        clearChinese();
        ui->labPY->setText("");
    }
}

void QkeyTools::clearChinese()
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
    if(Embedded==m_position || m_position == ProcessBottom){
        m_MainWindowAnimation.setTargetObject(m_mainwindow);
        m_MainWindowAnimation.setPropertyName("geometry");
        m_MainWindowAnimation.setDuration(AnimationTime);
    }

    // [2] 连接信号槽
    connect(&m_keyAnimation,SIGNAL(finished()),this,SLOT(keyAnimationFinished()));
    connect(&m_MainWindowAnimation,SIGNAL(finished()),this,SLOT(windowAnimationFinished()));
}

void QkeyTools::showAnimation()
{
    m_ishide = false;
    if(m_position==Embedded || m_position == ProcessBottom){
        m_MainWindowAnimation.setDirection(QAbstractAnimation::Forward);
        m_keyAnimation.setDirection(QAbstractAnimation::Forward);
        m_MainWindowAnimation.start();
        m_keyAnimation.start();
    }else{
        m_keyAnimation.setDirection(QAbstractAnimation::Forward);
        m_keyAnimation.start();
    }
}

void QkeyTools::hideAnimation()
{
    m_ishide = true;
    if(m_position==Embedded || m_position == ProcessBottom){
        m_MainWindowAnimation.setDirection(QAbstractAnimation::Backward);
        m_keyAnimation.setDirection(QAbstractAnimation::Backward);
        m_MainWindowAnimation.start();
        m_keyAnimation.start();
    }else{
        m_keyAnimation.setDirection(QAbstractAnimation::Forward);
        m_keyAnimation.start();
    }
}

void QkeyTools::resetAllWidget()
{
    // [0] 显示主程序默认位置
    if(Embedded==m_position){
        m_mainwindow->setGeometry(m_mainwindowPosition);
    }

    // [1] 隐藏键盘
    //    this->setVisible(false);
    this->close();

    // [2] 设置主窗口的默认动画位置
    m_MainWindowAnimation.setStartValue(m_mainwindowPosition);
    m_MainWindowAnimation.setEndValue(m_mainwindowPosition);

    // [3] 设置样式表
    if(!m_isSetStyle){
        ChangeStyle();
        m_isSetStyle = true;
    }
}

void QkeyTools::ChangeStyle()
{
    switch (m_style) {
    case BLUE:{
        changeStyle("#DEF0FE", "#C0DEF6","#DEF0FE", "#F8F8FF", "#C0DCF2", "#386487");
        break;
    }

    case DEV:{
        changeStyle("#C0D3EB", "#BCCFE7", "#C0D3EB", "#F0F8FF","#B4C2D7", "#324C6C");
        break;
    }

    case GRAY:{
        changeStyle("#E4E4E4", "#A2A2A2","#E4E4E4", "#4D4D4D", "#A9A9A9", "#000000");
        break;
    }

    case LIGHTGRAY:{
        changeStyle("#EEEEEE", "#E5E5E5", "#EEEEEE", "#B0C4DE","#D4D0C8", "#000000");
        break;
    }

    case DARKGRAY:{
        changeStyle("#A9A9A9", "#696969", "#A9A9A9", "#DCDCDC","#A9ACB5", "#FFFFFF");
        break;
    }

    case BLACK:{
        changeStyle("#4D4D4D", "#292929","#4D4D4D", "#DCDCDC", "#D9D9D9", "#CACAD0");
        break;
    }

    case BROWN:{
        changeStyle("#F4A460", "#D2691E","#F4A460", "#8B4513", "#D2691E", "#FFFFFF");
        break;
    }

    case LIGHTYELLOW:{
        changeStyle("#FFFFE0", "#FFFACD","#FFFFE0", "#F0E68C", "#F0E68C", "#000000");
        break;
    }
    }
}

void QkeyTools::ChangeFont()
{
    QFont btnFont(this->font().family(), m_btnFontSize);
    QFont labFont(this->font().family(), m_labFontSize);

    QList<QPushButton *> btns = ui->widgetMain->findChildren<QPushButton *>();
    foreach (QPushButton * btn, btns) {
        btn->setFont(btnFont);
    }

    QList<QLabel *> labs = ui->widgetTop->findChildren<QLabel *>();
    foreach (QLabel * lab, labs) {
        lab->setFont(labFont);
    }
    ui->btnPre->setFont(labFont);
    ui->btnNext->setFont(labFont);
    ui->btnClose->setFont(labFont);
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

    qss.append(QString("QPushButton#btnPre,QPushButton#btnNext,"
                       "QPushButton#btnClose{padding:5px;}"));

    qss.append(QString("QPushButton{border:1px solid %1;}")
               .arg(btnBorderColor));

    qss.append(QString("QLineEdit{border:1px solid %1;border-radius:5px;"
                       "padding:2px;background:none;selection-background-color:%2;"
                       "selection-color:%3;}")
               .arg(btnBorderColor).arg(widgetBottomColor).arg(widgetTopColor));
    this->setStyleSheet(qss.join(""));
}
