#include "SymbolTableManager.h"
#include <iostream>
#include <algorithm>

SymbolTableManager::SymbolTableManager() {}

SymbolTable* SymbolTableManager::findSymbol(const std::string& name, int scope) 
{
	for (size_t i = 0; i < symbolTable.size(); ++i)
	{
		if (symbolTable[i].name == name && symbolTable[i].scope == scope)
		{
			return &symbolTable[i];
		}
	}
    return nullptr;
}

void SymbolTableManager::findAndSetSymbol(const std::string& tokenName)
{
	for (const auto& symbol : symbolTable)
	{
		if (symbol.name == tokenName)
		{
			SymbolTemp = symbol;
			break;
		}
	}
}

void SymbolTableManager::add_s_Table(const std::string& name, int type, int kind, int isconst, int mPcodeAddress, bool mIsGlbScope, bool mIsEqualScope, int mGlobalSymbolScope, int mCurScope, int mSaveArrExp)
{
	SymbolTable symbol;
	
	symbol.name = name;
	symbol.type = type;
	symbol.kind = kind;
	symbol.isconst = isconst;
	symbol.pcodearr = mPcodeAddress;
	symbol.arrsize = mSaveArrExp;
	if (mIsGlbScope) {
		symbol.scope = 1;
	}
	else if (mIsEqualScope) {
		symbol.scope = mCurScope;
	}
	else if (mGlobalSymbolScope != mCurScope) {
		symbol.scope = mCurScope;
	}
	else {
		symbol.scope = mGlobalSymbolScope;
	}
	symbolTable.push_back(symbol);
}

//SymbolTable* SymbolTableManager::parseErrorC(const std::string& name) {
//    // 查找符号
//    SymbolTable* symbol = findSymbol(name, mCurScope);
//    if (symbol == nullptr) {
//        // 处理未找到符号的错误
//    }
//    return symbol;
//}

void SymbolTableManager::print_symbol_list() {
    // 打印符号表
    for (const auto& symbol : symbolTable) {
        std::string temp;
        // 根据符号信息确定类型字符串
        std::cout << symbol.scope << " " << symbol.name << " " << temp << std::endl;
    }
}