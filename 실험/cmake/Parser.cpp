#include "Parser.h"
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
using namespace std;

// Paser为初始化
Parser::Parser(const vector<TokenWord>& tokens) : mTokens(tokens), mCurpos(0) {
	m_pSymbolTableMgr = make_shared<SymbolTableManager>();
	m_pErrorHandler = make_shared<ErrorHandler>();

    if (!mTokens.empty()) {
        mCurToken = mTokens[mCurpos];
    }
}

void Parser::AdvanceToNextToken () 
{
    if (mCurpos < mTokens.size() - 1) 
	{
        mCurpos++;
        mCurToken = mTokens[mCurpos];
    }
}

void Parser::MoveToPreviousToken ()
{
    if (mCurpos > 0) 
	{
        mCurpos--;
        mCurToken = mTokens[mCurpos];
    }
}

void Parser::ParseCompUnit() {

	// 提前定义辅助函数以检查是否是声明或函数定义
	auto isDeclaration = [this]() -> bool {
		return (mTokens[mCurpos].tokenType == EnumTokenType::CONSTTK)
			|| ((mTokens[mCurpos].tokenType == EnumTokenType::INTTK
				|| mTokens[mCurpos].tokenType == EnumTokenType::CHARTK)
				&& mTokens[mCurpos + 1].tokenType == EnumTokenType::IDENFR && mTokens[mCurpos + 2].tokenType != EnumTokenType::LPARENT);
		};

	auto isFunctionDefinition = [this]() -> bool {
		return ((mTokens[mCurpos].tokenType == EnumTokenType::INTTK && mTokens[mCurpos + 1].tokenType == EnumTokenType::IDENFR && mTokens[mCurpos + 2].tokenType == EnumTokenType::LPARENT)
			|| (mTokens[mCurpos].tokenType == EnumTokenType::CHARTK && mTokens[mCurpos + 2].tokenType == EnumTokenType::LPARENT) || (mTokens[mCurpos].tokenType == EnumTokenType::VOIDTK && mTokens[mCurpos + 2].tokenType == EnumTokenType::LPARENT));
		};

	int temp;
	while (isDeclaration()) 
	{
		ParseDecl();
	}

	auto label = make_shared<PCodeLabel>();
	auto code1 = make_shared<PCode>("JMP", label);
	mCodelist.push_back(code1);

	while (isFunctionDefinition())
	{
		ParseFuncDef();
	}
	if (mTokens[mCurpos].tokenType == EnumTokenType::INTTK && mTokens[mCurpos + 1].tokenType == EnumTokenType::MAINTK) {
		temp = mCodelist.size();

		label->SetAddr(temp);

		ParseMainFuncDef();
		return;
	}
}

void Parser::ParseDecl() 
{
	if (mCurToken.tokenType == EnumTokenType::INTTK || mCurToken.tokenType == EnumTokenType::CHARTK)
	{
		ParseVarDecl();
	}
	else if (mCurToken.tokenType == EnumTokenType::CONSTTK)
	{
		ParseConstDecl();
	}
}

void Parser::ParseConstDecl()
{
	int type = -1;
	int kind = 0;			// var = 0 , func = 1 , param = 2, arr = 3

	AdvanceToNextToken();
	if (mCurToken.word == "int") 
	{
		type = INT_TYPE;
	}
	else 
	{
		type = CHAR_TYPE;
	}
	AdvanceToNextToken();

	ParseConstDef(type, kind);
	while (mCurToken.word == ",") 
	{
		AdvanceToNextToken();
		ParseConstDef(type, kind);
	}
	parseErrorI();
}

void Parser::ParseConstDef(int type, int kind)
{
	mLastNonT = mCurToken.lineNum;

	int arrexp = 0;
	
	string identname = mCurToken.word;
	AdvanceToNextToken();

	if (mCurToken.word == "[") 
	{
		AdvanceToNextToken();

		Value value1 = constExpValue();
		arrexp = value1.getValue();

		parseErrorK(); //  ]

		kind = 3;
	}

	parseErrorB(identname, type, kind, 1, mGlobalSymbolScope);
	AdvanceToNextToken();
	ParseConstInitVal(arrexp);

}

void Parser::ParseConstInitVal(int arrexp)
{
	mLastNonT = mCurToken.lineNum;

	int val = 0;

	if (mCurToken.word == "{") {
		AdvanceToNextToken();

		auto code1 = make_shared<PCode>("INT", 1);
		auto code2 = make_shared<PCode>("LOD_A", 0, mPcodeAddress);
		mCodelist.push_back(code1);
		mCodelist.push_back(code2);
		mPcodeAddress++;

		ParseExp();
		auto code3 = make_shared<PCode>("STO");
		mCodelist.push_back(code3);
		arrexp--;
		
		while (mCurToken.word == ",") {
			AdvanceToNextToken();

			auto code1 = make_shared<PCode>("INT", 1);
			auto code2 = make_shared<PCode>("LOD_A", 0, mPcodeAddress);
			mCodelist.push_back(code1);
			mCodelist.push_back(code2);
			mPcodeAddress++;
			ParseExp();
			auto code3 = make_shared<PCode>("STO");
			mCodelist.push_back(code3);
			arrexp--;
		}

		while (arrexp) {
			auto code1 = make_shared<PCode>("INT", 1);
			auto code2 = make_shared<PCode>("LOD_A", mPcodeAddress);
			mCodelist.push_back(code1);
			mCodelist.push_back(code2);

			mPcodeAddress++;
			auto code3 = make_shared<PCode>("LOD_C", 0);
			auto code4 = make_shared<PCode>("STO");
			mCodelist.push_back(code3);
			mCodelist.push_back(code4);
			arrexp--;
		}

		AdvanceToNextToken();
	}

	// 상수문자열 처리 
	else if (mCurToken.tokenType == STRCON) 
	{
		string str = mCurToken.word;

		str.erase(remove(str.begin(), str.end(), '\"'), str.end());

		AdvanceToNextToken();

		for (int i = 0; i < str.length(); i++) {
			auto code1 = make_shared<PCode>("INT", 1);
			mCodelist.push_back(code1);
			auto code2 = make_shared<PCode>("LOD_A", 0, mPcodeAddress);
			mPcodeAddress++;
			mCodelist.push_back(code2);

			val = str[i];
			if (str[i] == '\\') {
				switch (str[i + 1]) {
				case 'n': val = '\n'; i++; break;
				case 't': val = '\t'; i++; break;
				case 'v': val = '\v'; i++; break;
				case '\\': val = '\\'; i++; break;
				case '\'': val = '\''; i++; break;
				case '\"': val = '\"'; i++; break;
				case 'a': val = '\a'; i++; break;
				case 'b': val = '\b'; i++; break;
				case '0': val = '\0'; i++; break;
				case 'f': val = '\f'; i++; break;
				default: val = mCurToken.word[1]; break;
				}
			}

			auto code3 = make_shared<PCode>("LOD_C", val);	//mCurToken.word[i]
			auto code4 = make_shared<PCode>("STO");
			mCodelist.push_back(code3);
			mCodelist.push_back(code4);
			arrexp--;
		}

		while (arrexp--) {
			auto code1 = make_shared<PCode>("INT", 1);
			mCodelist.push_back(code1);
			auto code2 = make_shared<PCode>("LOD_A", 0, mPcodeAddress);
			mPcodeAddress++;
			mCodelist.push_back(code2);
			auto code3 = make_shared<PCode>("LOD_C", "0");	//\0 추가
			auto code4 = make_shared<PCode>("STO");
			mCodelist.push_back(code3);
			mCodelist.push_back(code4);
		}
	}
	//그냥 상수처리 
	else {
		auto code1 = make_shared<PCode>("INT", 1);
		auto code2 = make_shared<PCode>("LOD_A", 0, mPcodeAddress);
		mCodelist.push_back(code1);
		mCodelist.push_back(code2);
		mPcodeAddress++;
		ParseExp();
		auto code3 = make_shared<PCode>("STO");
		mCodelist.push_back(code3);
	}
}

void Parser::ParseVarDecl()
{
	int type = -1;
	int kind = 0;
	if (mCurToken.word == "int") 
	{
		type = INT_TYPE;
	}
	else 
	{		
		type = CHAR_TYPE;
	}

	AdvanceToNextToken();

	ParseVarDef(type, kind);

	while (mCurToken.word == ",") 
	{
		AdvanceToNextToken();

		ParseVarDef(type, kind);
	}

	parseErrorI(); //  ;

}

void Parser::ParseVarDef(int type, int kind)
{
	mLastNonT = mCurToken.lineNum;

	int cnt = 1;

	string identname = mCurToken.word;
	AdvanceToNextToken();
	if (mCurToken.word == "[") {
		AdvanceToNextToken();
		Value value1 = constExpValue();
		cnt = value1.getValue();
		mSaveArrExp = cnt;

		parseErrorK(); //  ]
		kind = ARR;
	}
	parseErrorB(identname, type, kind, 0, mGlobalSymbolScope);
	mSaveArrExp = 0;

	if (mCurToken.word == "=") {
		AdvanceToNextToken();

		ParseInitVal(cnt);
	}
	else {
		for (int i = 0; i < cnt; i++) 
		{
			auto code1 = make_shared<PCode>("INT", 1);
			auto code2 = make_shared<PCode>("LOD_A", 0, mPcodeAddress);
			mPcodeAddress++;
			auto code3 = make_shared<PCode>("LOD_C", 0);	//배열 값초기화 X 0으로 초기화
			auto code4 = make_shared<PCode>("STO");
			mCodelist.push_back(code1);
			mCodelist.push_back(code2);
			mCodelist.push_back(code3);
			mCodelist.push_back(code4);
		}
	}
}

void Parser::ParseInitVal(int arrexp)
{
	int val;

	if (mCurToken.word == "{") {//배열 처리 
		AdvanceToNextToken();

		auto code1 = make_shared<PCode>("INT", 1);
		auto code2 = make_shared<PCode>("LOD_A", 0, mPcodeAddress);
		mCodelist.push_back(code1);
		mCodelist.push_back(code2);
		mPcodeAddress++;
		ParseExp();
		auto code3 = make_shared<PCode>("STO");
		mCodelist.push_back(code3);
		arrexp--;
		while (mCurToken.word == ",") {
			AdvanceToNextToken();

			auto code1 = make_shared<PCode>("INT", 1);
			auto code2 = make_shared<PCode>("LOD_A", mPcodeAddress);
			mCodelist.push_back(code1);
			mCodelist.push_back(code2);
			mPcodeAddress++;
			ParseExp();
			auto code3 = make_shared<PCode>("STO");
			mCodelist.push_back(code3);
			arrexp--;
		}

		while (arrexp) {
			auto code1 = make_shared<PCode>("INT", 1);
			auto code2 = make_shared<PCode>("LOD_A", mPcodeAddress);
			mCodelist.push_back(code1);
			mCodelist.push_back(code2);
			mPcodeAddress++;
			auto code3 = make_shared<PCode>("LOD_C", 0);
			auto code4 = make_shared<PCode>("STO");
			mCodelist.push_back(code3);
			mCodelist.push_back(code4);
			arrexp--;
		}

		AdvanceToNextToken();
	}

	// 상수문자열 처리 
	else if (mCurToken.tokenType == STRCON) 
	{
		string str = mCurToken.word;
		str.erase(remove(str.begin(), str.end(), '\"'), str.end());

		AdvanceToNextToken();

		for (int i = 0; i < str.length(); i++) 
		{
			auto code1 = make_shared<PCode>("INT", 1);
			mCodelist.push_back(code1);
			auto code2 = make_shared<PCode>("LOD_A", 0, mPcodeAddress);
			mPcodeAddress++;
			mCodelist.push_back(code2);

			val = str[i];
			if (str[i] == '\\') {
				switch (str[i + 1]) {
				case 'n': val = '\n'; i++; break;
				case 't': val = '\t'; i++; break;
				case 'v': val = '\v'; i++; break;
				case '\\': val = '\\'; i++; break;
				case '\'': val = '\''; i++; break;
				case '\"': val = '\"'; i++; break;
				case 'a': val = '\a'; i++; break;
				case 'b': val = '\b'; i++; break;
				case '0': val = '\0'; i++; break;
				case 'f': val = '\f'; i++; break;
				default: val = mCurToken.word[1]; break;
				}
			}

			auto code3 = make_shared<PCode>("LOD_C", val);	//mCurToken.word[i]
			auto code4 = make_shared<PCode>("STO");
			mCodelist.push_back(code3);
			mCodelist.push_back(code4);
			arrexp--;
		}

		while (arrexp--) {
			auto code1 = std::make_shared<PCode>("INT", 1);
			mCodelist.push_back(code1);
			auto code2 = std::make_shared<PCode>("LOD_A", 0, mPcodeAddress);
			mPcodeAddress++;
			mCodelist.push_back(code2);
			auto code3 = std::make_shared<PCode>("LOD_C", "0");	//\0 추가	
			auto code4 = std::make_shared<PCode>("STO");
			mCodelist.push_back(code3);
			mCodelist.push_back(code4);
		}
	}
	//그냥 상수처리 
	else {
		// Value value1 = ExpValue();
		// val = value1.getValue();
		auto code1 = std::make_shared<PCode>("INT", 1);
		auto code2 = std::make_shared<PCode>("LOD_A", 0, mPcodeAddress);
		mCodelist.push_back(code1);
		mCodelist.push_back(code2);
		mPcodeAddress++;
		ParseExp();
		auto code3 = std::make_shared<PCode>("STO");
		mCodelist.push_back(code3);
	}
}

//FuncDef → FuncType Ident '(' [FuncFParams] ')' Block // j
void Parser::ParseFuncDef() 
{
	// 初始化函数类型和种类
	int type = -1;  // 用于存储函数的返回类型，初始化为 -1 表示未确定
	int kind = FUNC;  // 函数的种类，这里固定为 FUNC 表示函数
	mIsfuncDef = true;

	// 标记函数定义开始，这里注释掉的代码可能是用于标记函数定义的，暂时不用
	// mIsfuncDef = true; 
	mIsStmtReturn = false;  // 标记函数内是否有 return 语句，初始化为 false
	mIsNeedReturn = true;  // 标记函数是否需要返回值，初始化为 true

	auto pLabel = std::make_shared<PCodeLabel>();
	mPcodeAddress = 3; // 设置中间代码地址为 3
	int startedCode = mCodelist.size();

	// 创建一个标签代码，用于标记函数的开始位置
	auto code = std::make_shared<PCode>("INT_L", pLabel);	//라벨
	mCodelist.push_back(code);

	// 根据当前 Token 的值确定函数的返回类型
	if (mCurToken.word == "int" || mCurToken.word == "char" || mCurToken.word == "void") 
	{
		if (mCurToken.word == "int") {
			type = INT_TYPE;
		}
		else if (mCurToken.word == "char") 
		{
			type = CHAR_TYPE;
		}
		else 
		{
			type = VOID_TYPE;
			mIsNeedReturn = false;
		}
		AdvanceToNextToken();
	}

	// printcurToken();// Ident
	std::string identname = mCurToken.word;
	AdvanceToNextToken();

	// 将函数信息添加到 mFuncInfos 列表中
	mFuncInfos.push_back({});
	mFuncInfos[mFuncInfos.size() - 1].type = type;  // 设置函数的返回类型
	mFuncInfos[mFuncInfos.size() - 1].name = identname;  // 设置函数名
	mFuncInfos[mFuncInfos.size() - 1].startCode = startedCode;  // 设置函数开始的代码位置

	bool valid_func = false;
	if (parseErrorB(identname, type, kind, 0, 1)) 
	{
		valid_func = true;
	}

	mGlobalSymbolScope++;	 // 增加全局符号表范围，用于处理函数的局部变量
	// printcurToken();// (
	mLastNonT = mCurToken.lineNum;
	mCurScope = mGlobalSymbolScope;
	mIsGlbScope = false;
	AdvanceToNextToken();

	if (mCurToken.word != ")" && mCurToken.word != "{")
	{
		mLastNonT = mCurToken.lineNum;
		ParseFuncFParam();
		while (mCurToken.word == ",") {
			AdvanceToNextToken();
			ParseFuncFParam();
		}
	}

	parseErrorJ(); //  )

	mGlobalSymbolScope--;  // 恢复全局符号表范围
	mCurScope = mGlobalSymbolScope;  // 恢复当前作用域
	mFuncStartScope = mGlobalSymbolScope + 1;  // 设置函数开始的作用域
	ParseBlock();  // 解析函数体

	mIsfuncDef = false;
	mCurScope = mGlobalSymbolScope + 1;  // 设置当前作用域为函数开始的作用域
	mIsGlbScope = true;  // 标记当前为全局作用域

	if (valid_func) {
		mFuncInfos.push_back({});
	}
	else
	{
		mFuncInfos.pop_back();
	}

	if (!mIsNeedReturn) 
	{	
		auto code2 = std::make_shared<PCode>("RET");
		mCodelist.push_back(code2);
	}
	pLabel->SetAddr(mPcodeAddress);
}

void Parser::ParseMainFuncDef()
{
	mIsNeedReturn = true;
	mIsStmtReturn = false;

	auto pLabel = std::make_shared<PCodeLabel>();
	mPcodeAddress = 0;
	auto code = std::make_shared<PCode>("INT_L", pLabel);
	mCodelist.push_back(code);

	AdvanceToNextToken();

	AdvanceToNextToken();

	mLastNonT = mCurToken.lineNum;
	AdvanceToNextToken();

	parseErrorJ();

	mIsGlbScope = false;
	mCurScope = mGlobalSymbolScope;
	mFuncStartScope = mGlobalSymbolScope + 1;

	ParseBlock();

	mCodelist.pop_back();
	auto code1 = std::make_shared<PCode>("RET_TO_END");
	mCodelist.push_back(code1);

	mIsGlbScope = true;

	pLabel->SetAddr(mPcodeAddress);
}

void Parser::ParseFuncFParam()
{
	int type = -1;
	if (mCurToken.word == "int")
	{
		type = INT_TYPE;
	}
	else 
	{
		type = CHAR_TYPE;
	}
	AdvanceToNextToken();
	std::string identname = mCurToken.word;
	AdvanceToNextToken();

	int kind = VAR;
	if (mCurToken.word == "[") 
	{
		mLastNonT = mCurToken.lineNum;
		AdvanceToNextToken();
		parseErrorK();

		kind = ARR;
	}
	parseErrorB(identname, type, kind, 0, mGlobalSymbolScope);
	mFuncInfos[mFuncInfos.size() - 1].paramtype.push_back(type);
	mFuncInfos[mFuncInfos.size() - 1].paramkind.push_back(kind); 
	mFuncInfos[mFuncInfos.size() - 1].paramcnt++;

	mPcodeAddress++;
}

void Parser::ParseBlock()
{
	mGlobalSymbolScope++;
	mCurScope = mGlobalSymbolScope;
	mScopeStack.push(mCurScope); // 새로운 스코프를 스택에 추가


	if (mFuncStartScope == mCurScope) {
		mIsEqualScope = true;
	}
	else {
		mIsEqualScope = false;
	}
	AdvanceToNextToken();

	while (mCurToken.word != "}") 
	{
		//Decl
		if (mCurToken.tokenType == INTTK || mCurToken.tokenType == CHARTK || mCurToken.tokenType == CONSTTK)
		{
			ParseDecl();
		}
		//Stmt
		else {
			parseStmt();
		}
	}

	if (mFuncStartScope == mCurScope) 
	{
		parseErrorG();
	}

	// printcurToken();//	}
	mIsStmtReturn = false;

	if (!mScopeStack.empty()) {
		mScopeStack.pop();
		if (!mScopeStack.empty()) {
			mCurScope = mScopeStack.top();
		}
	}

	if (mFuncStartScope == mCurScope) {	//블럭 탈출할시 다시한번 비교 
		mIsEqualScope = true;
	}

	AdvanceToNextToken();
}

void Parser::parseStmt()
{	
	// 'if' '(' Cond ')' Stmt [ 'else' Stmt ] 
	auto handleIfStatement = [&]() {

		// 标记当前处于 if-else 语句块中
		mIsIfElse = true;
		// 创建两个标签，用于条件跳转
		auto label1 = std::make_shared<PCodeLabel>();
		auto label2 = std::make_shared<PCodeLabel>();

		// 移动到下一个 Token
		AdvanceToNextToken();
		AdvanceToNextToken();

		// bool hasVarDeclaration = false;
		// std::string varName;
		// int varType = -1;
	
		// // C++17 스타일 if 문: if(int a = 0; a < 10) { ... }
		// if (mCurToken.tokenType == INTTK || mCurToken.tokenType == CHARTK) {
		// 	hasVarDeclaration = true;
	
		// 	// 변수 타입 저장 (INT_TYPE, CHAR_TYPE)
		// 	varType = (mCurToken.tokenType == INTTK) ? INT_TYPE : CHAR_TYPE;
	
		// 	// 변수 이름 저장
		// 	AdvanceToNextToken();
		// 	varName = mCurToken.word;
	
		// 	// 변수 추가 (심볼 테이블에 저장)
		// 	parseErrorB(varName, varType, VAR, 0, mCurScope);
	
		// 	// '=' 처리
		// 	AdvanceToNextToken();
		// 	if (mCurToken.word == "=") {
		// 		AdvanceToNextToken();
		// 		ParseExp(); // 변수 초기화 값 처리
		// 	}
	
		// 	// 세미콜론(;) 확인
		// 	parseErrorJ();
		// }
		
		// 解析条件表达式
		ParseCond();
		// 生成条件跳转指令，如果条件为假则跳转到 label1
		auto code1 = std::make_shared<PCode>("BZT", label1);

		// 将生成的指令添加到代码列表中
		mCodelist.push_back(code1);

		// 处理条件判断后的错误检查
		parseErrorJ();

		// 递归解析 if 语句块中的语句
		parseStmt();

		if (mCurToken.word == "else") {
			AdvanceToNextToken();
			// 生成无条件跳转指令，跳转到 label2
			auto code2 = std::make_shared<PCode>("J", label2);
			// 将生成的指令添加到代码列表中
			mCodelist.push_back(code2);
			// 设置 label1 的跳转地址为当前代码列表的大小
			label1->SetAddr(mCodelist.size());
			// 递归解析 else 语句块中的语句
			parseStmt();
			// 设置 label2 的跳转地址为当前代码列表的大小
			label2->SetAddr(mCodelist.size());
		}
		else 
		{
			// 如果没有 else 语句，设置 label1 的跳转地址为当前代码列表的大小
			label1->SetAddr(mCodelist.size());
		}

		mIsIfElse = false;
	};

	// 'break' ';' | 'continue' ';' 
	auto handleBreakOrContinue = [&]() {
		// 记录当前非终结符的行号，用于错误处理等
		mLastNonT = mCurToken.lineNum;

		if (mCurToken.word == "break" && mIsFor > 0) {
			// 生成无条件跳转指令，跳转到 for 循环的 break 标签
			auto code1 = std::make_shared<PCode>("J", mForbreak.back());
			// 将生成的指令添加到代码列表中
			mCodelist.push_back(code1);
		}

		if (mCurToken.word == "continue" && mIsFor > 0) {
			// 生成无条件跳转指令，跳转到 for 循环的 continue 标签
			auto code2 = std::make_shared<PCode>("J", mForcontinue.back());
			// 将生成的指令添加到代码列表中
			mCodelist.push_back(code2);
			mIsforincontinue = true;
		}

		// 处理 break 或 continue 语句的错误检查
		parseErrorM();

		// 移动到下一个 Token
		AdvanceToNextToken();

		// 处理后续的错误检查
		parseErrorI();
	};

	// 'return' [Exp] ';'
	auto handleReturn = [&]() {
		// 记录当前非终结符的行号，用于错误处理等
		mLastNonT = mCurToken.lineNum;
		// 记录 return 语句所在的行号
		int returnline = mCurToken.lineNum;

		// 用于存储可能的返回值信息
		std::string justprint = "";

		// 如果当前不在 if-else 语句块中，标记语句有返回值
		if (!mIsIfElse)
		{
			mIsStmtReturn = true;
		}

		AdvanceToNextToken();

		// 如果函数需要返回值
		if (mIsNeedReturn) {
			// 生成加载地址指令
			auto code1 = std::make_shared<PCode>("LOD_A", 0, 0);
			// 将生成的指令添加到代码列表中
			mCodelist.push_back(code1);
		}

		// 判断当前 Token 是否为表达式的起始
		if (isdigit(mCurToken.word[0]) || mCurToken.token == "CHRCON" ||
			mCurToken.word == "(" || mCurToken.token == "IDENFR" || mCurToken.token == "MINU"
			|| mCurToken.token == "PLUS") {
			// 处理 return 语句的错误检查
			parseErrorF(returnline);
			// 记录当前 Token 的信息
			justprint = mCurToken.word;
			// 解析表达式
			ParseExp();
			// 处理后续的错误检查
			parseErrorI();
		}
		else 
		{
			// 处理后续的错误检查
			parseErrorI();
		}

		// 如果函数需要返回值
		if (mIsNeedReturn) {
			// 生成存储指令
			auto code2 = std::make_shared<PCode>("STO");
			// 将生成的指令添加到代码列表中
			mCodelist.push_back(code2);
		}
		// 生成返回指令
		auto code3 = std::make_shared<PCode>("RET", justprint);
		// 将生成的指令添加到代码列表中
		mCodelist.push_back(code3);
	};

	// 'printf''('StringConst {','Exp}')'';'
	auto handlePrintf = [&]() {
		int printlinenum = mCurToken.lineNum;
		AdvanceToNextToken();

		AdvanceToNextToken();

		std::string temp = mCurToken.word;
		mLastNonT = mCurToken.lineNum;
		AdvanceToNextToken();

		int expcnt = 0;
		while (mCurToken.word == ",") 
		{
			AdvanceToNextToken();
			ParseExp();
			expcnt++;
		}

		auto code1 = std::make_shared<PCode>("PRF", temp);
		// 将生成的指令添加到代码列表中
		mCodelist.push_back(code1);

		// 处理错误检查
		parseErrorJ();
		parseErrorI();
		parseErrorL(temp, expcnt, printlinenum);
	};

	// 记录当前非终结符的行号，用于错误处理等
	mLastNonT = mCurToken.lineNum;

	// 处理 if 语句：'if' '(' Cond ')' Stmt [ 'else' Stmt ]
	if (mCurToken.word == "if") 
	{
		handleIfStatement();
	}
	//'for' '(' [ForStmt] ';' [Cond] ';' [ForStmt] ')' Stmt 
	else if (mCurToken.word == "for") {
		// 调用解析 for 循环的函数
		parsefor();
	}
	// 处理 break 或 continue 语句：'break' ';' | 'continue' ';'
	else if (mCurToken.word == "break" || mCurToken.word == "continue") {
		handleBreakOrContinue();
	}
	// 处理 return 语句：'return' [Exp] ';'
	else if (mCurToken.word == "return") 
	{
		handleReturn();
	}
	//'printf''('StringConst {','Exp}')'';' // i j
	else if (mCurToken.word == "printf") {
		handlePrintf();
	}
	// 处理代码块：'{' ... '}'
	else if (mCurToken.word == "{") {
		// 调用解析代码块的函数
		ParseBlock();
	}
	// 处理其他情况，可能是 LVal 或表达式
	else {
		// 先判断是否为 LVal
		int index = 0;
		int isLVal = 0;
		while (true)
		{
			// 如果遇到分号，跳出循环
			if (mCurToken.word == ";")
				break;
			// 如果遇到等号，标记为 LVal
			if (mCurToken.word == "=")
			{
				isLVal = 1;
				break;
			}
			index++;

			// 如果当前位置超过 Token 列表的栈顶位置，跳出循环
			if (mCurpos >= mTokens.size()) {
				break;
			}
			else {
				AdvanceToNextToken();
			}
		}
		// 回到 LVal 的位置
		while (index) {
			// 移动到上一个 Token
			MoveToPreviousToken ();
			index--;
		}
		// 处理 LVal 赋值语句
		if (isLVal) {
			parseErrorH(mCurToken.word);
			int checkchar = ParseLVal(-1);

			if (checkchar == -2) {
				while (true)
				{
					if (mCurToken.word == ";")
						break;
					if (mCurpos >= mTokens.size()) {
						break;
					}
					else {
						AdvanceToNextToken();
					}
				}
			}
			if (mCurToken.word == "=") {
				AdvanceToNextToken();
			}

			if (mCurToken.word == "getint") {
				auto code1 = std::make_shared<PCode>("GET");
				mCodelist.push_back(code1);

				AdvanceToNextToken();

				mLastNonT = mCurToken.lineNum;
				AdvanceToNextToken();

				parseErrorJ();

				parseErrorI();
			}
			else if (mCurToken.word == "getchar") {
				auto code1 = std::make_shared<PCode>("GETC");
				mCodelist.push_back(code1);

				AdvanceToNextToken();

				// 记录当前非终结符的行号，用于错误处理等
				mLastNonT = mCurToken.lineNum;
				AdvanceToNextToken();

				parseErrorJ();

				parseErrorI();
			}
			// LVal '=' Exp ';' // i
			else {
				if (checkchar != -2) {
					ParseExp();
				}
				parseErrorI();
			}
			if (checkchar == 1) {
				auto code3 = std::make_shared<PCode>("MOD128");
				mCodelist.push_back(code3);
			}
			// 生成存储指令
			auto code2 = std::make_shared<PCode>("STO");
			// 将生成的指令添加到代码列表中
			mCodelist.push_back(code2);
		}
		//[Exp] ';' // i
		else {
			if (mCurToken.tokenType == IDENFR) {
				SymbolTable* symboltemp = parseErrorC(mCurToken.word);
				if (symboltemp == nullptr) {
					while (true)
					{
						if (mCurToken.word == ";")
							break;
						if (mCurpos >= mTokens.size()) {
							break;
						}
						else {
							AdvanceToNextToken();
						}
					}
				}
			}
			// 如果当前 Token 不是分号，解析表达式
			if (mCurToken.word != ";") {
				ParseExp();
			}
			parseErrorI();
		}
	}
}

void Parser::ParseForStmt()
{
	parseErrorH(mCurToken.word);	//cosnt값 변환 에러 처리 

	ParseLVal(-1);

	AdvanceToNextToken();
	ParseExp();

	auto code = std::make_shared<PCode>("STO");
	mCodelist.push_back(code);
}

int Parser::ParseExp()
{
	int type = INT_TYPE;
	mLastNonT = mCurToken.lineNum;

	type = ParseAddExp(type);

	return type;
}

void Parser::ParseCond()
{
	mLastNonT = mCurToken.lineNum;

	parseLOrExp();
}

int Parser::ParseLVal(int type)
{
	std::string temp = mCurToken.word;
	parseErrorC(mCurToken.word);

	SymbolTable* symboltemp = parseErrorC(temp);
	if (symboltemp == nullptr) {
		return -2;
	}

	int addr = symboltemp->pcodearr;
	int global = (symboltemp->scope == 1) ? 1 : 0;
	int	isarrtype = symboltemp->kind;//3이면 배열
	int thisarrsize = symboltemp->arrsize - 1;

	AdvanceToNextToken();

	bool isarr = false;

	if (mCurToken.word == "[") {
		isarr = true;

		mLastNonT = mCurToken.lineNum;
		AdvanceToNextToken();

		type = ParseExp();

		parseErrorK();
	}

	if (isarrtype == 3 && !isarr) {
		auto code = std::make_shared<PCode>("LOD", global, addr);
		mCodelist.push_back(code);
		pcodeislod = true;
	}
	else if (mIsfuncDef && isarr) {
		if (global == 0) {
			global = 2;
		}
		auto code = std::make_shared<PCode>("LOD_A", global, addr);
		mCodelist.push_back(code);
		auto code2 = std::make_shared<PCode>("ADD");
		mCodelist.push_back(code2);
	}
	else if (!isarr) {
		auto code = std::make_shared<PCode>("LOD_A", global, addr);
		mCodelist.push_back(code);
	}
	else {
		auto code1 = std::make_shared<PCode>("LOD_A", global, addr);
		mCodelist.push_back(code1);
		auto code2 = std::make_shared<PCode>("ADD");
		mCodelist.push_back(code2);
	}

	type = symboltemp->type;

	return type;
}

int Parser::ParsePrimaryExp(int type)
{
	if (mCurToken.word == "(")
	{//exp
		AdvanceToNextToken();

		type = ParseExp();

		parseErrorJ();
	}
	else if (isdigit(mCurToken.word[0])) 
	{//Number
		auto code1 = std::make_shared<PCode>("LOD_C", stoi(mCurToken.word));
		mCodelist.push_back(code1);

		type = INT_TYPE;
		AdvanceToNextToken();
	}
	else if (mCurToken.token == "IDENFR") {
		type = ParseLVal(type);
		if (!pcodeislod) {
			auto code1 = std::make_shared<PCode>("LODS");
			mCodelist.push_back(code1);
		}
		pcodeislod = false;
	}
	else {//Character
		int val = mCurToken.word[1];
		if (mCurToken.word[1] == '\\') {
			switch (mCurToken.word[2]) {
			case 'n': val = '\n'; break;
			case 't': val = '\t'; break;
			case 'v': val = '\v'; break;
			case '\\': val = '\\'; break;
			case '\'': val = '\''; break;
			case '\"': val = '\"'; break;
			case 'a': val = '\a'; break;
			case 'b': val = '\b'; break;
			case '0': val = '\0'; break;
			case 'f': val = '\f'; break;
			default: val = mCurToken.word[1]; break; // 알 수 없는 경우 기본값 설정
			}
		}
		auto code1 = std::make_shared<PCode>("LOD_C", val);
		mCodelist.push_back(code1);

		type = CHAR_TYPE;
		AdvanceToNextToken();
	}
	//printSyntax("PrimaryExp");
	return type;
}

int Parser::ParseUnaryExp(int type)
{
	AdvanceToNextToken();
	// 保存当前令牌的单词，用于后续判断是否为函数调用情况（如标识符后面跟括号）
	std::string temp1 = mCurToken.word;	
	// 再将当前令牌位置移回上一个，以便后续处理
	MoveToPreviousToken ();
	std::string op;

	// 如果当前令牌是一元运算符（+、-、!）
	if (mCurToken.word == "+" || mCurToken.word == "-" || mCurToken.word == "!") {
		op = mCurToken.word;

		AdvanceToNextToken();

		// 递归调用 ParseUnaryExp 解析后续的表达式
		type = ParseUnaryExp(type);

		if (op == "-") {
			auto code1 = std::make_shared<PCode>("MINU");
			mCodelist.push_back(code1);
		}
		else if (op == "!") {
			auto code1 = std::make_shared<PCode>("NOT");
			mCodelist.push_back(code1);
		}

	}
	// 如果当前令牌是标识符且下一个令牌是左括号，可能是函数调用
	else if (mCurToken.token == "IDENFR" && temp1 == "(") {
		parseErrorC(mCurToken.word);

		// 记录当前令牌所在的行号，分别更新 mLastNonT 和 mLastFuncLine
		mLastNonT = mCurToken.lineNum;
		mLastFuncLine = mCurToken.lineNum;
		// 保存当前标识符的名称，用于后续查找函数定义
		std::string identname = mCurToken.word;
		int paramcnt = 0;

		// 创建一个表示 INT 操作的 PCode 对象，参数为 3
		auto code1 = std::make_shared<PCode>("INT", 3);
		// 将该 PCode 对象添加到代码列表中
		mCodelist.push_back(code1);

		// 标记是否找到对应的函数定义
		bool isFound = false;
		// 遍历函数信息列表，查找与当前标识符名称匹配的函数定义
		for (int i = 0; i <= mFuncInfos.size(); i++) { 
			if (mFuncInfos[i].name == identname) {
				mFuncparse.push_back(mFuncInfos[i]);
				isFound = true;
				break;
			}
		}

		if (!isFound) {
			AdvanceToNextToken();
			// 跳过直到遇到右括号或分号
			while (mCurToken.word != ")" && mCurToken.word != ";") { 
				// printcurToken();
				AdvanceToNextToken();
				// 如果到达文件末尾，跳出循环
				if (mCurpos >= mTokens.size())
					break; 
			}
			// 如果当前令牌是右括号
			if (mCurToken.word == ")") {
				// 移动到下一个令牌，处理分号
				AdvanceToNextToken();  // ';' 처리 
			}

			// 返回当前类型，结束函数调用处理
			return type; 
		}

		// 获取找到的函数的类型
		type = mFuncparse.back().type;

		// 移动到下一个令牌（跳过标识符）
		AdvanceToNextToken();

		// 再移动到下一个令牌（跳过左括号）
		AdvanceToNextToken();

		// 如果当前令牌不是右括号
		if (mCurToken.word != ")") {
			// 如果当前令牌不是分号，解析函数的实际参数列表
			if (mCurToken.word != ";") {
				paramcnt = ParseFuncRParams();
			}
		}

		// 创建一个表示 DOWN 操作的 PCode 对象，参数为 3 + 参数数量
		auto code2 = std::make_shared<PCode>("DOWN", 3 + paramcnt);
		// 将该 PCode 对象添加到代码列表中
		mCodelist.push_back(code2);
		// 创建一个表示 CAL 操作的 PCode 对象，参数为函数的起始代码地址
		auto code3 = std::make_shared<PCode>("CAL", mFuncparse.back().startCode);
		mCodelist.push_back(code3);

		if (!parseErrorJ()) {
			if (mFuncparse.back().expectedcnt != mFuncparse.back().paramcnt) {
				parseErrorD(mLastFuncLine);
			}
		}

		mFuncparse.pop_back();
	}
	else 
	{
		type = ParsePrimaryExp(type);
	}

	return type;
}

int Parser::ParseFuncRParams()
{
	int paramscnt = 0;

	parseErrorE(paramscnt + 1);	 
	int type = ParseExp();

	paramscnt++;
	mFuncparse.back().expectedcnt++;
	while (mCurToken.word == ",") 
	{
		AdvanceToNextToken();

		parseErrorE(paramscnt + 1);
		type = ParseExp();

		paramscnt++;
		mFuncparse.back().expectedcnt++;
	}

	return paramscnt;
}

int Parser::ParseMulExp(int type)
{
	ParseUnaryExp(type); //UnaryExp+ 의 형태 

	//printSyntax("MulExp");

	while (mCurToken.word == "*" || mCurToken.word == "/" || mCurToken.word == "%") {
		std::string op = mCurToken.word;

		AdvanceToNextToken();

		type = ParseUnaryExp(type);

		if (op == "*")
		{
			auto code1 = std::make_shared<PCode>("MUL");
			mCodelist.push_back(code1);
		}
		else if (op == "/")
		{
			auto code2 = std::make_shared<PCode>("DIV");
			mCodelist.push_back(code2);
		}
		else if (op == "%")
		{
			auto code3 = std::make_shared<PCode>("MOD");
			mCodelist.push_back(code3);
		}
		//printSyntax("MulExp");
	}
	return type;
}

int Parser::ParseAddExp(int type)
{
	ParseMulExp(type);

	while (mCurToken.word == "+" || mCurToken.word == "-") {
		std::string op = mCurToken.word;

		AdvanceToNextToken();

		type = ParseMulExp(type);

		if (op == "+") {
			auto code1 = std::make_shared<PCode>("ADD");
			mCodelist.push_back(code1);
		}
		else if (op == "-") {
			auto code2 = std::make_shared<PCode>("SUB");
			mCodelist.push_back(code2);
		}
	}
	return type;
}

void Parser::ParseRelExp()
{
	ParseAddExp(-1);

	if (mCurToken.word == "<" || mCurToken.word == ">" || mCurToken.word == "<=" || mCurToken.word == ">=") {
		std::string op = mCurToken.word;

		AdvanceToNextToken();

		ParseRelExp();

		if (op == ">") {
			auto code1 = std::make_shared<PCode>("BGT");
			mCodelist.push_back(code1);
		}
		else if (op == "<") {
			auto code2 = std::make_shared<PCode>("BLT");
			mCodelist.push_back(code2);
		}
		else if (op == "<=") {
			auto code3 = std::make_shared<PCode>("BLE");
			mCodelist.push_back(code3);
		}
		else if (op == ">=") {
			auto code4 = std::make_shared<PCode>("BGE");
			mCodelist.push_back(code4);
		}
	}
}

void Parser::ParseEqExp()
{
	ParseRelExp();

	if (mCurToken.word == "==" || mCurToken.word == "!=") {
		std::string op = mCurToken.word;

		// printcurToken();  // == | !=
		AdvanceToNextToken();

		ParseEqExp();

		if (op == "==") {
			auto code1 = std::make_shared<PCode>("BEQ");
			mCodelist.push_back(code1);
		}
		else if (op == "!=") {
			auto code2 = std::make_shared<PCode>("BNE");
			mCodelist.push_back(code2);
		}
	}
}

void Parser::parseLAndExp()
{
	ParseEqExp();
	parseErrorA();

	if (mCurToken.token == "AND") {
		// printcurToken();  // &&
		AdvanceToNextToken();

		auto pLabel = std::make_shared<PCodeLabel>();
		auto code1 = std::make_shared<PCode>("JP0", pLabel);
		mCodelist.push_back(code1);
		auto code2 = std::make_shared<PCode>("DOWN", 1);
		mCodelist.push_back(code2);

		parseLAndExp();

		pLabel->SetAddr(mCodelist.size());
	}
}

void Parser::parseLOrExp()
{
	parseLAndExp();
	parseErrorA();

	if (mCurToken.token == "OR") 
	{
		AdvanceToNextToken();

		auto pLabel = std::make_shared<PCodeLabel>();

		auto code1 = std::make_shared<PCode>("JP1", pLabel);
		mCodelist.push_back(code1);
		auto code2 = std::make_shared<PCode>("DOWN", 1);
		mCodelist.push_back(code2);

		parseLOrExp();

		pLabel->SetAddr(mCodelist.size());
	}
}

void Parser::parseConstExp()
{
	mLastNonT = mCurToken.lineNum;

	ParseAddExp(-1);
}

void Parser::SyntaxAnalysis() {
	mScopeStack.push(mCurScope);

	mCurpos = 0;
	mCurToken = mTokens[mCurpos];
    ParseCompUnit();
}

void Parser::WriteToFile(std::string_view filePath)
{
	std::fstream outFile(filePath.data(), std::ios::trunc | std::ios::out);
	if (outFile.good())
	{
		for (auto& code : mCodelist)
		{
			outFile << "opcade:" << " " << code->GetName() << " scope:" << code->GetScope() << " print:" << code->GetPrint() << std::endl;
		}
	}
	outFile.flush();
	outFile.close();
}

Value Parser::constExpValue()
{
	Value* value = new Value(0);
	int val = 0;

	Value value1 = AddExpValue();
	val = value1.getValue();

	value->setValue(val);

	return *value;
}

Value Parser::AddExpValue()
{
	Value* value = new Value(0);
	int val = 0;
	int nextval;

	Value value1 = MulExpValue();
	val = value1.getValue();

	while (mCurToken.word == "+" || mCurToken.word == "-") {
		std::string op = mCurToken.word;

		AdvanceToNextToken();

		Value value2 = MulExpValue();
		nextval = value2.getValue();


		if (op == "+") {
			val = val + nextval;
		}
		else if (op == "-") {
			val = val - nextval;
		}
	}

	value->setValue(val);

	return *value;
}

Value Parser::MulExpValue()
{
	Value* value = new Value(0);
	int val = 0;
	int nextval;

	Value value1 = UnaryExpValue();
	val = value1.getValue();

	while (mCurToken.word == "*" || mCurToken.word == "/" || mCurToken.word == "%") {
		std::string op = mCurToken.word;

		AdvanceToNextToken();

		Value value1 = UnaryExpValue();
		nextval = value1.getValue();

		if (op == "*") {
			val *= nextval;
		}
		else if (op == "/") {
			if (nextval != 0) { // 0으로 나누기 방지
				val /= nextval;
			}
			else {
				//cout << "Error: Division by zero in MulExpValue!" << endl;
				delete value;
				return *new Value(0); // 기본값 반환
			}
		}
		else if (op == "%") {
			if (nextval != 0) { // 0으로 나머지 연산 방지
				val %= nextval;
			}
			else {
				//cout << "Error: Modulus by zero in MulExpValue!" << endl;
				delete value;
				return *new Value(0); // 기본값 반환
			}
		}
	}

	value->setValue(val);

	return *value;
}

Value Parser::UnaryExpValue()
{
	Value* value = new Value(0);
	AdvanceToNextToken();
	std::string temp1 = mCurToken.word;	//ident일때(받기 
	MoveToPreviousToken ();
	int val = 0;
	int nextval;

	if (mCurToken.word == "+" || mCurToken.word == "-" || mCurToken.word == "!") {
		std::string op = mCurToken.word;
		AdvanceToNextToken();

		Value value1 = UnaryExpValue();
		nextval = value1.getValue();

		if (op == "+") {
			val = nextval;
		}
		else if (op == "-") {
			val = nextval * -1;
		}
	}
	else if (mCurToken.token == "IDENFR" && temp1 == "(") {
		parseErrorC(mCurToken.word);

		// printcurToken();  // ident
		mLastNonT = mCurToken.lineNum;
		mLastFuncLine = mCurToken.lineNum;
		std::string identname = mCurToken.word;

		bool isFound = false;
		for (int i = 0; i <= mFuncInfos.size(); i++) {  // 함수 정의 목록에서 매칭 검사
			if (mFuncInfos[i].name == identname) {
				mFuncparse.push_back(mFuncInfos[i]);
				isFound = true;
				break;
			}
		}

		if (!isFound) {
			// 정의되지 않은 함수일 경우
			// printcurToken(); // 'ident'
			AdvanceToNextToken();
			while (mCurToken.word != ")" && mCurToken.word != ";") {  // ')' 또는 ';'까지 건너뜀
				// printcurToken();
				AdvanceToNextToken();
				if (mCurpos >= mTokens.size()) break;  // 파일 끝에 도달했으면 탈출
			}
			if (mCurToken.word == ")") {
				// printcurToken();
				AdvanceToNextToken();  // ';' 처리 
			}
			//printSyntax("UnaryExp");
			return *value;  // 함수 호출 처리 종료
		}
		AdvanceToNextToken();

		// printcurToken();  //(
		AdvanceToNextToken();

		if (mCurToken.word != ")") {
			if (mCurToken.word != ";") {// 그냥 )parse err 
				ParseFuncRParams();
			}
		}

		if (!parseErrorJ()) {		//에러j가 우선순위를 가짐 
			if (mFuncparse.back().expectedcnt != mFuncparse.back().paramcnt) {
				parseErrorD(mLastFuncLine);
			}
		}

		mFuncparse.pop_back();
	}
	else {
		Value value2 = PrimaryExpValue();
		val = value2.getValue();
	}
	//printSyntax("UnaryExp");

	value->setValue(val);

	return *value;
}

Value Parser::PrimaryExpValue()
{
	Value* value = new Value(0);
	int val = 0;
	int nextval;
	if (mCurToken.word == "(") {//exp
		// printcurToken();  // (
		AdvanceToNextToken();

		Value value1 = ExpValue();
		val = value1.getValue();
		value->setValue(val);

		parseErrorJ();
	}
	else if (isdigit(mCurToken.word[0])) {//Number
		val = stoi(mCurToken.word);

		AdvanceToNextToken();
	}
	else if (mCurToken.token == "IDENFR") {
		Value value1 = LValValue();
		val = value1.getValue();
	}
	else {//Character
		val = mCurToken.word[1];
		if (mCurToken.word[1] == '\\') {
			switch (mCurToken.word[2]) {
			case 'n': val = '\n'; break;
			case 't': val = '\t'; break;
			case 'v': val = '\v'; break;
			case '\\': val = '\\'; break;
			case '\'': val = '\''; break;
			case '\"': val = '\"'; break;
			case 'a': val = '\a'; break;
			case 'b': val = '\b'; break;
			case '0': val = '\0'; break;
			case 'f': val = '\f'; break;
			default: val = mCurToken.word[1]; break; // 알 수 없는 경우 기본값 설정
			}
		}
		AdvanceToNextToken();
	}

	//printSyntax("PrimaryExp");

	value->setValue(val);

	return *value;
}

Value Parser::ExpValue()
{
	Value* value = new Value(0);
	int val = 0;

	Value value1 = AddExpValue();
	val = value1.getValue();

	//printSyntax("Exp");

	value->setValue(val);

	return *value;
}

Value Parser::LValValue()
{
	Value* value = new Value(0);
	int val = 0;
	int nextval;

	// printcurToken();
	AdvanceToNextToken();

	if (mCurToken.word == "[") {
		// printcurToken();
		AdvanceToNextToken();

		Value value1 = ExpValue();
		val = value1.getValue();

		parseErrorK();
	}
	value->setValue(val);

	//printSyntax("LVal");

	return *value;
}

void Parser::parsefor()
{
	isBorC = true;		// 显示进入for语句

	auto endforlabel = std::make_shared<PCodeLabel>();
	auto returnforlabel = std::make_shared<PCodeLabel>();
	auto startstmtlabel = std::make_shared<PCodeLabel>();
	auto returnthree = std::make_shared<PCodeLabel>();

	bool NotCalledCond = true;
	bool isthree = false;
	mIsFor++;

	mForbreak.push_back(endforlabel);
	mForcontinue.push_back(returnforlabel);

	// for token和 “(” 消费
	AdvanceToNextToken();
	AdvanceToNextToken();

	if (mCurToken.word != ";") {
		if (mCurToken.word == "int" || mCurToken.word == "char") {
            int type = -1;
            int kind = 0;
            if (mCurToken.word == "int")
                type = INT_TYPE;
            else
                type = CHAR_TYPE;
            // 자료형 토큰 소비 (예: "int" 또는 "char")
            AdvanceToNextToken();
            // 변수 정의 처리 (ParseVarDef는 기존에 선언된 변수를 처리하는 함수)
            ParseVarDef(type, kind);
            // 쉼표(,)로 연결된 추가 변수 선언 처리
            while (mCurToken.word == ",") {
                AdvanceToNextToken();
                ParseVarDef(type, kind);
            }
        }
        else {
            // 변수 선언이 아니라 일반 식일 경우 기존 로직대로 처리
            ParseForStmt();
        }
	}


	AdvanceToNextToken();

	returnforlabel->SetAddr(mCodelist.size());

	if (mCurToken.word != ";") {
		NotCalledCond = false;

		ParseCond();

		auto codebreak = std::make_shared<PCode>("BZT", endforlabel);
		mCodelist.push_back(codebreak);
	}

	AdvanceToNextToken();

	returnthree->SetAddr(mCodelist.size() + 1);

	if (mCurToken.word != ")") {

		mForcontinue.pop_back();
		mForcontinue.push_back(returnthree);

		isthree = true;
		auto codethree = std::make_shared<PCode>("J", startstmtlabel);
		mCodelist.push_back(codethree);

		ParseForStmt();

		auto codethree2 = std::make_shared<PCode>("J", returnforlabel);
		mCodelist.push_back(codethree2);
	}

	AdvanceToNextToken();

	startstmtlabel->SetAddr(mCodelist.size());
	parseStmt();

	if (isthree) {
		auto codetothree = std::make_shared<PCode>("J", returnthree);
		mCodelist.push_back(codetothree);
	}

	auto codereturnfor = std::make_shared<PCode>("J", returnforlabel); // 无条件跳转
	mCodelist.push_back(codereturnfor);

	mIsFor--;
	endforlabel->SetAddr(mCodelist.size());

	mForbreak.pop_back();
	mForcontinue.pop_back();

	isBorC = false;	
}

// Error处理
void Parser::parseErrorA()
{
	if (mCurToken.token == "EAND") {
		//PushErrorList('a');
		m_pErrorHandler->PushErrorList('a', mCurToken.lineNum);
		AdvanceToNextToken();

		parseLAndExp();
	}
	else if (mCurToken.token == "EOR") {
		m_pErrorHandler->PushErrorList('a', mCurToken.lineNum);
		AdvanceToNextToken();

		parseLOrExp();
	}
}

bool Parser::parseErrorB(std::string name, int type, int kind, int isconst, int nowScope)
{
	auto pSymbol = m_pSymbolTableMgr->findSymbol(name, nowScope);
	if (pSymbol)
	{
		m_pErrorHandler->PushErrorList('b', mCurToken.lineNum);
		return false;
	}
	if (mIsEqualScope) 
	{
		auto pSymbol = m_pSymbolTableMgr->findSymbol(name, mCurScope);
		if (pSymbol)
		{
			m_pErrorHandler->PushErrorList('b', mCurToken.lineNum);
			return false;
		}
	}

	m_pSymbolTableMgr->add_s_Table(name, type, kind, isconst, mPcodeAddress, mIsGlbScope, mIsEqualScope, mGlobalSymbolScope, mCurScope, mSaveArrExp);

	return true;
}

SymbolTable* Parser::parseErrorC(std::string name)
{
	SymbolTable* foundSymbol = nullptr;  // 찾은 심볼을 저장할 변수
	bool isFound = false;
	std::stack<int> tempScopeStack = mScopeStack; // 임시 스택을 사용하여 원래 스택을 변경하지 않음

	while (!tempScopeStack.empty()) {
		int currentScope = tempScopeStack.top(); // 현재 스코프
		tempScopeStack.pop(); // 스택을 망가뜨리지 않고 상위 스코프로 이동

		// 현재 스코프에서 심볼 찾기
		SymbolTable* symbol = m_pSymbolTableMgr->findSymbol(name, currentScope);
		if (symbol != nullptr) {
			foundSymbol = symbol;  // 심볼을 찾으면 foundSymbol에 저장
			isFound = true;
			break;
		}
	}

	// 모든 스코프를 탐색했음에도 불구하고 심볼을 찾지 못했으면 에러 추가
	if (!isFound) {
		m_pErrorHandler->PushErrorList('c', mCurToken.lineNum);
	}

	return foundSymbol;  // 찾은 심볼을 반환
}

void Parser::parseErrorD(int funcline)
{
	m_pErrorHandler->PushErrorList('d', mCurToken.lineNum);
	return;
}

void Parser::parseErrorE(int param_num)
{
	FuncParam& curFunc = mFuncparse.back();

	if (curFunc.paramcnt == 0) { // 매개변수 없을 경우 체크 X
		return;
	}

	int nextTokencnt = 0;
	bool first = true;

	while (mCurToken.word != ")" && mCurToken.word != "," && mCurToken.word != ";") {
		if (first) {
			first = false;
		}
		else {
			AdvanceToNextToken();
			nextTokencnt++;
		}

		// 변수일 때
		if (mCurToken.token == "IDENFR") {
			m_pSymbolTableMgr->findAndSetSymbol(mCurToken.word);
			if (curFunc.paramkind[param_num - 1] == 3) { // 기대값이 배열일 때
				AdvanceToNextToken(); // 배열을 단일로 받는 경우 처리
				if (mCurToken.word == "[") {
					AdvanceToNextToken();
					if (isdigit(mCurToken.word[0])) {
						MoveToPreviousToken ();
						MoveToPreviousToken ();
						m_pErrorHandler->PushErrorList('e', mLastFuncLine);
						break;
					}
					MoveToPreviousToken ();
				}
				MoveToPreviousToken ();

				if (m_pSymbolTableMgr->GetTempSymbol().type != curFunc.paramtype[param_num - 1] ||
					m_pSymbolTableMgr->GetTempSymbol().kind != curFunc.paramkind[param_num - 1]) {
					m_pErrorHandler->PushErrorList('e', mLastFuncLine);
					break;
				}
			}
			else if (curFunc.paramkind[param_num - 1] == 0) { // 일반 변수
				bool isfind = false;
				AdvanceToNextToken(); // 배열을 단일로 받는 경우 처리
				if (mCurToken.word == "[") {
					AdvanceToNextToken();
					if (isdigit(mCurToken.word[0])) {
						isfind = true;
					}
					MoveToPreviousToken ();
				}
				MoveToPreviousToken ();

				if (!isfind && m_pSymbolTableMgr->GetTempSymbol().kind == 3) {
					m_pErrorHandler->PushErrorList('e', mLastFuncLine);
					break;
				}
			}
		}
		else if (isdigit(mCurToken.word[0])) { // 숫자이고 배열이 아니면 인트
			//printfSymbol[print_top].name = mCurToken.word; // print only
			//printfSymbol[print_top].kind = 0;
			//printfSymbol[print_top].type = 0;
			//print_top++;

			if (curFunc.paramkind[param_num - 1] == 3) { // 배열 기대 시 에러 추가
				m_pErrorHandler->PushErrorList('e', mLastFuncLine);
				break;
			}
		}
		else if (isalpha(mCurToken.word[0]) || mCurToken.word[0] == '\'') {
			//printfSymbol[print_top].name = mCurToken.word; // print only
			//printfSymbol[print_top].kind = 0;
			//printfSymbol[print_top].type = 1;
			//print_top++;

			if (curFunc.paramkind[param_num - 1] == 3) { // 배열 기대 시 에러 추가
				m_pErrorHandler->PushErrorList('e', mLastFuncLine);
				break;
			}
		}
	}

	// 반복문 종료 후, AdvanceToNextToken  호출 횟수만큼 MoveToPreviousToken  호출
	while (nextTokencnt > 0) {
		MoveToPreviousToken ();
		nextTokencnt--;
	}

	return;
}

void Parser::parseErrorF(int linenum)
{
	if (!mIsNeedReturn) {
		m_pErrorHandler->PushErrorList('f', linenum);
	}
}

void Parser::parseErrorG()
{
	if (mIsNeedReturn) {
		if (!mIsStmtReturn) {
			m_pErrorHandler->PushErrorList('g', mCurToken.lineNum);
		}
	}
}

void Parser::parseErrorH(std::string name)
{
	auto pSymbol = m_pSymbolTableMgr->findSymbol(name, mGlobalSymbolScope);
	if (pSymbol)
	{
		auto pScopeSymbol = m_pSymbolTableMgr->findSymbol(name, mCurScope);
		if (!pScopeSymbol)
		{
			pScopeSymbol = m_pSymbolTableMgr->findSymbol(name, mFuncStartScope);
			if (!pScopeSymbol)
			{
				pScopeSymbol = m_pSymbolTableMgr->findSymbol(name, 1);
			}
		}
		if (pScopeSymbol)
		{
			if (pScopeSymbol->isconst == 1)
			{
				m_pErrorHandler->PushErrorList('h', mCurToken.lineNum);
			}
		}
	}
}

bool Parser::parseErrorI()
{
	if (mCurToken.word != ";") {
		m_pErrorHandler->PushErrorList('i', mLastNonT);
		return true;;
	}
	else {
		// printcurToken();
		AdvanceToNextToken();
	}
	return false;
}

bool Parser::parseErrorJ()
{
	if (mCurToken.word != ")") {
		m_pErrorHandler->PushErrorList('j', mLastNonT);
		return true;
	}
	else {
		// printcurToken();
		AdvanceToNextToken();
	}
	return false;
}

void Parser::parseErrorK()
{
	if (mCurToken.word != "]") {
		m_pErrorHandler->PushErrorList('k', mLastNonT);
	}
	else {
		// printcurToken();
		AdvanceToNextToken();
	}
}

void Parser::parseErrorL(std::string formatString, int print_param_cnt, int printlinenum)
{
	int formatCount = 0;
	for (int i = 0; i < formatString.length(); i++) {
		if (formatString[i] == '%' && (formatString[i + 1] == 'd' || formatString[i + 1] == 'c')) { // '%' 문자가 포맷 문자인 경우
			formatCount++;
		}
	}

	if (formatCount != print_param_cnt) {
		m_pErrorHandler->PushErrorList('l', printlinenum);
	}
}

void Parser::parseErrorM()
{
	if (!isBorC) {
		m_pErrorHandler->PushErrorList('m', mCurToken.lineNum);
	}
}
