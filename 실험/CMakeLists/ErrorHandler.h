#ifndef ERRORHANDLER_H
#define ERRORHANDLER_H

#include <vector>
#include <string>

struct ErrorWord {
    int linenum;
    char err_type;
};

class ErrorHandler 
{
public:
    bool HasError() { return !mErrorList.empty(); };
private:
    std::vector<ErrorWord> mErrorList;

public:
    void PushErrorList(char err_type, int err_line);
    void PrintErrorList(std::string_view filePath);
};

#endif