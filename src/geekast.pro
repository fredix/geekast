# -------------------------------------------------
# Project created by QtCreator 2010-01-09T00:05:53
# -------------------------------------------------
QT += network \
    testlib \
    xml
TARGET = geekast
TEMPLATE = app
SOURCES += main.cpp \
    core_linux.cpp \
    core_windows.cpp \
    core.cpp \
    core_mac.cpp \
    push.cpp \
    mainwindow.cpp
HEADERS += core_linux.h \
    core.h \
    push.h \
    mainwindow.h
FORMS += mainwindow.ui
RESOURCES += ../geekast.qrc
#LIBS += -Lexternal/hyperic-sigar-1.6.3/sigar-bin/lib/ \
LIBS += -L/externals/qxmpp/lib/libqxmpp.a \
     -lsigar-amd64-linux \
     -lqjson

INCLUDEPATH += /usr/include/qxt/
INCLUDEPATH += /usr/include/qxt/QxtCore
INCLUDEPATH += ./externals/
INCLUDEPATH += ./externals/qxmpp/src/
INCLUDEPATH += /usr/include/qjson
# LIBS += -LC:\code\hyperic-sigar-1.6.3\sigar-bin\lib -lsigar-x86-winnt
# INCLUDEPATH += C:\code\hyperic-sigar-1.6.3\sigar-bin\include
#INCLUDEPATH += external/hyperic-sigar-1.6.3/sigar-bin/include
