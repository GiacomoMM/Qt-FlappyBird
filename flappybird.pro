QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    mainwindow.h

FORMS += \
    mainwindow.ui

ONNX_PATH = C:/Users/giaco/OneDrive/Desktop/onnxruntime-win-x64-1.19.2

PYTORCH_PATH = C:/Users/giaco/OneDrive/Desktop/libtorch

INCLUDEPATH += $$quote($$PYTORCH_PATH/include)
INCLUDEPATH += $$quote($$PYTORCH_PATH/include/torch/csrc/api/include)
INCLUDEPATH += $$quote($$ONNX_PATH/include)

# Additional settings for Windows
win32: {
    #CONFIG += debug_and_release
    #LIBS += -Wl,--add-stdcall-alias

    message("Using win32 configuration")


    CONFIG(debug, debug|release) {
    LIBS    += -L"$$PYTORCH_PATH/lib"
    LIBS    += $$quote($$PYTORCH_PATH/lib/*.lib)
    LIBS    += -L"$$ONNX_PATH/lib" -lonnxruntime
    #LIBS    += $$quote($$ONNX_PATH/lib/*.lib)
    }


    CONFIG(release, debug|release) {
    LIBS    += -L"$$PYTORCH_PATH/lib"
    LIBS    += $$quote($$PYTORCH_PATH/lib/*.lib)
    LIBS    += -L"$$ONNX_PATH/lib" -lonnxruntime
    #LIBS    += $$quote($$ONNX_PATH/lib/*.lib)
    }
}

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    bottompipe.png \
    flappybird.png \
    flappybirdImage.png \
    toppipe.png

RESOURCES += \
    flappybirdImage.qrc
