#-------------------------------------------------
#
# Project created by QtCreator 2020-09-05T15:49:45
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Client
TEMPLATE = app


SOURCES += main.cpp\
    registerr.cpp \
    communicate.cpp \
    login.cpp \
    brainstorm.cpp

HEADERS  += \
    registerr.h \
    communicate.h \
    login.h \
    common.h \
    brainstorm.h

FORMS    += \
    registerr.ui \
    login.ui \
    brainstorm.ui
