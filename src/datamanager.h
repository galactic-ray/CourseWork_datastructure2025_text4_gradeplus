// datamanager.h
// 数据结构和CSV持久化管理

#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <cmath>

/*======================= 基本数据结构（顺序表） =======================*/
struct Student {
    std::string id, name;
};

struct ClassRec {
    std::string classID;
    std::vector<Student> students;
};

struct MajorRec {
    std::string majorName;
    std::vector<ClassRec> classes;
};

/*======================= 二级顺序表索引 =======================*/
struct MIdx { 
    std::string key; 
    int pos; 
};

struct CIdx { 
    std::string key; 
    int pos; 
};

// 全局数据
extern std::vector<MajorRec> gMajors;
extern std::vector<MIdx> majorIndex;
extern std::vector<std::vector<CIdx>> classIndex;

/*======================= 索引操作 =======================*/
void rebuildIndexes();
int findMajorByIndex(const std::string& majorName);
int findClassByIndex(int majorPos, const std::string& classID);

/*======================= CSV 持久化 =======================*/
std::string csvEscape(const std::string& s);
std::vector<std::string> parseCsvLine(const std::string& line);
int ensureMajor(const std::string& major);
int ensureClass(int mi, const std::string& cls);
bool saveCSV(const std::string& file = "data/students.csv");
bool loadCSV(const std::string& file = "data/students.csv");

/*======================= ASL 计算 =======================*/
double calcASL_strict();

#endif // DATAMANAGER_H

