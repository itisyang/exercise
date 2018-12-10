QT += core gui

CONFIG += c++11

TARGET = myopengl
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cc \
    window.cc \
    transform3d.cc \
    input.cc \
    camera3d.cc

HEADERS += \
    window.h \
    vertex.h \
    transform3d.h \
    input.h \
    camera3d.h

RESOURCES += \
    resources.qrc
