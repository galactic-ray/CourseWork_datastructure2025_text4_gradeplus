// mainwindow.h
// 主窗口类声明

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QTableWidget>
#include <QLabel>
#include <QString>
#include "datamanager.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

private slots:
    void onCreateTable();
    void onInsertRecord();
    void onSearchStudent();
    void onCalculateASL();
    void onSaveCSV();
    void onLoadCSV();
    void refreshDataDisplay();
    void updateStatus(const QString &msg);

private:
    void setupUI();
    
    QLineEdit *majorInput, *classInput, *idInput, *nameInput;
    QLineEdit *searchMajorInput, *searchClassInput, *searchIdInput;
    QTableWidget *dataTable;
    QLabel *statusLabel;
};

#endif // MAINWINDOW_H

