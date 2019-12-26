#-------------------------------------------------
#
# Project created by QtCreator 2019-05-15T11:24:54
#
#-------------------------------------------------

QT       += gui sql widgets

#  配置Qt版本5.8以上才可以使用qml模块
#check Qt version
QT_VERSION = $$[QT_VERSION]
QT_VERSION = $$split(QT_VERSION, ".")
QT_VER_MAJ = $$member(QT_VERSION, 0)
QT_VER_MIN = $$member(QT_VERSION, 1)

greaterThan(QT_VER_MAJ, 5) | greaterThan(QT_VER_MIN, 7) {
        message(Qt版本大于5.7配置qml模块)
        QT += qml
}
else{
        message(Qt版本小于5.7)
}

TARGET = QkeyTools
TEMPLATE = lib

DEFINES += QKEYTOOLS_LIBRARY

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        qkeytools.cpp

HEADERS += \
        qkeytools.h \
        qkeytools_global.h 

unix {
    target.path = /usr/lib
    INSTALLS += target
}

FORMS += \
    qkeytools.ui
