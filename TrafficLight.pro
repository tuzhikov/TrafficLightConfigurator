#-------------------------------------------------
#
# Project created by QtCreator 2018-03-23T15:30:46
#
#-------------------------------------------------

QT       += core gui serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TrafficLight
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp \
    src/forms/mainwindows.cpp \
    src/forms/setting_port.cpp \
    src/interface/comport.cpp \
    src/controller/controller.cpp \
    src/settings/settings.cpp

HEADERS  += \
    src/forms/mainwindows.h \
    src/forms/setting_port.h \
    src/controller/controller.h \
    src/controller/crc.h \
    src/interface/comport.h \
    src/interface/link_interface.h \
    src/settings/settings.h

FORMS    += mainwindow.ui \
    settingsdialog.ui

RESOURCES += \
    resources.qrc
TRANSLATIONS += \
    TS/language_ru.ts
