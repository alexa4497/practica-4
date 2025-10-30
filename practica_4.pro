TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        main.cpp \
        network.cpp \
        router.cpp

HEADERS += \
    Network.h \
    Router.h
