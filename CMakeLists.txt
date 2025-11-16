cmake_minimum_required(VERSION 3.10)
project(StudentInfoSystem)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# 查找Qt6，如果找不到则查找Qt5
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Widgets)

# 设置自动处理MOC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 源文件
set(SOURCES
    src/main_gui.cpp
    src/mainwindow.cpp
    src/datamanager.cpp
)

set(HEADERS
    src/mainwindow.h
    src/datamanager.h
)

# GUI版本
add_executable(student_info_gui ${SOURCES} ${HEADERS})
target_link_libraries(student_info_gui Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Widgets)

# 设置包含目录
target_include_directories(student_info_gui PRIVATE src)

