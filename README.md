# 学生信息查询系统

基于索引顺序表和二级索引的学生信息管理系统（Qt GUI版本）

## 项目结构

```
project/
│
├── src/                    # 源代码目录
│   ├── main_gui.cpp        # 主函数入口
│   ├── mainwindow.h        # 主窗口类声明
│   ├── mainwindow.cpp      # 主窗口类实现
│   ├── datamanager.h       # 数据结构和CSV操作声明
│   └── datamanager.cpp     # 数据结构和CSV操作实现
│
├── data/                   # 数据文件目录
│   └── students.csv       # 学生数据CSV文件（自动生成）
│
├── build/                  # 编译输出目录（自动生成，已忽略）
│
├── CMakeLists.txt         # CMake构建文件
├── student_info.pro       # qmake项目文件
├── build.sh               # 编译脚本
├── README.md              # 本文件
└── .gitignore             # Git忽略文件
```

## 编译和运行

### 方法1：使用编译脚本（推荐）

```bash
./build.sh
./build/student_info_gui
```

### 方法2：使用qmake

```bash
qmake student_info.pro
make
./build/student_info_gui
```

### 方法3：使用CMake

```bash
mkdir -p build && cd build
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

## 功能说明

### 主要功能

1. **创建表** - 批量创建专业、班级和学生信息
   - 支持表格输入和文本输入两种方式
2. **插入记录** - 单条插入学生记录
3. **查询学生** - 根据专业、班级、学号查询
4. **数据浏览** - 表格形式查看所有数据
5. **计算ASL** - 计算平均查找长度
6. **保存/加载CSV** - 数据持久化

### 数据结构

- **一级索引**：专业索引（按专业名称有序）
- **二级索引**：班级索引（每个专业内按班级编号有序）
- **数据区**：班级内学生记录（无序，按录入顺序）

### 查找算法

1. 专业索引：二分查找（O(log M)）
2. 班级索引：二分查找（O(log C)）
3. 学生记录：顺序查找（O(n)）

### ASL计算

平均查找长度 = E[专业索引] + E[班级索引] + E[班内顺序查找]

## 数据文件

数据文件默认保存在 `data/students.csv`，格式为：

```csv
kind,major,class,id,name
M,专业名称,,,
C,专业名称,班级号,,
S,专业名称,班级号,学号,姓名
```

## 注意事项

1. 创建新表会清空现有数据，请谨慎操作
2. 数据会自动保存到 `data/students.csv`
3. 专业和班级会自动排序
4. 班级内学生记录保持录入顺序（无序）
