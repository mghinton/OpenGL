QT       += core gui opengl widgets declarative
QT += multimedia
TARGET   = Fire
TEMPLATE = app

SOURCES += main.cpp\
           mainwindow.cpp \
           glwidget.cpp


HEADERS  += mainwindow.h \
            glwidget.h

FORMS    += mainwindow.ui

OTHER_FILES += \
    fire2.png \
    fire.bmp
