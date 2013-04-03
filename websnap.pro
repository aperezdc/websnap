
QMAKE_CXXFLAGS_RELEASE *= -s
QMAKE_LFLAGS_RELEASE   *= -s

LIBS        *= -ldl -lm
CONFIG      *= debug_and_release
QT          *= core gui webkit network
TEMPLATE     = app
TARGET       = 
DEPENDPATH  += .
INCLUDEPATH += .
OBJECTS_DIR  = .build
MOC_DIR      = .build

# Input
HEADERS += websnap.h clock.h perf.h gcpuprofiler.h
SOURCES += websnap.cc main.cc clock.cc perf.cc gcpuprofiler.cc
