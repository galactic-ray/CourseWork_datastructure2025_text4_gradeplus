// mainwindow.cpp
// 主窗口类实现

#include "mainwindow.h"
#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTextEdit>
#include <QTableWidgetItem>
#include <QMessageBox>
#include <QHeaderView>
#include <QFileDialog>
#include <QTabWidget>
#include <QFormLayout>
#include <QDialog>
#include <QDialogButtonBox>
#include <QDir>
#include <algorithm>
#include <string>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle("学生信息查询系统 - 索引顺序表");
    setMinimumSize(900, 700);
    
    // 确保data目录存在
    QDir().mkdir("data");
    
    // 先创建UI
    setupUI();
    
    // 然后加载数据
    if (loadCSV("data/students.csv")) {
        updateStatus("已从 data/students.csv 加载数据");
    } else {
        updateStatus("未发现 data/students.csv，当前为空表");
    }
    
    refreshDataDisplay();
}

void MainWindow::onCreateTable() {
    QDialog dialog(this);
    dialog.setWindowTitle("创建表");
    dialog.resize(600, 500);
    
    QVBoxLayout *mainLayout = new QVBoxLayout(&dialog);
    
    QLabel *infoLabel = new QLabel("提示：创建新表将清空现有数据！", &dialog);
    infoLabel->setStyleSheet("color: red; font-weight: bold;");
    mainLayout->addWidget(infoLabel);
    
    QTabWidget *tabs = new QTabWidget(&dialog);
    
    // 方式1: 表格输入
    QWidget *tableTab = new QWidget();
    QVBoxLayout *tableLayout = new QVBoxLayout(tableTab);
    
    QTableWidget *createTable = new QTableWidget(tableTab);
    createTable->setColumnCount(4);
    createTable->setHorizontalHeaderLabels(QStringList() << "专业名称" << "班级号" << "学号" << "姓名");
    createTable->horizontalHeader()->setStretchLastSection(true);
    tableLayout->addWidget(createTable);
    
    QHBoxLayout *btnLayout1 = new QHBoxLayout();
    QPushButton *addRowBtn = new QPushButton("添加行", tableTab);
    QPushButton *delRowBtn = new QPushButton("删除行", tableTab);
    btnLayout1->addWidget(addRowBtn);
    btnLayout1->addWidget(delRowBtn);
    btnLayout1->addStretch();
    tableLayout->addLayout(btnLayout1);
    
    connect(addRowBtn, &QPushButton::clicked, [=]() {
        createTable->insertRow(createTable->rowCount());
    });
    connect(delRowBtn, &QPushButton::clicked, [=]() {
        int row = createTable->currentRow();
        if (row >= 0) createTable->removeRow(row);
    });
    
    tabs->addTab(tableTab, "表格输入");
    
    // 方式2: 文本输入
    QWidget *textTab = new QWidget();
    QVBoxLayout *textLayout = new QVBoxLayout(textTab);
    
    QTextEdit *inputArea = new QTextEdit(textTab);
    inputArea->setPlaceholderText("输入格式（每行一条记录）：\n专业名称,班级号,学号,姓名\n例如：\n计算机科学,CS2021,2021001,张三\n计算机科学,CS2021,2021002,李四\n软件工程,SE2021,2022001,王五");
    textLayout->addWidget(inputArea);
    
    tabs->addTab(textTab, "文本输入");
    
    mainLayout->addWidget(tabs);
    
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    mainLayout->addWidget(buttonBox);
    
    connect(buttonBox, &QDialogButtonBox::accepted, [&]() {
        gMajors.clear();
        bool success = false;
        
        if (tabs->currentIndex() == 0) {
            // 表格方式
            for (int i = 0; i < createTable->rowCount(); ++i) {
                QTableWidgetItem *item0 = createTable->item(i, 0);
                QTableWidgetItem *item1 = createTable->item(i, 1);
                QTableWidgetItem *item2 = createTable->item(i, 2);
                QTableWidgetItem *item3 = createTable->item(i, 3);
                
                if (!item0 || !item1 || !item2 || !item3) continue;
                
                std::string major = item0->text().toStdString();
                std::string cls = item1->text().toStdString();
                std::string id = item2->text().toStdString();
                std::string name = item3->text().toStdString();
                
                if (major.empty() || cls.empty() || id.empty() || name.empty()) continue;
                
                int mi = ensureMajor(major);
                int ci = ensureClass(mi, cls);
                Student s; s.id = id; s.name = name;
                gMajors[mi].classes[ci].students.push_back(s);
                success = true;
            }
        } else {
            // 文本方式
            QStringList lines = inputArea->toPlainText().split('\n');
            for (const QString &line : lines) {
                QStringList parts = line.split(',');
                if (parts.size() >= 4) {
                    std::string major = parts[0].trimmed().toStdString();
                    std::string cls = parts[1].trimmed().toStdString();
                    std::string id = parts[2].trimmed().toStdString();
                    std::string name = parts[3].trimmed().toStdString();
                    
                    if (major.empty() || cls.empty() || id.empty() || name.empty()) continue;
                    
                    int mi = ensureMajor(major);
                    int ci = ensureClass(mi, cls);
                    Student s; s.id = id; s.name = name;
                    gMajors[mi].classes[ci].students.push_back(s);
                    success = true;
                }
            }
        }
        
        if (success) {
            std::sort(gMajors.begin(), gMajors.end(),
                [](const MajorRec&a, const MajorRec&b){ return a.majorName < b.majorName; });
            for (auto &m : gMajors)
                std::sort(m.classes.begin(), m.classes.end(),
                    [](const ClassRec&a, const ClassRec&b){ return a.classID < b.classID; });
            rebuildIndexes();
            saveCSV("data/students.csv");
            refreshDataDisplay();
            updateStatus("表创建成功并已保存");
            dialog.accept();
        } else {
            QMessageBox::warning(&dialog, "错误", "没有有效数据，请至少输入一条完整记录");
        }
    });
    
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    
    dialog.exec();
}

void MainWindow::onInsertRecord() {
    std::string major = majorInput->text().toStdString();
    std::string cls = classInput->text().toStdString();
    std::string id = idInput->text().toStdString();
    std::string name = nameInput->text().toStdString();
    
    if (major.empty() || cls.empty() || id.empty() || name.empty()) {
        QMessageBox::warning(this, "错误", "请填写所有字段");
        return;
    }
    
    int mi = findMajorByIndex(major);
    if (mi == -1) {
        gMajors.push_back(MajorRec{major, {}});
        std::sort(gMajors.begin(), gMajors.end(),
            [](const MajorRec&a, const MajorRec&b){ return a.majorName < b.majorName; });
        rebuildIndexes();
        mi = findMajorByIndex(major);
    }
    int ci = findClassByIndex(mi, cls);
    if (ci == -1) {
        gMajors[mi].classes.push_back(ClassRec{cls, {}});
        std::sort(gMajors[mi].classes.begin(), gMajors[mi].classes.end(),
            [](const ClassRec&a, const ClassRec&b){ return a.classID < b.classID; });
        rebuildIndexes();
        ci = findClassByIndex(mi, cls);
    }
    
    Student s; s.id = id; s.name = name;
    gMajors[mi].classes[ci].students.push_back(s);
    saveCSV("data/students.csv");
    refreshDataDisplay();
    updateStatus("插入成功");
    
    majorInput->clear();
    classInput->clear();
    idInput->clear();
    nameInput->clear();
}

void MainWindow::onSearchStudent() {
    std::string major = searchMajorInput->text().toStdString();
    std::string cls = searchClassInput->text().toStdString();
    std::string id = searchIdInput->text().toStdString();
    
    if (major.empty() || cls.empty() || id.empty()) {
        QMessageBox::warning(this, "错误", "请填写所有查询字段");
        return;
    }
    
    int mi = findMajorByIndex(major);
    if (mi == -1) {
        QMessageBox::information(this, "查询结果", "未找到该专业");
        return;
    }
    int ci = findClassByIndex(mi, cls);
    if (ci == -1) {
        QMessageBox::information(this, "查询结果", "未找到该班级");
        return;
    }
    
    auto &vec = gMajors[mi].classes[ci].students;
    for (auto &s : vec) {
        if (s.id == id) {
            QString result = QString("找到学生：\n学号：%1\n姓名：%2")
                .arg(QString::fromStdString(s.id))
                .arg(QString::fromStdString(s.name));
            QMessageBox::information(this, "查询结果", result);
            return;
        }
    }
    QMessageBox::information(this, "查询结果", "未找到该学生");
}

void MainWindow::onCalculateASL() {
    double asl = calcASL_strict();
    QString result = QString("平均查找长度 (ASL) ≈ %1").arg(asl, 0, 'f', 2);
    QMessageBox::information(this, "ASL计算结果", result);
    updateStatus(result);
}

void MainWindow::onSaveCSV() {
    QString fileName = QFileDialog::getSaveFileName(this, "保存CSV文件", "data/students.csv", "CSV Files (*.csv)");
    if (!fileName.isEmpty()) {
        if (saveCSV(fileName.toStdString())) {
            QMessageBox::information(this, "成功", "数据已保存到 " + fileName);
            updateStatus("数据已保存");
        } else {
            QMessageBox::warning(this, "错误", "保存失败");
        }
    }
}

void MainWindow::onLoadCSV() {
    QString fileName = QFileDialog::getOpenFileName(this, "加载CSV文件", "data", "CSV Files (*.csv)");
    if (!fileName.isEmpty()) {
        if (loadCSV(fileName.toStdString())) {
            refreshDataDisplay();
            size_t cntM=gMajors.size(), cntC=0, cntS=0;
            for(auto&m:gMajors){ cntC+=m.classes.size(); for(auto&c:m.classes) cntS+=c.students.size(); }
            QString msg = QString("已加载：专业=%1 班级=%2 学生=%3").arg(cntM).arg(cntC).arg(cntS);
            QMessageBox::information(this, "成功", msg);
            updateStatus(msg);
        } else {
            QMessageBox::warning(this, "错误", "加载失败");
        }
    }
}

void MainWindow::refreshDataDisplay() {
    dataTable->clear();
    dataTable->setRowCount(0);
    dataTable->setColumnCount(4);
    dataTable->setHorizontalHeaderLabels(QStringList() << "专业" << "班级" << "学号" << "姓名");
    dataTable->horizontalHeader()->setStretchLastSection(true);
    
    int row = 0;
    for (auto &m : gMajors) {
        for (auto &c : m.classes) {
            if (c.students.empty()) {
                dataTable->insertRow(row);
                dataTable->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(m.majorName)));
                dataTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(c.classID)));
                dataTable->setItem(row, 2, new QTableWidgetItem("(空)"));
                dataTable->setItem(row, 3, new QTableWidgetItem(""));
                row++;
            } else {
                for (auto &s : c.students) {
                    dataTable->insertRow(row);
                    dataTable->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(m.majorName)));
                    dataTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(c.classID)));
                    dataTable->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(s.id)));
                    dataTable->setItem(row, 3, new QTableWidgetItem(QString::fromStdString(s.name)));
                    row++;
                }
            }
        }
    }
    
    // 更新统计信息
    size_t cntM=gMajors.size(), cntC=0, cntS=0;
    for(auto&m:gMajors){ cntC+=m.classes.size(); for(auto&c:m.classes) cntS+=c.students.size(); }
    if (statusLabel) {
        statusLabel->setText(QString("专业: %1 | 班级: %2 | 学生: %3").arg(cntM).arg(cntC).arg(cntS));
    }

    refreshStructureView();
}

void MainWindow::updateStatus(const QString &msg) {
    if (statusLabel) {
        statusLabel->setText(msg);
    }
}

void MainWindow::setupUI() {
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    
    // 标签页
    QTabWidget *tabs = new QTabWidget(this);
    
    // 标签1: 插入记录
    QWidget *insertTab = new QWidget();
    QFormLayout *insertLayout = new QFormLayout(insertTab);
    majorInput = new QLineEdit();
    classInput = new QLineEdit();
    idInput = new QLineEdit();
    nameInput = new QLineEdit();
    insertLayout->addRow("专业名称:", majorInput);
    insertLayout->addRow("班级号:", classInput);
    insertLayout->addRow("学号:", idInput);
    insertLayout->addRow("姓名:", nameInput);
    QPushButton *insertBtn = new QPushButton("插入记录", insertTab);
    insertLayout->addRow(insertBtn);
    connect(insertBtn, &QPushButton::clicked, this, &MainWindow::onInsertRecord);
    tabs->addTab(insertTab, "插入记录");
    
    // 标签2: 查询学生
    QWidget *searchTab = new QWidget();
    QFormLayout *searchLayout = new QFormLayout(searchTab);
    searchMajorInput = new QLineEdit();
    searchClassInput = new QLineEdit();
    searchIdInput = new QLineEdit();
    searchLayout->addRow("专业名称:", searchMajorInput);
    searchLayout->addRow("班级号:", searchClassInput);
    searchLayout->addRow("学号:", searchIdInput);
    QPushButton *searchBtn = new QPushButton("查询", searchTab);
    searchLayout->addRow(searchBtn);
    connect(searchBtn, &QPushButton::clicked, this, &MainWindow::onSearchStudent);
    tabs->addTab(searchTab, "查询学生");
    
    // 标签3: 数据浏览
    QWidget *viewTab = new QWidget();
    QVBoxLayout *viewLayout = new QVBoxLayout(viewTab);
    dataTable = new QTableWidget(viewTab);
    viewLayout->addWidget(dataTable);
    tabs->addTab(viewTab, "数据浏览");

    // 标签4: 结构视图
    QWidget *structureTab = new QWidget();
    QVBoxLayout *structureLayout = new QVBoxLayout(structureTab);
    QLabel *structureHint = new QLabel("说明：专业按名称升序排列，专业内班级按编号升序排列，班级内学生保持录入顺序（表示无序表）。", structureTab);
    structureHint->setWordWrap(true);
    structureLayout->addWidget(structureHint);
    structureTree = new QTreeWidget(structureTab);
    structureTree->setColumnCount(2);
    structureTree->setHeaderLabels(QStringList() << "节点" << "信息");
    structureTree->header()->setStretchLastSection(true);
    structureLayout->addWidget(structureTree);
    tabs->addTab(structureTab, "结构视图");
    
    mainLayout->addWidget(tabs);
    
    // 按钮组
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *createBtn = new QPushButton("创建表", this);
    QPushButton *aslBtn = new QPushButton("计算ASL", this);
    QPushButton *saveBtn = new QPushButton("保存CSV", this);
    QPushButton *loadBtn = new QPushButton("加载CSV", this);
    QPushButton *refreshBtn = new QPushButton("刷新", this);
    
    connect(createBtn, &QPushButton::clicked, this, &MainWindow::onCreateTable);
    connect(aslBtn, &QPushButton::clicked, this, &MainWindow::onCalculateASL);
    connect(saveBtn, &QPushButton::clicked, this, &MainWindow::onSaveCSV);
    connect(loadBtn, &QPushButton::clicked, this, &MainWindow::onLoadCSV);
    connect(refreshBtn, &QPushButton::clicked, this, &MainWindow::refreshDataDisplay);
    
    buttonLayout->addWidget(createBtn);
    buttonLayout->addWidget(aslBtn);
    buttonLayout->addWidget(saveBtn);
    buttonLayout->addWidget(loadBtn);
    buttonLayout->addWidget(refreshBtn);
    buttonLayout->addStretch();
    
    mainLayout->addLayout(buttonLayout);
    
    // 状态栏
    statusLabel = new QLabel("就绪", this);
    mainLayout->addWidget(statusLabel);
}

void MainWindow::refreshStructureView() {
    if (!structureTree) return;

    structureTree->clear();
    structureTree->setColumnCount(2);
    structureTree->setHeaderLabels(QStringList() << "节点" << "信息");
    structureTree->header()->setStretchLastSection(true);

    for (auto &m : gMajors) {
        size_t classCount = m.classes.size();
        size_t studentCount = 0;
        for (auto &c : m.classes) studentCount += c.students.size();

        auto *majorItem = new QTreeWidgetItem(structureTree);
        majorItem->setText(0, QString::fromStdString(m.majorName));
        majorItem->setText(1, QString("班级: %1 | 学生: %2").arg(classCount).arg(studentCount));

        for (auto &c : m.classes) {
            auto *classItem = new QTreeWidgetItem(majorItem);
            classItem->setText(0, QString::fromStdString(c.classID));
            classItem->setText(1, QString("学生: %1").arg(c.students.size()));

            for (auto &s : c.students) {
                auto *studentItem = new QTreeWidgetItem(classItem);
                studentItem->setText(0, QString::fromStdString(s.id));
                studentItem->setText(1, QString::fromStdString(s.name));
            }
        }
    }

    structureTree->expandAll();
}

