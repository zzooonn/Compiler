// ComplierSysY.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <string>
#include "Lexer.h"
#include "Parser.h"
#include "Interpreter.h"

#ifdef _WINDOWS_DEBUG
#include <Windows.h>
#endif

int main()
{
// #ifdef _WINDOWS_DEBUG
//     std::string strModuleDir;
//
//     char szModulePath[512];
//     memset(szModulePath, 0, sizeof(szModulePath));
//     GetModuleFileNameA(NULL, szModulePath, sizeof(szModulePath));
//
//     strModuleDir = szModulePath;
//     strModuleDir = strModuleDir.substr(0, strModuleDir.find_last_of('\\') + 1);
//
//     SetCurrentDirectoryA(strModuleDir.c_str());
// #endif

	// todo 判断文件是否存在
	
	// 词法分析
	auto pLexer = std::make_shared<Lexer>("testfile.txt");
	pLexer->LexicalAnalysis2();
	pLexer->WriteToFile("lexer.txt");
	const std::vector<TokenWord>& tokens = pLexer->GetTokenList();

	// 语法分析
	std::shared_ptr<Parser> pParser = std::make_shared<Parser>(tokens);
	pParser->SyntaxAnalysis();
	pParser->WriteToFile("pcode.txt");
	auto pErrorHandler = pParser->GetErrorHandler();
	if (pErrorHandler && pErrorHandler->HasError())
	{
		pErrorHandler->PrintErrorList("error.txt");
	}

	// 解释执行
	std::shared_ptr<Interpreter> pInterpreter = std::make_shared<Interpreter>();
	pInterpreter->Inter(pParser->GetCodeList());
	pInterpreter->WriteToFile("pcoderesult.txt");

	return 0;
}

