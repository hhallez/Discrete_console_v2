QT += core
QT -= gui

CONFIG += c++11

TARGET = Discrete_console
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += src/main.cpp \
    src/drivers/ADS1015.c \
    src/drivers/bma400.c \
    src/drivers/bme680.c \
    src/drivers/i2c_blocking.c \
    src/drivers/MIKROE_2467.c \
    src/drivers/MIKROE_3056.c \
    src/drivers/MIKROE_3085.c \
    src/drivers/MIKROE_3149.c \
    src/drivers/peripheral_controller.c \
    src/drivers/si7034.c \
    src/drivers/MultichannelGasSensor.c

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
    src/drivers/ADS1015.h \
    src/drivers/bma400_defs.h \
    src/drivers/bma400.h \
    src/drivers/bme680_defs.h \
    src/drivers/bme680.h \
    src/drivers/i2c_blocking.h \
    src/drivers/MIKROE_2467.h \
    src/drivers/MIKROE_3056.h \
    src/drivers/MIKROE_3085.h \
    src/drivers/MIKROE_3149.h \
    src/drivers/peripheral_controller.h \
    src/drivers/si7034.h \
    src/influxdb.h \
    src/drivers/MultichannelGasSensor.h \
    src/peripheral_controller.h

DISTFILES += \
    settings.conf \
    README.md
