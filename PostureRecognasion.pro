#-------------------------------------------------
#
# Project created by QtCreator 2017-04-24T15:57:06
#
#-------------------------------------------------

QT       += core gui network axcontainer

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

RC_FILE = res.rc


TARGET = PostureRecognition
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h \
    webaxwidget.h

FORMS    += mainwindow.ui

RESOURCES += \
    image/image.qrc




