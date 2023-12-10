QT += core
QT -= gui

CONFIG += c++11

TARGET = lora_control
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

INCLUDEPATH += WiMODLR
INCLUDEPATH += Utils

SOURCES += main.cpp \
    Utils/ComSlip.cpp \
    Utils/CRC16.cpp \
    Utils/KeyValueList.cpp \
    Utils/RegistryKey.cpp \
    Utils/SerialDevice.cpp \
    WiMODLR/WiMODLRHCI.cpp \
    MainWindow2.cpp

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

HEADERS += \
    Utils/ComSlip.h \
    Utils/CRC16.h \
    Utils/KeyValueList.h \
    Utils/RegistryKey.h \
    Utils/SerialDevice.h \
    WiMODLR/WiMODLRHCI.h \
    WiMODLR/WiMODLRHCI_IDs.h \
    WiMODLR/WMDefs.h \
    MainWindow2.h
