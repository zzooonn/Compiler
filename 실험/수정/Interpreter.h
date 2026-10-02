#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <vector>
#include "DataDef.h"
#include <array>
#include <string>

class Interpreter 
{
public:
    Interpreter();
    std::vector<std::string> Inter(const std::vector<std::shared_ptr<PCode>>& mCodelist);

    void WriteToFile(std::string_view filePath);
private:
    std::array<int, 100001> mDStack;
    int BAddr = 0;
    int at = 0;
    int sp = -1;
    int mainBAddr = 0;

    std::vector<std::string> mPrintResult;

};

#endif