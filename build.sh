#!/bin/bash
# 编译脚本 - 学生信息查询系统 GUI版本

echo "正在编译学生信息查询系统..."

# 检查Qt是否安装
if command -v qmake &> /dev/null; then
    echo "使用qmake编译..."
    qmake student_info.pro
    make
    if [ $? -eq 0 ]; then
        echo "编译成功！运行 ./student_info_gui 启动程序"
    else
        echo "编译失败"
        exit 1
    fi
elif command -v cmake &> /dev/null; then
    echo "使用CMake编译..."
    mkdir -p build
    cd build
    cmake ..
    make
    if [ $? -eq 0 ]; then
        echo "编译成功！运行 ./build/student_info_gui 启动程序"
    else
        echo "编译失败"
        exit 1
    fi
else
    echo "错误：未找到qmake或cmake，请先安装Qt开发库"
    echo "Ubuntu/Debian: sudo apt-get install qt6-base-dev 或 qt5-default"
    echo "Fedora: sudo dnf install qt6-qtbase-devel 或 qt5-qtbase-devel"
    exit 1
fi

