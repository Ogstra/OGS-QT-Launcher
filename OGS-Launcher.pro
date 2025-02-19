QT += core gui widgets

CONFIG += c++17

SOURCES += src/main.cpp \
           src/app/myapp.cpp
           
HEADERS += src/app/myapp.h

INCLUDEPATH += /lib/QHotkey/include

LIBS += -L/lib/QHotkey/include -lqhotkey

DEPENDPATH += /lib/QHotkey/include

CONFIG(debug, debug|release) {
    DESTDIR = $$PWD/build/test
    TARGET = OGS-Launcher
    DEFINES += TEST_MODE
}

CONFIG(release, debug|release) {
    DESTDIR = /usr/local/bin
    TARGET = OGS-Launcher
}

QMAKE_POST_LINK += cp $$PWD/src/ogs.launcher.desktop /usr/share/applications/