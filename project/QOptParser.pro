#-------------------------------------------------
#
# Project created by QtCreator 2012-07-25T12:28:41
#
#-------------------------------------------------

QT       -= gui

TARGET = QOptParser
TEMPLATE = lib

DESTDIR     = ../build
OBJECTS_DIR = ../build/.obj
MOC_DIR     = ../build/.moc
RCC_DIR     = ../build/.rcc
UI_DIR      = ../build/.ui

unittest {
    TEMPLATE     = app
    QT          += testlib
    SOURCES     += ../test/main.cpp
    INCLUDEPATH += ../src/
    TARGET       = test01
}

DEFINES += QOPTPARSER_LIBRARY

SOURCES += ../src/qoptparser.cpp \
           ../src/qoption.cpp

HEADERS += ../src/qoptparser.h\
           ../src/QOptParser_global.h \
           ../src/qoption.h \
           ../src/QOptParser

symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xEF17518C
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = QOptParser.dll
    addFiles.path = !:/sys/bin
    DEPLOYMENT += addFiles
}

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
