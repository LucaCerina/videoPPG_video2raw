TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += link_pkgconfig
PKGCONFIG += opencv

#QMAKE_CXXFLAGS = $(shell pkg-config --cflags opencv) -I /usr/local/boost_1_60_0
#LIBS = $(shell pkg-config --libs opencv) -L/usr/local/boost_1_60_0 -L/usr/share/lintian/overrides

INCLUDEPATH += /usr/local/include/opencv2 /usr/include
LIBS += -L/usr/local/lib -L /usr/lib -lopencv_core -lopencv_imgcodecs -lopencv_videoio -lopencv_imgproc -lopencv_objdetect -lopencv_highgui -lavformat -lavutil

SOURCES += main.cpp \
    roifunctions.cpp \
    geomfunctions.cpp \
    trackerfunctions.cpp

HEADERS += \
    roifunctions.h \
    geomfunctions.h \
    trackerfunctions.h
