QT += core widgets
CONFIG += c++17

TARGET = student_info_gui
TEMPLATE = app

# 源文件
SOURCES += src/main_gui.cpp \
           src/mainwindow.cpp \
           src/datamanager.cpp

# 头文件
HEADERS += src/mainwindow.h \
           src/datamanager.h

# 输出目录
DESTDIR = build
OBJECTS_DIR = build/obj
MOC_DIR = build/moc
RCC_DIR = build/rcc
UI_DIR = build/ui

