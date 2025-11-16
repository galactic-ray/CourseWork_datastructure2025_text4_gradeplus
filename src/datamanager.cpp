// datamanager.cpp
// 数据结构和CSV持久化实现

#include "datamanager.h"
#include <sstream>

// 全局数据定义
std::vector<MajorRec> gMajors;
std::vector<MIdx> majorIndex;
std::vector<std::vector<CIdx>> classIndex;

/*======================= 索引操作 =======================*/
void rebuildIndexes() {
    majorIndex.clear();
    classIndex.clear();
    for (int i = 0; i < (int)gMajors.size(); ++i)
        majorIndex.push_back({gMajors[i].majorName, i});
    std::sort(majorIndex.begin(), majorIndex.end(),
         [](const MIdx& a, const MIdx& b){ return a.key < b.key; });

    classIndex.assign(gMajors.size(), {});
    for (int i = 0; i < (int)gMajors.size(); ++i) {
        auto &ci = classIndex[i];
        for (int j = 0; j < (int)gMajors[i].classes.size(); ++j)
            ci.push_back({gMajors[i].classes[j].classID, j});
        std::sort(ci.begin(), ci.end(),
             [](const CIdx& a, const CIdx& b){ return a.key < b.key; });
    }
}

int findMajorByIndex(const std::string& majorName) {
    auto it = std::lower_bound(majorIndex.begin(), majorIndex.end(), majorName,
        [](const MIdx& a, const std::string& key){ return a.key < key; });
    return (it!=majorIndex.end() && it->key==majorName) ? it->pos : -1;
}

int findClassByIndex(int majorPos, const std::string& classID) {
    if (majorPos < 0 || majorPos >= (int)classIndex.size()) return -1;
    auto &ci = classIndex[majorPos];
    auto it = std::lower_bound(ci.begin(), ci.end(), classID,
        [](const CIdx& a, const std::string& key){ return a.key < key; });
    return (it!=ci.end() && it->key==classID) ? it->pos : -1;
}

/*======================= CSV 持久化 =======================*/
std::string csvEscape(const std::string& s){
    bool need=false;
    for(char c:s){ if(c=='"'||c==','||c=='\n'||c=='\r'){ need=true; break; } }
    if(!need) return s;
    std::string out="\"";
    for(char c:s) out += (c=='"')? "\"\"" : std::string(1,c);
    out += "\"";
    return out;
}

std::vector<std::string> parseCsvLine(const std::string& line){
    std::vector<std::string> f; f.reserve(8);
    std::string cur; bool inq=false;
    for(size_t i=0;i<line.size();++i){
        char c=line[i];
        if(inq){
            if(c=='"'){ if(i+1<line.size() && line[i+1]=='"'){ cur.push_back('"'); ++i; } else inq=false; }
            else cur.push_back(c);
        }else{
            if(c==','){ f.push_back(cur); cur.clear(); }
            else if(c=='"'){ inq=true; }
            else if(c=='\r'){ }
            else cur.push_back(c);
        }
    }
    f.push_back(cur);
    if(!f.empty() && !f[0].empty() && (unsigned char)f[0][0]==0xEF){
        if(f[0].size()>=3 && (unsigned char)f[0][1]==0xBB && (unsigned char)f[0][2]==0xBF)
            f[0]=f[0].substr(3);
    }
    return f;
}

int ensureMajor(const std::string& major){
    for(int i=0;i<(int)gMajors.size();++i) if(gMajors[i].majorName==major) return i;
    gMajors.push_back(MajorRec{major, {}});
    return (int)gMajors.size()-1;
}

int ensureClass(int mi, const std::string& cls){
    auto &cs = gMajors[mi].classes;
    for(int i=0;i<(int)cs.size();++i) if(cs[i].classID==cls) return i;
    cs.push_back(ClassRec{cls, {}});
    return (int)cs.size()-1;
}

bool saveCSV(const std::string& file){
    std::ofstream out(file, std::ios::trunc);
    if(!out) return false;
    out << "kind,major,class,id,name\n";
    for (auto &m : gMajors)
        out << "M," << csvEscape(m.majorName) << ",,,\n";
    for (auto &m : gMajors)
        for (auto &c : m.classes)
            out << "C," << csvEscape(m.majorName) << "," << csvEscape(c.classID) << ",,\n";
    for (auto &m : gMajors)
        for (auto &c : m.classes)
            for (auto &s : c.students)
                out << "S," << csvEscape(m.majorName) << "," << csvEscape(c.classID) << ","
                    << csvEscape(s.id) << "," << csvEscape(s.name) << "\n";
    return true;
}

bool loadCSV(const std::string& file){
    std::ifstream in(file);
    if(!in) return false;
    gMajors.clear();
    std::string line;
    if(!getline(in,line)) return false;
    while(getline(in,line)){
        if(line.empty()) continue;
        auto f = parseCsvLine(line);
        if(f.empty()) continue;
        std::string kind = f[0];
        if(kind=="M"){
            if(f.size()<2) continue;
            ensureMajor(f[1]);
        }else if(kind=="C"){
            if(f.size()<3) continue;
            int mi = ensureMajor(f[1]);
            ensureClass(mi, f[2]);
        }else if(kind=="S"){
            if(f.size()<5) continue;
            std::string major=f[1], cls=f[2];
            Student s; s.id=f[3]; s.name=f[4];
            int mi = ensureMajor(major);
            int ci = ensureClass(mi, cls);
            gMajors[mi].classes[ci].students.push_back(s);
        }
    }
    std::sort(gMajors.begin(), gMajors.end(),
        [](const MajorRec&a, const MajorRec&b){ return a.majorName < b.majorName; });
    for (auto &m : gMajors)
        std::sort(m.classes.begin(), m.classes.end(),
            [](const ClassRec&a, const ClassRec&b){ return a.classID < b.classID; });
    rebuildIndexes();
    return true;
}

/*======================= ASL 计算 =======================*/
static inline double binAvgCost(int n) {
    if (n <= 1) return n;
    return ceil(log2((double)n));
}

double calcASL_strict(){
    long long totalStu = 0;
    for (auto &m : gMajors)
        for (auto &c : m.classes)
            totalStu += (long long)c.students.size();
    if (totalStu == 0) return 0.0;

    double asl = 0.0;
    int M = (int)gMajors.size();
    double costMaj = binAvgCost(M);

    for (auto &m : gMajors) {
        long long stuInMajor = 0;
        for (auto &c : m.classes) stuInMajor += (long long)c.students.size();
        if (stuInMajor == 0) continue;

        double pMajor = (double)stuInMajor / (double)totalStu;
        int C = (int)m.classes.size();
        double costCls = binAvgCost(C);

        double costStu = 0.0;
        for (auto &c : m.classes) {
            int n = (int)c.students.size();
            if (n == 0) continue;
            double pClassInMajor = (double)n / (double)stuInMajor;
            costStu += pClassInMajor * ((n + 1) / 2.0);
        }
        asl += pMajor * (costMaj + costCls + costStu);
    }
    return asl;
}

