# -------------------------------------------------
# Project created by QtCreator 2010-01-09T00:05:53
# -------------------------------------------------
QT += network \
    testlib \
    xml
TARGET = geekast
TEMPLATE = app
QXT     += core
SOURCES += main.cpp \
    core_linux.cpp \
    core_windows.cpp \
    core.cpp \
    core_mac.cpp \
    push.cpp \
    mainwindow.cpp \
    xmpp_client.cpp
HEADERS += core_linux.h \
    core.h \
    push.h \
    mainwindow.h \
    xmpp_client.h
FORMS += mainwindow.ui
RESOURCES += ../geekast.qrc
#LIBS += -Lexternal/hyperic-sigar-1.6.3/sigar-bin/lib/ \
LIBS += -lqxmpp \
        -lsigar \
        -lQxtCore

INCLUDEPATH += /usr/include/QxtCore
INCLUDEPATH += /usr/include/qxt/
INCLUDEPATH += /usr/include/qxt/QxtCore
INCLUDEPATH += /usr/include/qxmpp
#INCLUDEPATH += ./externals/
#INCLUDEPATH += ./externals/qxmpp-0.3.0/src/
# LIBS += -LC:\code\hyperic-sigar-1.6.3\sigar-bin\lib -lsigar-x86-winnt
# INCLUDEPATH += C:\code\hyperic-sigar-1.6.3\sigar-bin\include
#INCLUDEPATH += external/hyperic-sigar-1.6.3/sigar-bin/include
