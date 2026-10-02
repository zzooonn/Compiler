#include "ErrorHandler.h"
#include <iostream>
#include <algorithm>
#include <fstream>


void ErrorHandler::PushErrorList(char err_type, int err_line) {
    ErrorWord error;
    error.linenum = err_line;
    error.err_type = err_type;
    mErrorList.push_back(error);
}

void ErrorHandler::PrintErrorList(std::string_view filePath)
{
    std::sort(mErrorList.begin(), mErrorList.end(), [](const ErrorWord& a, const ErrorWord& b) {
        if (a.linenum == b.linenum) 
        {
            return a.err_type > b.err_type;
        }
        return a.linenum < b.linenum;
        });

    std::fstream outFile(filePath.data(), std::ios::trunc | std::ios::out);
    if (outFile.good())
    {
        int last_line = -1;
        for (const auto& error : mErrorList) 
        {
            if (error.linenum != last_line)
            {
                outFile << error.linenum << " " << error.err_type;
                std::cout << error.linenum << " " << error.err_type << std::endl;
                last_line = error.linenum;

            }
        }
    }
    outFile.flush();
    outFile.close();
}