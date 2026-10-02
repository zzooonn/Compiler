#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <vector>
#include "DataDef.h"
#include <array>
#include <string>

using namespace std;

class Interpreter 
{
public:
    Interpreter();
    vector<string> Inter(const vector<shared_ptr<PCode>>& mCodelist);

    void WriteToFile(string_view filePath);
private:
    array<int, 100001> mDStack;
    int BAddr = 0;
    int at = 0;
    int sp = -1;
    int mainBAddr = 0;

    vector<string> mPrintResult;
};

#endif