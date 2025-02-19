QT += core gui widgets

CONFIG += c++17

SOURCES += src/main.cpp \
           src/app/myapp.cpp
           
HEADERS += src/app/myapp.h

INCLUDEPATH += $$PWD/lib/QHotkey/QHotkey
LIBS += -L$$PWD/lib/QHotkey/QHotkey -lqhotkey


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