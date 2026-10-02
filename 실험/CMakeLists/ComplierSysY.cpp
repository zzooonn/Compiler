// ComplierSysY.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <string>
#include "Lexer.h"
#include "Parser.h"
#include "Interpreter.h"

using namespace std;

#ifdef _WINDOWS_DEBUG
#include <Windows.h>
#endif

int main()
{
#ifdef _WINDOWS_DEBUG
    std::string strModuleDir;

    char szModulePath[512];
    memset(szModulePath, 0, sizeof(szModulePath));
    GetModuleFileNameA(NULL, szModulePath, sizeof(szModulePath));

    strModuleDir = szModulePath;
    strModuleDir = strModuleDir.substr(0, strModuleDir.find_last_of('\\') + 1);

    SetCurrentDirectoryA(strModuleDir.c_str());
#endif

	// 词法分析
	auto pLexer = make_shared<Lexer>("testfile.txt");
	pLexer->LexicalAnalysis2();
	pLexer->WriteToFile("lexer.txt");
	const vector<TokenWord>& tokens = pLexer->GetTokenList();

	// 语法分析
	shared_ptr<Parser> pParser = make_shared<Parser>(tokens);
	pParser->SyntaxAnalysis();
	pParser->WriteToFile("pcode.txt");
	auto pErrorHandler = pParser->GetErrorHandler();
	if (pErrorHandler && pErrorHandler->HasError())
	{
		pErrorHandler->PrintErrorList("error.txt");
	}

	// 解释执行
	shared_ptr<Interpreter> pInterpreter = make_shared<Interpreter>();
	pInterpreter->Inter(pParser->GetCodeList());
	pInterpreter->WriteToFile("pcoderesult.txt");

	return 0;
}

