#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <stack>
#include <vector>
#include <array>
#include <memory>
#include <algorithm>
#include <cmath>

using namespace std;

enum KindEnum
{
	VAR,
	FUNC,
	PARAM,
	ARR
};

enum EnumTypeEnum
{
	INT_TYPE,
	CHAR_TYPE,
	VOID_TYPE
};

struct FuncParam {
	int type;						//0 = int, 1 = char, 2 = void
	string name;
	int paramcnt = 0;
	vector<int> paramtype;
	vector<int> paramkind;
	int expectedcnt = 0;
	int startCode = 0;
};


// 主要用于表示一个标签，通常在代码生成和解释执行阶段用于标记特定的位置
class PCodeLabel
{
public:
	PCodeLabel() {}

	int GetAddr()
	{
		return mAdd;
	}

	void SetAddr(int add)
	{
		this->mAdd = add;
	}
private:
	int mAdd = 0;
};

class PCode {
private:
	string name;
	int scope = 0;
	int addr = 0;
	string print = "";
	shared_ptr<PCodeLabel> label = nullptr;

	int type = 0;
public:
	PCode() {}

	PCode(string name) : name(name), type(1) {}
	PCode(string name, int addr) : name(name), addr(addr), type(2) {}
	PCode(string name, string print) : name(name), print(print), type(3) {}
	PCode(string name, shared_ptr<PCodeLabel> label) : name(name), label(label), type(4) {}
	PCode(string name, int scope, int addr) : name(name), scope(scope), addr(addr), type(5) {}

	string getName() {
		return name;
	}

	int getScope() {
		return scope;
	}

	int getaddr() {
		return addr;
	}

	string getPrint() {
		return print;
	}

	shared_ptr<PCodeLabel> getLabel() {
		return label;
	}
};

class Value {
private:
	int value;
	string Svalue;
	bool isInt;
public:

	Value(int value) : value(value), isInt(true) {}
	Value(string Svalue) : Svalue(Svalue), isInt(false) {}

	int getValue() {
		if (isInt) {
			return value;
		}
		return 0;
	}

	void setValue(int value) {
		this->value = value;
		isInt = true;
	}

	string getSValue() {
		if (!isInt) {
			return Svalue;
		}
	}

	void setSValue(string Svalue) {
		this->Svalue = Svalue;
		isInt = false;
	}
};

/////////////////////////////////////////////////////

enum EnumTokenType
{
	// 标识符
	IDENFR,
	// 整数常量
	INTCON,
	// 字符串常量
	STRCON,
	// 字符常量
	CHRCON,
	// 关键字 "main"
	MAINTK,
	// 关键字 "const"
	CONSTTK,
	// 关键字 "int"
	INTTK,
	// 关键字 "char"
	CHARTK,
	// 关键字 "break"
	BREAKTK,
	// 关键字 "continue"
	CONTINUETK,
	// 关键字 "if"
	IFTK,
	// 关键字 "else"
	ELSETK,
	// 关键字 "for"
	FORTK,
	// 关键字 "getint"
	GETINTTK,
	// 关键字 "getchar"
	GETCHARTK,
	// 关键字 "printf"
	PRINTFTK,
	// 关键字 "return"
	RETURNTK,
	// 关键字 "void"
	VOIDTK,
	// 逻辑非运算符
	NOT,
	// 逻辑与运算符
	AND,
	EAND, // & 错误
	// 逻辑或运算符
	OR,
	EOR, // | 错误
	// 乘法运算符
	MULT,
	DMULT,
	// 除法运算符
	DIV,
	// 取模运算符
	MOD,
	// 小于运算符
	LSS,
	// 小于等于运算符
	LEQ,
	// 大于运算符
	GRE,
	// 大于等于运算符
	GEQ,
	// 等于运算符
	EQL,
	// 不等于运算符
	NEQ,
	// 赋值运算符
	ASSIGN,
	// 加法运算符
	PLUS,
	DPLUS,
	// 减法运算符
	MINU,
	// 分号
	SEMICN,
	// 逗号
	COMMA,
	// 左小括号
	LPARENT,
	// 右小括号
	RPARENT,
	// 左中括号
	LBRACK,
	// 右中括号
	RBRACK,
	// 左大括号
	LBRACE,
	// 右大括号
	RBRACE,
	BITAND,
};

struct TokenWord
{
	EnumTokenType tokenType;
	string tokenName;
	string token;
	string word;
	int lineNum;
};

class Lexer
{
public:
	Lexer(string_view filePath);

	void LexicalAnalysis2();

	const vector<TokenWord>& GetTokenList() const;

	void WriteToFile(string_view filePath);
private:
	string mInputFilePath;
	std::string mInputText;
	int mCurpos;
	int mCurrentLinenum;

	std::vector<TokenWord> mTokens;

	void SkipBlank();
	void SkipLineComment();
	void SkipBlockComment();
	void PushTypeToken(const std::string& Token);
	void PushTokenList(const std::string& token, const std::string& word);
	void PushTokenList(EnumTokenType tokenType, const std::string& word, int lineNum);
	bool CheckCharType(int check);

	static std::string TokenTypeToString(EnumTokenType token);
	static EnumTokenType StringToTokenType(std::string_view token);
private:
	static std::unordered_map<std::string, EnumTokenType> mKeywords;
	static std::unordered_map<std::string, EnumTokenType> mOperators;
};

std::unordered_map<std::string, EnumTokenType> Lexer::mKeywords = {
	{"const", CONSTTK}, {"int", INTTK}, {"char", CHARTK}, {"break", BREAKTK},
	{"continue", CONTINUETK}, {"if", IFTK}, {"else", ELSETK}, {"for", FORTK},
	{"getint", GETINTTK}, {"getchar", GETCHARTK}, {"printf", PRINTFTK},
	{"return", RETURNTK}, {"void", VOIDTK}, {"main", MAINTK}
};
std::unordered_map<std::string, EnumTokenType> Lexer::mOperators = {
	{"+", PLUS}, {"-", MINU}, {"*", MULT}, {"/", DIV}, {"%", MOD},
	{"<", LSS}, {"<=", LEQ}, {">", GRE}, {">=", GEQ}, {"==", EQL},
	{"!=", NEQ}, {"=", ASSIGN}, {"&&", AND}, {"||", OR}, {"!", NOT},
	{";", SEMICN}, {",", COMMA}, {"(", LPARENT}, {")", RPARENT},
	{"[", LBRACK}, {"]", RBRACK}, {"{", LBRACE}, {"}", RBRACE},
	{"**", DMULT}, {"++", DPLUS} ,{"bitand", BITAND}

};

Lexer::Lexer(std::string_view filePath) : mCurpos(0), mCurrentLinenum(1)
{
	mInputFilePath = filePath;

	std::ifstream inputFile(filePath.data());
	if (!inputFile.is_open())
	{
		//assert(false);
		return;
	}
	mInputText = std::string((std::istreambuf_iterator<char>(inputFile)), std::istreambuf_iterator<char>());
	inputFile.close();

}

void Lexer::SkipBlank()
{
	while (mCurpos < mInputText.length() &&
		(mInputText[mCurpos] == ' ' || mInputText[mCurpos] == '\t' ||
			mInputText[mCurpos] == '\r' || mInputText[mCurpos] == '\n')) {
		if (mInputText[mCurpos] == '\n') {
			mCurrentLinenum++;
		}
		mCurpos++;
	}
}

void Lexer::SkipLineComment()
{
	while (mInputText[mCurpos] != '\n' && mInputText[mCurpos] != '\r' && mCurpos < mInputText.length()) {
		mCurpos++;
	}
}

void Lexer::SkipBlockComment()
{
	while (!(mInputText[mCurpos] == '*' && mInputText[mCurpos + 1] == '/') && mCurpos < mInputText.length()) {
		mCurpos++;
	}
	if (mCurpos < mInputText.length()) {
		mCurpos += 2;
	}
}

void Lexer::PushTypeToken(const std::string& Token)
{
	if (Token == "main") {
		PushTokenList("MAINTK", "main");
	}
	else if (Token == "const") {
		PushTokenList("CONSTTK", "const");
	}
	else if (Token == "int") {
		PushTokenList("INTTK", "int");
	}
	else if (Token == "char") {
		PushTokenList("CHARTK", "char");
	}
	else if (Token == "break") {
		PushTokenList("BREAKTK", "break");
	}
	else if (Token == "continue") {
		PushTokenList("CONTINUETK", "continue");
	}
	else if (Token == "if") {
		PushTokenList("IFTK", "if");
	}
	else if (Token == "else") {
		PushTokenList("ELSETK", "else");
	}
	else if (Token == "for") {
		PushTokenList("FORTK", "for");
	}
	else if (Token == "getint") {
		PushTokenList("GETINTTK", "getint");
	}
	else if (Token == "getchar") {
		PushTokenList("GETCHARTK", "getchar");
	}
	else if (Token == "printf") {
		PushTokenList("PRINTFTK", "printf");
	}
	else if (Token == "return") {
		PushTokenList("RETURNTK", "return");
	}
	else if (Token == "void") {
		PushTokenList("VOIDTK", "void");
	}
	else {
		PushTokenList("IDENFR", Token);
	}
}

void Lexer::PushTokenList(const std::string& token, const std::string& word) {
	TokenWord tokenWord;
	tokenWord.tokenType = StringToTokenType(token);
	tokenWord.token = token;
	tokenWord.word = word;
	tokenWord.lineNum = mCurrentLinenum;
	mTokens.push_back(tokenWord);
}

void Lexer::PushTokenList(EnumTokenType tokenType, const std::string& word, int lineNum)
{
	TokenWord tokenWord;
	tokenWord.tokenType = tokenType;
	tokenWord.token = TokenTypeToString(tokenType);
	tokenWord.word = word;
	tokenWord.lineNum = lineNum;
	mTokens.push_back(tokenWord);
}

bool Lexer::CheckCharType(int check)
{
	// 定义常量
	static const int DOUBLE_QUOTE = 34;
	static const int SINGLE_QUOTE = 39;
	static const int BACKSLASH = 92;
	static const int NULL_CHAR = 0;

	if ((check >= 32 && check <= 126) || (check >= 7 && check <= 12) || check == DOUBLE_QUOTE
		|| check == SINGLE_QUOTE || check == BACKSLASH || check == NULL_CHAR)
		return true;
	return false;
}

std::string Lexer::TokenTypeToString(EnumTokenType token)
{
	switch (token) {
	case IDENFR: return "IDENFR";
	case INTCON: return "INTCON";
	case STRCON: return "STRCON";
	case CHRCON: return "CHRCON";
	case MAINTK: return "MAINTK";
	case CONSTTK: return "CONSTTK";
	case INTTK: return "INTTK";
	case CHARTK: return "CHARTK";
	case BREAKTK: return "BREAKTK";
	case CONTINUETK: return "CONTINUETK";
	case IFTK: return "IFTK";
	case ELSETK: return "ELSETK";
	case FORTK: return "FORTK";
	case GETINTTK: return "GETINTTK";
	case GETCHARTK: return "GETCHARTK";
	case PRINTFTK: return "PRINTFTK";
	case RETURNTK: return "RETURNTK";
	case VOIDTK: return "VOIDTK";
	case NOT: return "NOT";
	case AND: return "AND";
	case EAND: return "EAND";
	case OR: return "OR";
	case EOR:return "EOR";
	case MULT: return "MULT";
	case DMULT: return "DMULT";
	case DIV: return "DIV";
	case MOD: return "MOD";
	case LSS: return "LSS";
	case LEQ: return "LEQ";
	case GRE: return "GRE";
	case GEQ: return "GEQ";
	case EQL: return "EQL";
	case NEQ: return "NEQ";
	case ASSIGN: return "ASSIGN";
	case PLUS: return "PLUS";
	case DPLUS: return "DPLUS";
	case MINU: return "MINU";
	case SEMICN: return "SEMICN";
	case COMMA: return "COMMA";
	case LPARENT: return "LPARENT";
	case RPARENT: return "RPARENT";
	case LBRACK: return "LBRACK";
	case RBRACK: return "RBRACK";
	case LBRACE: return "LBRACE";
	case RBRACE: return "RBRACE";
		case BITAND: return "BITAND";
	default: return " ";
	}
}

EnumTokenType Lexer::StringToTokenType(std::string_view tokenStr)
{
	if (tokenStr == "IDENFR") return IDENFR;
	if (tokenStr == "INTCON") return INTCON;
	if (tokenStr == "STRCON") return STRCON;
	if (tokenStr == "CHRCON") return CHRCON;
	if (tokenStr == "MAINTK") return MAINTK;
	if (tokenStr == "CONSTTK") return CONSTTK;
	if (tokenStr == "INTTK") return INTTK;
	if (tokenStr == "CHARTK") return CHARTK;
	if (tokenStr == "BREAKTK") return BREAKTK;
	if (tokenStr == "CONTINUETK") return CONTINUETK;
	if (tokenStr == "IFTK") return IFTK;
	if (tokenStr == "ELSETK") return ELSETK;
	if (tokenStr == "FORTK") return FORTK;
	if (tokenStr == "GETINTTK") return GETINTTK;
	if (tokenStr == "GETCHARTK") return GETCHARTK;
	if (tokenStr == "PRINTFTK") return PRINTFTK;
	if (tokenStr == "RETURNTK") return RETURNTK;
	if (tokenStr == "VOIDTK") return VOIDTK;
	if (tokenStr == "NOT") return NOT;
	if (tokenStr == "AND") return AND;
	if (tokenStr == "OR") return OR;
	if (tokenStr == "MULT") return MULT;
	if (tokenStr == "DMULT") return DMULT;
	if (tokenStr == "DIV") return DIV;
	if (tokenStr == "MOD") return MOD;
	if (tokenStr == "LSS") return LSS;
	if (tokenStr == "LEQ") return LEQ;
	if (tokenStr == "GRE") return GRE;
	if (tokenStr == "GEQ") return GEQ;
	if (tokenStr == "EQL") return EQL;
	if (tokenStr == "NEQ") return NEQ;
	if (tokenStr == "ASSIGN") return ASSIGN;
	if (tokenStr == "PLUS") return PLUS;
	if (tokenStr == "DPLUS") return DPLUS;
	if (tokenStr == "MINU") return MINU;
	if (tokenStr == "SEMICN") return SEMICN;
	if (tokenStr == "COMMA") return COMMA;
	if (tokenStr == "LPARENT") return LPARENT;
	if (tokenStr == "RPARENT") return RPARENT;
	if (tokenStr == "LBRACK") return LBRACK;
	if (tokenStr == "RBRACK") return RBRACK;
	if (tokenStr == "LBRACE") return LBRACE;
	if (tokenStr == "RBRACE") return RBRACE;
	if (tokenStr == "BITAND") return BITAND;
	// 如果没有匹配的字符串，可根据需求进行错误处理，这里简单返回默认值
	//assert(false);
	return IDENFR;
}

void Lexer::WriteToFile(std::string_view filePath)
{
	std::fstream outFile(filePath.data(), std::ios::trunc | std::ios::out);
	if (outFile.good())
	{
		for (const auto& token : mTokens)
		{
			std::cout << token.token << " " << token.word << std::endl;
			outFile << token.token << " " << token.word << std::endl;
		}
	}
	outFile.flush();
	outFile.close();
}

void Lexer::LexicalAnalysis2()
{
	mTokens.clear();

	std::ifstream inputFile(mInputFilePath.data());
	char currentChar;

	mCurrentLinenum = 1;
	while (inputFile.get(currentChar))
	{
		if (isspace(currentChar))
		{
			if (currentChar == '\n')
			{
				mCurrentLinenum++;
			}
			continue;
		}

		// 주석 처리 및 나누기 처리
		else if (currentChar == '/')
		{
			char nextChar = inputFile.peek();

			// 주석인데 /*로 시작할 경우
			if (nextChar == '*')
			{
				inputFile.get(); // '*' 소비
				bool endofComment = false;

				while (inputFile.get(currentChar))
				{
					if (currentChar == '\n') {
						mCurrentLinenum++;
					}

					else if (currentChar == '*')
					{
						if (inputFile.peek() == '/')
						{
							inputFile.get(); // '/' 소비
							endofComment = true;
							break;
						}
					}

					if (inputFile.eof()) break; // 파일 끝에 도달 시 종료
				}
				continue;
			}

			// 주석인데 //로 시작할 경우
			else if (nextChar == '/')
			{
				inputFile.get(); // 두 번째 '/' 소비

				while (inputFile.get(currentChar)) {
					if (currentChar == '\n') {
						mCurrentLinenum++; // 줄 번호 증가
						break;
					}

					if (inputFile.eof()) break; // 파일 끝에 도달 시 종료
				}
				continue;
			}

			// 나눗셈인 경우
			else
			{
				PushTokenList(DIV, "/", mCurrentLinenum);
			}
		}

		// 숫자 처리
		else if (isdigit(currentChar))
		{
			std::string number;
			number += currentChar;

			// EOF 체크 추가
			while (isdigit(inputFile.peek()))
			{
				number += inputFile.get();
			}
			PushTokenList(INTCON, number, mCurrentLinenum);
		}

		// 문자열 상수 처리
		else if (currentChar == '"')
		{
			std::string strConst;
			strConst += currentChar;
			bool closed = false;

			while (inputFile.peek() != EOF) {
				char c = inputFile.get();
				strConst += c;

				if (c == '"') {
					closed = true;
					break;
				}

				if (c == '\n') {
					mCurrentLinenum++;
					break;
				}

				if (inputFile.eof()) break; // 파일 끝에 도달 시 종료
			}

			if (closed) {
				PushTokenList(STRCON, strConst, mCurrentLinenum);
			}
			else
			{
				//assert(false);
				//mErrors.push_back( lineNumber, 'a' );
			}
		}

		// 문자 상수 처리
		else if (currentChar == '\'') {

			std::string charConst;
			charConst += currentChar;       // 첫 번째 따옴표 추가
			char c = inputFile.get();
			charConst += c;

			if (c == '\\') // 이스케이프 시퀀스 시작
			{
				char nextChar = inputFile.get();
				charConst += nextChar; // 이스케이프 시퀀스의 두 번째 문자 추가
				c = nextChar; // c를 업데이트하여 다음 검사에서 사용
			}

			if (inputFile.peek() == '\'')
			{
				charConst += inputFile.get(); // 종료 따옴표 추가
				PushTokenList(CHRCON, charConst, mCurrentLinenum);
			}
			else
			{
				// 오류 발생 시 errors 벡터에 추가
				//assert(false);
			}
		}

		// ** 加办法
		else if (currentChar == '*') {
			std::string op(1, currentChar);
			char nextChar = inputFile.peek();

			if (nextChar == '*') {
				op += inputFile.get();
				PushTokenList(DMULT, op, mCurrentLinenum);
			}

			else {
				PushTokenList(MULT, op, mCurrentLinenum);
			}
		}

		// ++
		else if (currentChar == '+') {
			std::string op(1, currentChar);
			char nextChar = inputFile.peek();

			if (nextChar == '+') {
				op += inputFile.get();
				PushTokenList(DPLUS, op, mCurrentLinenum);
			}

			else {
				PushTokenList(PLUS, op, mCurrentLinenum);
			}
		}

		// 식별자 처리
		else if (isalpha(currentChar) || currentChar == '_')
		{
			std::string ident;   // 식별자 선언
			ident += currentChar;

			while (inputFile.peek() != EOF && (isalnum(inputFile.peek()) || inputFile.peek() == '_'))
			{
				ident += inputFile.get();
			}

			if (ident == "bitand") {
				PushTokenList(BITAND, ident, mCurrentLinenum);
			}
			// 키워드인지 판단
			else if (mKeywords.find(ident) != mKeywords.end())
			{
				PushTokenList(mKeywords[ident], ident, mCurrentLinenum);
			}
			else
			{
				//HRLog::Instance()->LogError(std::format("unknow keyword:{}", ident), __FUNCTION__, __LINE__);
				// 식별자 토큰 추가
				PushTokenList(IDENFR, ident, mCurrentLinenum);
			}
		}

		// 연산자 및 구분자 처리
		else {
			std::string op(1, currentChar);
			char nextChar = inputFile.peek();

			if (currentChar == '&') {

				if (nextChar == '&') {
					op += inputFile.get();
					PushTokenList(AND, op, mCurrentLinenum);
				}

				else {
					// mErrors.push_back({ lineNumber, 'a' });
					//assert(false);
					PushTokenList(EAND, "&", mCurrentLinenum);
					continue;
				}
			}

			else if (currentChar == '|') {

				if (nextChar == '|') {
					op += inputFile.get();
					PushTokenList(OR, op, mCurrentLinenum);
				}

				else {
					//assert(false);
					//mErrors.push_back({ lineNumber, 'a' });
					PushTokenList(EOR, "&", mCurrentLinenum);
					continue;
				}
			}

			else if ((currentChar == '!' || currentChar == '=' || currentChar == '<' || currentChar == '>') && nextChar == '=') {
				op += inputFile.get();
				PushTokenList(mOperators[op], op, mCurrentLinenum);
			}

			else if (mOperators.find(op) != mOperators.end())
			{
				PushTokenList(mOperators[op], op, mCurrentLinenum);
			}
		}

		if (inputFile.eof()) break; // 파일 끝에 도달 시 종료
	}

	inputFile.close();
}

const std::vector<TokenWord>& Lexer::GetTokenList() const {
	return mTokens;
}

/////////////////////////////////////////////////


struct error_word {
	int linenum;
	char err_type;
};

class ErrorHandler {
private:
	std::vector<error_word> error_list;

public:
	void push_err_list(char err_type, int err_line);
	void print_err_list(std::string_view filePath);
	bool HasError() { return !error_list.empty(); };
};

void ErrorHandler::push_err_list(char err_type, int err_line) {
	error_word error;
	error.linenum = err_line;
	error.err_type = err_type;
	error_list.push_back(error);
}

void ErrorHandler::print_err_list(std::string_view filePath) {
	std::sort(error_list.begin(), error_list.end(), [](const error_word& a, const error_word& b) {
		if (a.linenum == b.linenum) {
			return a.err_type > b.err_type;
		}
		return a.linenum < b.linenum;
		});

	std::fstream outFile(filePath.data(), std::ios::trunc | std::ios::out);
	if (outFile.good())
	{
		int last_line = -1;
		for (const auto& error : error_list)
		{
			if (error.linenum != last_line)
			{
				outFile << error.linenum << " " << error.err_type << std::endl;
				std::cout << error.linenum << " " << error.err_type << std::endl;
				last_line = error.linenum;

			}
		}
	}
	outFile.flush();
	outFile.close();
}

struct SymbolTable {
	std::string name;
	int type = -1;		//0 = int, 1 = char, 2 = void
	int kind = -1;		//0 = var, 1 = func, 2 = param, 3 = arr
	int isconst = 0;	//cosnt = 1, not const = 0
	int scope;
	int pcodearr = 0;
	int arrsize = 0;
};

class SymbolTableManager {
private:
	std::vector<SymbolTable> symbolTable;
	SymbolTable SymbolTemp;
public:
	SymbolTableManager();
	const SymbolTable& GetTempSymbol() { return SymbolTemp; }
	//int GetSTop() { return ST_top; }

	void add_s_Table(const std::string& name, int type, int kind, int isconst, int mPcodeAddress, bool mIsGlbScope, bool mIsEqualScope, int mGlobalSymbolScope, int mCurScope, int savearrexp);
	//SymbolTable* parseErrorC(const std::string& name);
	void print_symbol_list();

	SymbolTable* findSymbol(const std::string& name, int scope);
	void findAndSetSymbol(const std::string& tokenName);

};


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

void SymbolTableManager::add_s_Table(const std::string& name, int type, int kind, int isconst, int mPcodeAddress, bool mIsGlbScope, bool mIsEqualScope, int mGlobalSymbolScope, int mCurScope, int savearrexp)
{
	SymbolTable symbol;

	symbol.name = name;
	symbol.type = type;
	symbol.kind = kind;
	symbol.isconst = isconst;
	symbol.pcodearr = mPcodeAddress;
	symbol.arrsize = savearrexp;
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

////////////////////////////////////////////////////////////////////////////

class Parser
{
public:
	Parser(const std::vector<TokenWord>& tokens);
	void SyntaxAnalysis();

	void WriteToFile(std::string_view filePath);

    const std::vector<std::shared_ptr<PCode>>& GetCodeList() { return mCodelist; }
	const std::shared_ptr<ErrorHandler>& GetErrorHandler() { return m_pErrorHandler; }

private:
	// 符号表管理器的智能指针
	std::shared_ptr<SymbolTableManager> m_pSymbolTableMgr;
	// 错误处理器的智能指针
	std::shared_ptr<ErrorHandler> m_pErrorHandler;

	// 存储词法分析得到的 Token 列表
	const std::vector<TokenWord>& mTokens;
	// 当前处理的 Token 位置
	int mCurpos = 0;
	// 当前处理的 Token
	TokenWord mCurToken;

	// 存储生成的代码列表
	//std::vector<PCode> mCodelist;
	std::vector<std::shared_ptr<PCode>> mCodelist;

	// 全局作用域标志
	bool mIsGlbScope = true;
	// 作用域相等标志
	bool mIsEqualScope = false;
	// 是否需要返回值标志
	bool mIsNeedReturn = true;
	// 语句是否返回标志
	bool mIsStmtReturn = false;
	// 其他布尔标志
	bool isBorC = false;
	// P 代码地址
	int mPcodeAddress = 0;

	int mIsFor = 0;
	int savearrexp = 0;
	bool isfuncDef = false;
	bool isforincontinue = false;
	bool pcodeislod = false;

	std::vector<FuncParam> mFuncInfos;
	std::vector<FuncParam> mFuncparse;

	// 符号作用域编号
	int mGlobalSymbolScope = 1;
	// 上一个非终结符的行号
	int mLastNonT;
	// 当前作用域编号
	int mCurScope = 1;
	// 函数开始的作用域编号
	int mFuncStartScope = 0;
	// 最后一个函数的行号（用于错误处理）
	int mLastFuncLine;

	// 是否为 if-else 语句的标志
	bool mIsIfElse = false;

	std::stack<int> mScopeStack;


	std::vector<std::shared_ptr<PCodeLabel>> mForbreak;
	std::vector<std::shared_ptr<PCodeLabel>> mForcontinue;

	// 移动到下一个 Token 的函数
	void AdvanceToNextToken();
	// 移动到上一个 Token 的函数
	void MoveToPreviousToken();

	// 各种语法规则解析函数
	void ParseCompUnit();  // 解析编译单元
	void ParseDecl();  // 解析声明
	void ParseConstDecl();  // 解析常量声明
	void ParseConstDef(int type, int  kind);  // 解析常量定义
	void ParseConstInitVal(int arrexp);  // 解析常量初始值
	void ParseVarDecl();  // 解析变量声明
	void ParseVarDef(int type, int kind);  // 解析变量定义
	void ParseInitVal(int arrexp);  // 解析初始值
	void ParseFuncDef();  // 解析函数定义
	void ParseMainFuncDef();  // 解析主函数定义
	void ParseFuncFParam();  // 解析函数形式参数
	void ParseBlock();  // 解析代码块
	void parseStmt();  // 解析语句
	void parsefor();  // 解析 for 循环
	void ParseForStmt();  // 解析 for 语句
	int ParseExp();  // 解析表达式
	void ParseCond();  // 解析条件
	int ParseLVal(int type);  // 解析左值表达式
	int ParsePrimaryExp(int type);  // 解析基本表达式
	int ParseUnaryExp(int type);  // 解析一元表达式
	int ParseFuncRParams();  // 解析函数实际参数列表
	int ParseMulExp(int type);  // 解析乘法表达式
	int ParseAddExp(int type);  // 解析加法表达式
	void ParseRelExp();  // 解析关系表达式
	void ParseEqExp();  // 解析相等表达式
	void parseLAndExp();  // 解析逻辑与表达式
	void parseLOrExp();  // 解析逻辑或表达式
	void parseConstExp();  // 解析常量表达式


	void parseErrorA();
	bool parseErrorB(std::string name, int type, int kind, int isconst, int nowScope);
	SymbolTable* parseErrorC(std::string name);
	void parseErrorD(int funcline);
	void parseErrorE(int param_num);
	void parseErrorF(int linenum);
	void parseErrorG();
	void parseErrorH(std::string name);
	bool parseErrorI();
	bool parseErrorJ();
	void parseErrorK();
	void parseErrorL(std::string formatString, int print_param_cnt, int printlinenum);
	void parseErrorM();

	// 计算表达式值的函数
	Value constExpValue();
	Value AddExpValue();
	Value MulExpValue();
	Value UnaryExpValue();
	Value PrimaryExpValue();
	Value ExpValue();
	Value LValValue();
};

Parser::Parser(const std::vector<TokenWord>& tokens) : mTokens(tokens), mCurpos(0) {
	m_pSymbolTableMgr = std::make_shared<SymbolTableManager>();
	m_pErrorHandler = std::make_shared<ErrorHandler>();

	if (!mTokens.empty()) {
		mCurToken = mTokens[mCurpos];
	}
}

void Parser::AdvanceToNextToken()
{
	if (mCurpos < mTokens.size() - 1)
	{
		mCurpos++;
		mCurToken = mTokens[mCurpos];
	}
	else
	{
		//std::cerr << "Error: Token list out of range." << std::endl;
		//assert(false);
	}
}

void Parser::MoveToPreviousToken()
{
	if (mCurpos > 0)
	{
		mCurpos--;
		mCurToken = mTokens[mCurpos];
	}
	else
	{
		//std::cerr << "Error: Token list out of range." << std::endl;
		//assert(false);
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

	auto label = std::make_shared<PCodeLabel>();
	auto code1 = std::make_shared<PCode>("JMP", label);
	mCodelist.push_back(code1);

	while (isFunctionDefinition())
	{
		ParseFuncDef();
	}
	if (mTokens[mCurpos].tokenType == EnumTokenType::INTTK && mTokens[mCurpos + 1].tokenType == EnumTokenType::MAINTK) {
		temp = mCodelist.size();

		label->SetAddr(temp);

		ParseMainFuncDef();
		//printSyntax("CompUnit");
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
	else
	{
		// assert(false);
	}
}

void Parser::ParseConstDecl()
{
	int type = -1;
	int kind = 0;

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

	std::string identname = mCurToken.word;
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

		auto code1 = std::make_shared<PCode>("INT", 1);
		auto code2 = std::make_shared<PCode>("LOD_A", 0, mPcodeAddress);
		mCodelist.push_back(code1);
		mCodelist.push_back(code2);
		mPcodeAddress++;
		ParseExp();
		auto code3 = std::make_shared<PCode>("STO");
		mCodelist.push_back(code3);
		arrexp--;
		while (mCurToken.word == ",") {
			AdvanceToNextToken();

			auto code1 = std::make_shared<PCode>("INT", 1);
			auto code2 = std::make_shared<PCode>("LOD_A", 0, mPcodeAddress);
			mCodelist.push_back(code1);
			mCodelist.push_back(code2);
			mPcodeAddress++;
			ParseExp();
			auto code3 = std::make_shared<PCode>("STO");
			mCodelist.push_back(code3);
			arrexp--;
		}

		while (arrexp) {
			auto code1 = std::make_shared<PCode>("INT", 1);
			auto code2 = std::make_shared<PCode>("LOD_A", mPcodeAddress);
			mCodelist.push_back(code1);
			mCodelist.push_back(code2);
			mPcodeAddress++;
			auto code3 = std::make_shared<PCode>("LOD_C", 0);
			auto code4 = std::make_shared<PCode>("STO");
			mCodelist.push_back(code3);
			mCodelist.push_back(code4);
			arrexp--;
		}

		AdvanceToNextToken();
	}
	// 상수문자열 처리
	else if (mCurToken.tokenType == STRCON)
	{
		std::string str = mCurToken.word;
		//"제거
		str.erase(remove(str.begin(), str.end(), '\"'), str.end());

		AdvanceToNextToken();

		for (int i = 0; i < str.length(); i++) {
			auto code1 = std::make_shared<PCode>("INT", 1);
			mCodelist.push_back(code1);
			auto code2 = std::make_shared<PCode>("LOD_A", 0, mPcodeAddress);
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

			auto code3 = std::make_shared<PCode>("LOD_C", val);	//mCurToken.word[i]
			auto code4 = std::make_shared<PCode>("STO");
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

	std::string identname = mCurToken.word;
	AdvanceToNextToken();
	if (mCurToken.word == "[") {
		AdvanceToNextToken();
		Value value1 = constExpValue();
		cnt = value1.getValue();
		savearrexp = cnt;

		parseErrorK(); //  ]
		kind = ARR;
	}
	parseErrorB(identname, type, kind, 0, mGlobalSymbolScope);
	savearrexp = 0;

	if (mCurToken.word == "=") {
		AdvanceToNextToken();

		ParseInitVal(cnt);
	}
	else {
		for (int i = 0; i < cnt; i++)
		{
			auto code1 = std::make_shared<PCode>("INT", 1);
			auto code2 = std::make_shared<PCode>("LOD_A", 0, mPcodeAddress);
			mPcodeAddress++;
			auto code3 = std::make_shared<PCode>("LOD_C", 0);	//배열 값초기화 X 0으로 초기화
			auto code4 = std::make_shared<PCode>("STO");
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

		auto code1 = std::make_shared<PCode>("INT", 1);
		auto code2 = std::make_shared<PCode>("LOD_A", 0, mPcodeAddress);
		mCodelist.push_back(code1);
		mCodelist.push_back(code2);
		mPcodeAddress++;
		ParseExp();
		auto code3 = std::make_shared<PCode>("STO");
		mCodelist.push_back(code3);
		arrexp--;
		while (mCurToken.word == ",") {
			AdvanceToNextToken();

			auto code1 = std::make_shared<PCode>("INT", 1);
			auto code2 = std::make_shared<PCode>("LOD_A", mPcodeAddress);
			mCodelist.push_back(code1);
			mCodelist.push_back(code2);
			mPcodeAddress++;
			ParseExp();
			auto code3 = std::make_shared<PCode>("STO");
			mCodelist.push_back(code3);
			arrexp--;
		}

		while (arrexp) {
			auto code1 = std::make_shared<PCode>("INT", 1);
			auto code2 = std::make_shared<PCode>("LOD_A", mPcodeAddress);
			mCodelist.push_back(code1);
			mCodelist.push_back(code2);
			mPcodeAddress++;
			auto code3 = std::make_shared<PCode>("LOD_C", 0);
			auto code4 = std::make_shared<PCode>("STO");
			mCodelist.push_back(code3);
			mCodelist.push_back(code4);
			arrexp--;
		}

		AdvanceToNextToken();
	}
	// 상수문자열 처리
	else if (mCurToken.tokenType == STRCON)
	{
		std::string str = mCurToken.word;
		str.erase(remove(str.begin(), str.end(), '\"'), str.end());

		AdvanceToNextToken();

		for (int i = 0; i < str.length(); i++)
		{
			auto code1 = std::make_shared<PCode>("INT", 1);
			mCodelist.push_back(code1);
			auto code2 = std::make_shared<PCode>("LOD_A", 0, mPcodeAddress);
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

			auto code3 = std::make_shared<PCode>("LOD_C", val);	//mCurToken.word[i]
			auto code4 = std::make_shared<PCode>("STO");
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
	isfuncDef = true;

	// 标记函数定义开始，这里注释掉的代码可能是用于标记函数定义的，暂时不用
	// isfuncDef = true;
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

	isfuncDef = false;
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
	auto handleIfStatement = [&]() {
		// 标记当前处于 if-else 语句块中
		mIsIfElse = true;

		// 创建两个标签，用于条件跳转
		auto label1 = std::make_shared<PCodeLabel>();
		auto label2 = std::make_shared<PCodeLabel>();

		// 移动到下一个 Token
		AdvanceToNextToken();
		AdvanceToNextToken();

		//Stmt → if '(' Btype Ident '=' InitVal ')' Stmt [else Stmt]
		// if (mCurToken.word == "int" || mCurToken.word == "char") {

		// 	// Btype
		// 	int type = -1;
		// 	if (mCurToken.word == "int") {
		// 		type = INT_TYPE;
		// 	}
		// 	else {
		// 		type = CHAR_TYPE;
		// 	}

		// 	AdvanceToNextToken();

		// 	std::string varName = mCurToken.word;
		// 	AdvanceToNextToken();

		// 	AdvanceToNextToken();//=

		// 	// 单变量
		// 	ParseInitVal(1);

		// 	AdvanceToNextToken();//)
		// }
		// else {
			// 解析条件表达式
			ParseCond();
			// 生成条件跳转指令，如果条件为假则跳转到 label1
			auto code1 = std::make_shared<PCode>("BZT", label1);
			// 将生成的指令添加到代码列表中
			mCodelist.push_back(code1);
			// 处理条件判断后的错误检查
			parseErrorJ();
		// }

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
			isforincontinue = true;
		}

		// 处理 break 或 continue 语句的错误检查
		parseErrorM();

		// 移动到下一个 Token
		AdvanceToNextToken();

		// 处理后续的错误检查
		parseErrorI();
		};

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
		int tempLineNumber = mCurToken.lineNum;
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
			if (tempLineNumber != mCurToken.lineNum) {
				parseErrorI();
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
			MoveToPreviousToken();
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

				AdvanceToNextToken();		 // "("
				mLastNonT = mCurToken.lineNum;
				AdvanceToNextToken();		 // ")"

				parseErrorJ();
				parseErrorI();
			}

			else if (mCurToken.word == "getchar") {
				auto code1 = std::make_shared<PCode>("GETC");
				mCodelist.push_back(code1);

				// 记录当前非终结符的行号，用于错误处理等
				AdvanceToNextToken();
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

// if forstmt加内容的时候
void Parser::ParseForStmt()
{
	//if (mCurToken.word == "int" || mCurToken.word == "char") {

	//	// Btype
	//	int type = -1;
	//	if (mCurToken.word == "int") {
	//		type = INT_TYPE;
	//	}
	//	else {
	//		type = CHAR_TYPE;
	//	}

	//	AdvanceToNextToken();

	//	// vardef
	//	int kind = 0;
	//	ParseVarDef(type, kind);

	//	while (mCurToken.word == ",") {
	//		AdvanceToNextToken();
	//		ParseVarDef(type, kind);
	//	}
	//}

	//else {
		parseErrorH(mCurToken.word);	//cosnt값 변환 에러 처리

		ParseLVal(-1);

		AdvanceToNextToken();
		ParseExp();

		auto code = std::make_shared<PCode>("STO");
		mCodelist.push_back(code);
	//}
}

int Parser::ParseExp()
{
	int type = INT_TYPE;
	mLastNonT = mCurToken.lineNum;

	type = ParseAddExp(type);

	return type;
}

// 如果有if的3行运算
void Parser::ParseCond()
{
	mLastNonT = mCurToken.lineNum;
	// 먼저 논리합 표현식(OR 등)을 파싱한다.
	parseLOrExp();

	// 만약 삼항 연산자 '?'가 나오면 true/false 분기를 생성한다.
	//if (mCurToken.word == "?") {
	//	// 거짓 분기와 전체 종료를 위한 라벨 생성
	//	auto falseLabel = std::make_shared<PCodeLabel>();
	//	auto endLabel = std::make_shared<PCodeLabel>();

	//	// 조건이 false일 경우 falseLabel로 점프하도록 BZT 명령어 생성
	//	auto branchFalse = std::make_shared<PCode>("BZT", falseLabel);
	//	mCodelist.push_back(branchFalse);

	//	// '?' 토큰 소비
	//	AdvanceToNextToken();

	//	// 참일 때의 식을 파싱하여 그 결과를 계산한다.
	//	ParseExp();

	//	// 참 분기 이후, false 부분을 건너뛰기 위한 무조건 점프(J) 명령어 생성
	//	auto jumpToEnd = std::make_shared<PCode>("J", endLabel);
	//	mCodelist.push_back(jumpToEnd);

	//	// falseLabel의 주소를 현재 코드 리스트 크기로 설정(즉, 참 분기 식 뒤 시작)
	//	falseLabel->SetAddr(mCodelist.size());

	//	// ':' 토큰 소비 (false 분기를 구분)
	//	AdvanceToNextToken();

	//	// 거짓일 때의 조건식은 재귀적으로 파싱한다.
	//	ParseCond();

	//	// 전체 삼항 연산 종료 후, endLabel의 주소를 현재 코드 리스트 크기로 설정한다.
	//	endLabel->SetAddr(mCodelist.size());
	//}
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
	else if (isfuncDef && isarr) {
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

// 把char类型转换为ASCII
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
	MoveToPreviousToken();
	std::string op;

	// 如果当前令牌是一元运算符（+、-、!）
	if (mCurToken.word == "+" || mCurToken.word == "-" || mCurToken.word == "!" || mCurToken.word == "++") {
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
		else if (op == "++") {
			auto code1 = std::make_shared<PCode>("DADD");
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

	while (mCurToken.word == "*" || mCurToken.word == "/" || mCurToken.word == "%" || mCurToken.word == "**" || mCurToken.word == "bitand") {
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
		else if (op == "**") {
			auto code4 = make_shared<PCode>("DMULT");
			mCodelist.push_back(code4);
		}
		else if(op == "bitand"){
			auto code4 = make_shared<PCode>("bitand");
			mCodelist.push_back(code4);
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
			outFile << "opcade:" << " " << code->getName() << " scope:" << code->getScope() << " print:" << code->getPrint() << std::endl;
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

	while (mCurToken.word == "*" || mCurToken.word == "/" || mCurToken.word == "%" || mCurToken.word == "**") {
		std::string op = mCurToken.word;

		AdvanceToNextToken();

		Value value1 = UnaryExpValue();
		nextval = value1.getValue();

		if (op == "*") {
			val *= nextval;
		}
		else if (op == "**") {
			int base = val + nextval;
			int result = pow(base, nextval);

			val = result;
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
	MoveToPreviousToken();
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
	isBorC = true;

	auto endforlabel = std::make_shared<PCodeLabel>();
	auto returnforlabel = std::make_shared<PCodeLabel>();
	auto startstmtlabel = std::make_shared<PCodeLabel>();
	auto returnthree = std::make_shared<PCodeLabel>();

	bool NotCalledCond = true;
	bool isthree = false;
	mIsFor++;

	mForbreak.push_back(endforlabel);
	mForcontinue.push_back(returnforlabel);

	AdvanceToNextToken();
	AdvanceToNextToken();

	if (mCurToken.word != ";") {
		ParseForStmt();
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

	isBorC = false;	//반복문 밖 브레이크 컨티뉴 처리
}

void Parser::parseErrorA()
{
	if (mCurToken.token == "EAND") {
		//push_err_list('a');
		m_pErrorHandler->push_err_list('a', mCurToken.lineNum);
		AdvanceToNextToken();

		parseLAndExp();
	}
	else if (mCurToken.token == "EOR") {
		m_pErrorHandler->push_err_list('a', mCurToken.lineNum);
		AdvanceToNextToken();

		parseLOrExp();
	}
}

bool Parser::parseErrorB(std::string name, int type, int kind, int isconst, int nowScope)
{
	auto pSymbol = m_pSymbolTableMgr->findSymbol(name, nowScope);
	if (pSymbol)
	{
		m_pErrorHandler->push_err_list('b', mCurToken.lineNum);
		return false;
	}
	if (mIsEqualScope)
	{
		auto pSymbol = m_pSymbolTableMgr->findSymbol(name, mCurScope);
		if (pSymbol)
		{
			m_pErrorHandler->push_err_list('b', mCurToken.lineNum);
			return false;
		}
	}

	m_pSymbolTableMgr->add_s_Table(name, type, kind, isconst, mPcodeAddress, mIsGlbScope, mIsEqualScope, mGlobalSymbolScope, mCurScope, savearrexp);

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
		m_pErrorHandler->push_err_list('c', mCurToken.lineNum);
	}

	return foundSymbol;  // 찾은 심볼을 반환
}

void Parser::parseErrorD(int funcline)
{
	m_pErrorHandler->push_err_list('d', mCurToken.lineNum);
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
						MoveToPreviousToken();
						MoveToPreviousToken();
						m_pErrorHandler->push_err_list('e', mLastFuncLine);
						break;
					}
					MoveToPreviousToken();
				}
				MoveToPreviousToken();

				if (m_pSymbolTableMgr->GetTempSymbol().type != curFunc.paramtype[param_num - 1] ||
					m_pSymbolTableMgr->GetTempSymbol().kind != curFunc.paramkind[param_num - 1]) {
					m_pErrorHandler->push_err_list('e', mLastFuncLine);
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
					MoveToPreviousToken();
				}
				MoveToPreviousToken();

				if (!isfind && m_pSymbolTableMgr->GetTempSymbol().kind == 3) {
					m_pErrorHandler->push_err_list('e', mLastFuncLine);
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
				m_pErrorHandler->push_err_list('e', mLastFuncLine);
				break;
			}
		}
		else if (isalpha(mCurToken.word[0]) || mCurToken.word[0] == '\'') {
			//printfSymbol[print_top].name = mCurToken.word; // print only
			//printfSymbol[print_top].kind = 0;
			//printfSymbol[print_top].type = 1;
			//print_top++;

			if (curFunc.paramkind[param_num - 1] == 3) { // 배열 기대 시 에러 추가
				m_pErrorHandler->push_err_list('e', mLastFuncLine);
				break;
			}
		}
	}

	// 반복문 종료 후, AdvanceToNextToken  호출 횟수만큼 MoveToPreviousToken  호출
	while (nextTokencnt > 0) {
		MoveToPreviousToken();
		nextTokencnt--;
	}

	return;
}

void Parser::parseErrorF(int linenum)
{
	if (!mIsNeedReturn) {
		m_pErrorHandler->push_err_list('f', linenum);
	}
}

void Parser::parseErrorG()
{
	if (mIsNeedReturn) {
		if (!mIsStmtReturn) {
			m_pErrorHandler->push_err_list('g', mCurToken.lineNum);
		}
	}
}

void Parser::parseErrorH(std::string name)
{
	// 전역 심볼(스코프 1)에서 먼저 검색
	SymbolTable* pSymbol = m_pSymbolTableMgr->findSymbol(name, 1);
	// 만약 전역 심볼이 없으면 현재 스코프나 함수 시작 스코프에서 검색
	if (!pSymbol) {
		pSymbol = m_pSymbolTableMgr->findSymbol(name, mCurScope);
		if (!pSymbol)
			pSymbol = m_pSymbolTableMgr->findSymbol(name, mFuncStartScope);
	}
	// 만약 심볼을 찾았고 그 심볼이 const라면 에러 h를 기록
	if (pSymbol && pSymbol->isconst == 1) {
		m_pErrorHandler->push_err_list('h', mCurToken.lineNum);
	}

	//auto pSymbol = m_pSymbolTableMgr->findSymbol(name, mGlobalSymbolScope);
	//if (pSymbol)
	//{
	//	auto pScopeSymbol = m_pSymbolTableMgr->findSymbol(name, mCurScope);
	//	if (!pScopeSymbol)
	//	{
	//		pScopeSymbol = m_pSymbolTableMgr->findSymbol(name, mFuncStartScope);
	//		if (!pScopeSymbol)
	//		{
	//			pScopeSymbol = m_pSymbolTableMgr->findSymbol(name, 1);
	//		}
	//	}
	//	if (pScopeSymbol)
	//	{
	//		if (pScopeSymbol->isconst == 1)
	//		{
	//			m_pErrorHandler->push_err_list('h', mCurToken.lineNum);
	//		}
	//	}
	//}
}


bool Parser::parseErrorI()
{
	if (mCurToken.word != ";") {
		m_pErrorHandler->push_err_list('i', mLastNonT);
		return true;;
	}
	else {
		//printcurToken();
		AdvanceToNextToken();
	}
	return false;
}

bool Parser::parseErrorJ()
{
	if (mCurToken.word != ")") {
		m_pErrorHandler->push_err_list('j', mLastNonT);
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
		m_pErrorHandler->push_err_list('k', mLastNonT);
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
		m_pErrorHandler->push_err_list('l', printlinenum);
	}
}

void Parser::parseErrorM()
{
	if (!isBorC) {
		m_pErrorHandler->push_err_list('m', mCurToken.lineNum);
	}
}

////////////////////////////////////////////////////////////////////

class Interpreter
{
public:
	Interpreter();
	std::vector<std::string> inter(const std::vector<std::shared_ptr<PCode>>& mCodelist);
private:
	std::array<int, 100001> mDStack;
	int BAddr = 0;
	int at = 0;
	int sp = -1;
	int mainBAddr = 0;
};


Interpreter::Interpreter()
{
	mDStack.fill(0);
}

std::vector<std::string> Interpreter::inter(const std::vector<std::shared_ptr<PCode>>& mCodelist)
{
	int addr;
	std::vector<std::string> print;
	while (at < mCodelist.size()) {
		auto pCurCode = mCodelist[at];

		if (pCurCode->getLabel() != nullptr) {
			std::cout << "code.getLabel() = " << pCurCode->getLabel()->GetAddr() << std::endl;
		}

		if (pCurCode->getName() == "INT")
		{
			sp += pCurCode->getaddr();
			at++;
		}
		else if (pCurCode->getName() == "INT_L")
		{
			sp += pCurCode->getLabel()->GetAddr();
			at++;
		}
		else if (pCurCode->getName() == "DOWN")
		{
			sp -= pCurCode->getaddr();
			at++;
		}
		else if (pCurCode->getName() == "LOD")
		{
			sp++;
			if (pCurCode->getScope() == 0) {
				addr = BAddr + pCurCode->getaddr();
			}
			else {
				addr = pCurCode->getaddr();
			}
			mDStack[sp] = mDStack[addr];
			if (mainBAddr == BAddr) {
				mDStack[sp] = addr;
			}
			at++;
		}
		else if (pCurCode->getName() == "LODS")
		{
			mDStack[sp] = mDStack[mDStack[sp]];
			at++;
		}
		else if (pCurCode->getName() == "LOD_A")
		{
			bool iscasearr = false;
			sp++;
			if (pCurCode->getScope() == 0) {
				addr = BAddr + pCurCode->getaddr();
			}
			else if (pCurCode->getScope() == 1) {
				addr = pCurCode->getaddr();
			}
			else {
				addr = BAddr + pCurCode->getaddr();
				iscasearr = true;
			}
			if (!iscasearr) {
				mDStack[sp] = addr;
			}
			else {
				mDStack[sp] = mDStack[addr];
			}
			//cout << "value == " << dstack[dstack[sp]] << endl;
			at++;
		}
		else if (pCurCode->getName() == "LOD_C")
		{
			sp++;
			mDStack[sp] = pCurCode->getaddr();
			at++;
		}
		else if (pCurCode->getName() == "STO")
		{
			sp--;
			mDStack[mDStack[sp]] = mDStack[sp + 1];
			sp--;
			at++;
		}
		else if (pCurCode->getName() == "JMP")
		{
			BAddr = sp + 1;
			mainBAddr = BAddr;
			at = pCurCode->getLabel()->GetAddr();
		}
		else if (pCurCode->getName() == "RET")
		{
			at = mDStack[BAddr + 2];
			sp = BAddr;
			BAddr = mDStack[BAddr + 1];
		}
		else if (pCurCode->getName() == "CAL")
		{
			mDStack[sp + 1] = 0;
			mDStack[sp + 2] = BAddr;
			mDStack[sp + 3] = at + 1;
			BAddr = sp + 1;
			sp = sp + 3;
			at = pCurCode->getaddr();
		}
		else if (pCurCode->getName() == "ADD")
		{
			sp--;
			mDStack[sp] = mDStack[sp] + mDStack[sp + 1];
			at++;
		}
		// ++ 符号处理（新的）
		else if (pCurCode->getName() == "DADD") {
			sp--;
			mDStack[sp] = mDStack[sp + 1] + 1;
			at++;
		}
		else if (pCurCode->getName() == "SUB")
		{
			sp--;
			mDStack[sp] = mDStack[sp] - mDStack[sp + 1];
			at++;
		}
		else if (pCurCode->getName() == "MUL")
		{
			sp--;
			mDStack[sp] = mDStack[sp] * mDStack[sp + 1];
			at++;
		}
		// ** 符号处理
		else if (pCurCode->getName() == "DMULT")
		{
			sp--;
			int a = mDStack[sp];
			int b = mDStack[sp + 1];

			int base = a + b;
			int result = 1;
			result = pow(base, b);

			mDStack[sp] = result;
			at++;
		}

		else if (pCurCode->getName() == "DIV")
		{
			sp--;
			mDStack[sp] = mDStack[sp] / mDStack[sp + 1];
			at++;
		}
		else if (pCurCode->getName() == "MOD")
		{
			sp--;
			mDStack[sp] = mDStack[sp] % mDStack[sp + 1];
			at++;
		}
		else if (pCurCode->getName() == "MOD128")
		{
			mDStack[sp] = mDStack[sp] % 128;
			at++;
		}
		else if (pCurCode->getName() == "MINU")
		{
			mDStack[sp] = -mDStack[sp];
			at++;
		}
		else if (pCurCode->getName() == "GET")
		{
			sp++;
			std::cin >> mDStack[sp];
			at++;
		}
		else if (pCurCode->getName() == "GETC")
		{
			sp++;
			char input;
			std::cin >> input;
			mDStack[sp] = static_cast<int>(input);
			at++;
		}

		// ASCII转换过程也有
		else if (pCurCode->getName() == "PRF")
		{
			// 获取要处理的字符串
			std::string s = pCurCode->getPrint();
			// 去除字符串中的双引号
			s.erase(remove(s.begin(), s.end(), '\"'), s.end());
			// 统计格式指定符的数量并调整栈指针
			int cnt = count(s.begin(), s.end(), '%');
			sp = sp - cnt;
			// 替换格式指定符
			for (int i = 0; i < cnt; i++) {
				size_t pos = s.find('%');
				if (pos != std::string::npos) {
					if (s[pos + 1] == 'd') {
						s.replace(pos, 2, std::to_string(mDStack[sp + i + 1]));
					}
					// 转换为文字形式
					else if (s[pos + 1] == 'c') {
						char ch = static_cast<char>(mDStack[sp + i + 1] % 128);
						s.replace(pos, 2, std::string(1, ch));
					}
				}
			}

			at++;
			// 处理换行符
			size_t pos = 0;
			while ((pos = s.find("\\n", pos)) != std::string::npos)
			{
				s.replace(pos, 2, "\n");
				pos += 1;
			}

			print.push_back(s);
		}
		else if (pCurCode->getName() == "BGT") {
			sp--;
			mDStack[sp] = (mDStack[sp] > mDStack[sp + 1]) ? 1 : 0;
			at++;
		}
		else if (pCurCode->getName() == "BGE") {
			sp--;
			mDStack[sp] = (mDStack[sp] >= mDStack[sp + 1]) ? 1 : 0;
			at++;
		}
		else if (pCurCode->getName() == "BLT") {
			sp--;
			mDStack[sp] = (mDStack[sp] < mDStack[sp + 1]) ? 1 : 0;
			at++;
		}
		else if (pCurCode->getName() == "BLE") {
			sp--;
			mDStack[sp] = (mDStack[sp] <= mDStack[sp + 1]) ? 1 : 0;
			at++;
		}
		else if (pCurCode->getName() == "BEQ") {
			sp--;
			mDStack[sp] = (mDStack[sp] == mDStack[sp + 1]) ? 1 : 0;
			at++;
		}
		else if (pCurCode->getName() == "BNE") {
			sp--;
			mDStack[sp] = (mDStack[sp] != mDStack[sp + 1]) ? 1 : 0;
			at++;
		}
		else if (pCurCode->getName() == "BZT") {
			if (mDStack[sp] == 0) {
				at = pCurCode->getLabel()->GetAddr();
			}
			else {
				at++;
			}
			sp--;
		}
		else if (pCurCode->getName() == "J") {
			at = pCurCode->getLabel()->GetAddr();
		}
		else if (pCurCode->getName() == "JP0") {
			if (mDStack[sp] == 0) {
				at = pCurCode->getLabel()->GetAddr();
			}
			else {
				at++;
			}
		}
		else if (pCurCode->getName() == "JP1") {
			if (mDStack[sp] == 1) {
				at = pCurCode->getLabel()->GetAddr();
			}
			else {
				at++;
			}
		}
		else if (pCurCode->getName() == "NOT") {
			if (mDStack[sp] == 0) {
				mDStack[sp] = 1;
			}
			else {
				mDStack[sp] = 0;
			}
			at++;
		}

		else if(pCurCode->getName() == "bitand"){
			sp--;
			mDStack[sp] = mDStack[sp] & mDStack[sp+1];
			at++;
		}

		else {
			at++;
		}
		if (sp < mDStack.size())
			std::cout << "sp == " << sp << "  " << "sp value == " << mDStack[sp] << "  baddr == " << BAddr << "\n\n";
	}
	return print;
}

int main()
{
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

	// 可以把return省略
	if (pErrorHandler && pErrorHandler->HasError())
	{
		pErrorHandler->print_err_list("error.txt");
		return 0;
	}

	//// 解释执行
	Interpreter interpreter;
	const std::vector<std::string>& output = interpreter.inter(pParser->GetCodeList());
	{
		std::fstream outFile("pcoderesult.txt", std::ios::trunc | std::ios::out);
		if (outFile.good())
		{
			for (auto& code : output)
			{
				outFile << code;
				std::cout << code;
			}
		}
		outFile.flush();
		outFile.close();
	}

	return 0;
}