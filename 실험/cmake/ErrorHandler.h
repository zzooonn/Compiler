#ifndef ERRORHANDLER_H
#define ERRORHANDLER_H

#include <vector>
#include <string>

using namespace std;

struct ErrorWord {
    int linenum;
    char err_type;
};

class ErrorHandler 
{
public:
    bool HasError() { return !mErrorList.empty(); };
private:
    vector<ErrorWord> mErrorList;

public:
    void PushErrorList(char err_type, int err_line);
    void PrintErrorList(string_view filePath);
};

#endif