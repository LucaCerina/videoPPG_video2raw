TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

#QMAKE_CXXFLAGS = $(shell pkg-config --cflags opencv) -I /usr/local/boost_1_60_0
#LIBS = $(shell pkg-config --libs opencv) -L/usr/local/boost_1_60_0 -L/usr/share/lintian/overrides

!win32{
CONFIG += link_pkgconfig
PKGCONFIG += opencv
INCLUDEPATH += /usr/local/include/opencv2 /usr/include
LIBS += -L/usr/local/lib -L /usr/lib -lopencv_core -lopencv_imgcodecs -lopencv_videoio -lopencv_imgproc -lopencv_objdetect -lopencv_highgui -lavformat -lavutil
}
win32{
INCLUDEPATH += C:\Users\Bio-tec\Documents\opencv\build\include
Debug:LIBS += C:\Users\Bio-tec\Documents\opencv\build\x64\vc14\lib\opencv_world310d.lib
Release:LIBS += C:\Users\Bio-tec\Documents\opencv\build\x64\vc14\lib\opencv_world310.lib
}

SOURCES += main.cpp \
    roifunctions.cpp \
    geomfunctions.cpp \
    trackerfunctions.cpp

HEADERS += \
    roifunctions.h \
    geomfunctions.h \
    trackerfunctions.h

DISTFILES += \
    3rdparty/haarcascade_frontalface_alt.xml \
    3rdparty/haarcascade_lefteye_2splits.xml \
    3rdparty/haarcascade_righteye_2splits.xml

Debug::DESTDIR = $${OUT_PWD}/debug
Release::DESTDIR = $${OUT_PWD}/release
3rdparty.path   = $${DESTDIR}/3rdparty
3rdparty.files  = $${PWD}/3rdparty/*
INSTALLS       += 3rdparty
