# 学生信息查询系统 - GUI版本使用说明

## 概述

这是一个基于索引顺序表和二级索引的学生信息查询系统，采用Qt图形界面，提供了友好的用户交互体验。

## 功能特点

### 1. 创建表
- **表格输入方式**：在表格中直接输入专业、班级、学号、姓名
- **文本输入方式**：使用CSV格式批量输入（格式：专业,班级,学号,姓名）
- 支持添加/删除行
- 自动排序（专业按名称，班级按编号）

### 2. 插入记录
- 单条插入学生记录
- 自动创建不存在的专业和班级
- 实时保存到CSV文件

### 3. 查询学生
- 根据专业、班级、学号精确查询
- 使用二级索引提高查询效率
- 显示查询结果

### 4. 数据浏览
- 表格形式显示所有数据
- 支持查看专业、班级、学号、姓名
- 自动刷新

### 5. 计算ASL
- 计算平均查找长度
- 考虑专业索引、班级索引和班内顺序查找

### 6. 数据持久化
- 自动保存到 students.csv
- 支持手动保存/加载CSV文件
- 启动时自动加载数据

## 编译和运行

### 方法1：使用编译脚本（推荐）
```bash
./build.sh
./student_info_gui
```

### 方法2：使用qmake
```bash
qmake student_info.pro
make
./student_info_gui
```

### 方法3：使用CMake
```bash
mkdir build && cd build
cmake ..
make
./student_info_gui
```

## 系统要求

- Qt 5.12+ 或 Qt 6.0+
- C++17 编译器（g++ 7+ 或 clang++ 5+）
- CMake 3.10+ 或 qmake

## 安装Qt开发库

### Ubuntu/Debian
```bash
sudo apt-get update
sudo apt-get install qt6-base-dev qt6-base-dev-tools
# 或使用Qt5
sudo apt-get install qt5-default qtbase5-dev
```

### Fedora
```bash
sudo dnf install qt6-qtbase-devel qt6-qttools-devel
# 或使用Qt5
sudo dnf install qt5-qtbase-devel
```

### Arch Linux
```bash
sudo pacman -S qt6-base
# 或使用Qt5
sudo pacman -S qt5-base
```

## 数据结构说明

### 索引顺序表结构
- **一级索引**：专业索引（按专业名称有序）
- **二级索引**：班级索引（每个专业内按班级编号有序）
- **数据区**：班级内学生记录（无序，按录入顺序）

### 查找算法
1. 专业索引：二分查找（O(log M)）
2. 班级索引：二分查找（O(log C)）
3. 学生记录：顺序查找（O(n)）

### ASL计算
平均查找长度 = E[专业索引] + E[班级索引] + E[班内顺序查找]

## 文件说明

- `main_gui.cpp` - GUI主程序
- `4.cpp` - 原始终端版本（保留）
- `CMakeLists.txt` - CMake构建文件
- `student_info.pro` - qmake项目文件
- `build.sh` - 编译脚本
- `students.csv` - 数据文件（自动生成）

## 使用示例

### 创建表示例
1. 点击"创建表"按钮
2. 选择"表格输入"标签
3. 点击"添加行"添加记录
4. 输入：专业名称、班级号、学号、姓名
5. 点击"确定"完成创建

### 插入记录示例
1. 切换到"插入记录"标签
2. 填写：专业名称、班级号、学号、姓名
3. 点击"插入记录"按钮

### 查询示例
1. 切换到"查询学生"标签
2. 输入：专业名称、班级号、学号
3. 点击"查询"按钮查看结果

## 注意事项

1. 创建新表会清空现有数据，请谨慎操作
2. 数据会自动保存到 students.csv
3. 专业和班级会自动排序
4. 班级内学生记录保持录入顺序（无序）

# CourseWork_datastructure2025_text4_gradeplus
