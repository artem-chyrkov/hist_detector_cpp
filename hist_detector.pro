#CONFIG -= qt

OBJECTS_DIR = obj
#DESTDIR = bin

LIBS += -LC:/Development/opencv/build/x64/vc14/lib -lopencv_world343

INCLUDEPATH += \
    C:/Development/opencv/sources/include/opencv2 \
    C:/Development/opencv/build/include

SOURCES += \
    test/main.cpp \
    hist_detector/HistDetector.cpp \
    hist_detector/SlidingStripeRois.cpp \
    hist_detector/statistics.cpp \
    hist_detector/mixtureSeparation.cpp

HEADERS += \
    hist_detector/HistDetector.h \
    hist_detector/CircularList.h \
    hist_detector/CircularList.inl \
    hist_detector/SlidingStripeRois.h \
    hist_detector/statistics.h \
    hist_detector/mixtureSeparation.h
