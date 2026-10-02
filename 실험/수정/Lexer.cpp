#include "Lexer.h"
#include <cctype>
#include <iostream>
#include <fstream>
//#include <assert.h>

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
	{"[", LBRACK}, {"]", RBRACK}, {"{", LBRACE}, {"}", RBRACE}, {"**", DMULT}
};

Lexer::Lexer(std::string_view filePath) : mCurpos(0), mCurrentLinenum(1)
{
	mInputFilePath = filePath;

	std::ifstream inputFile(filePath.data());
	if (!inputFile.is_open()) 
	{
		// assert(false);
		return;
	}
	mInputText = std::string((std::istreambuf_iterator<char>(inputFile)), std::istreambuf_iterator<char>());
	inputFile.close();

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
	case MINU: return "MINU";
	case SEMICN: return "SEMICN";
	case COMMA: return "COMMA";
	case LPARENT: return "LPARENT";
	case RPARENT: return "RPARENT";
	case LBRACK: return "LBRACK";
	case RBRACK: return "RBRACK";
	case LBRACE: return "LBRACE";
	case RBRACE: return "RBRACE";
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
	if (tokenStr == "MINU") return MINU;
	if (tokenStr == "SEMICN") return SEMICN;
	if (tokenStr == "COMMA") return COMMA;
	if (tokenStr == "LPARENT") return LPARENT;
	if (tokenStr == "RPARENT") return RPARENT;
	if (tokenStr == "LBRACK") return LBRACK;
	if (tokenStr == "RBRACK") return RBRACK;
	if (tokenStr == "LBRACE") return LBRACE;
	if (tokenStr == "RBRACE") return RBRACE;
	// 如果没有匹配的字符串，可根据需求进行错误处理，这里简单返回默认值
	// assert(false);
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

				PushTokenList( DIV, "/", mCurrentLinenum);
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

			// 키워드인지 판단
			if (mKeywords.find(ident) != mKeywords.end())
			{
				PushTokenList( mKeywords[ident], ident, mCurrentLinenum);
			}
			else
			{
				//HRLog::Instance()->LogError(std::format("unknow keyword:{}", ident), __FUNCTION__, __LINE__);
				// 식별자 토큰 추가
				PushTokenList( IDENFR, ident, mCurrentLinenum);
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
			PushTokenList( INTCON, number, mCurrentLinenum);
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
				PushTokenList( STRCON, strConst, mCurrentLinenum);
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
				PushTokenList( CHRCON, charConst, mCurrentLinenum);
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


		// 연산자 및 구분자 처리
		else {
			std::string op(1, currentChar);
			char nextChar = inputFile.peek();

			if (currentChar == '&') {

				if (nextChar == '&') {
					op += inputFile.get();
					PushTokenList( AND, op, mCurrentLinenum);
				}

				else {
					// mErrors.push_back({ lineNumber, 'a' });
					//assert(false);
					PushTokenList(EAND, "&", mCurrentLinenum );
					continue;
				}
			}

			else if (currentChar == '|') {

				if (nextChar == '|') {
					op += inputFile.get();
					PushTokenList( OR, op, mCurrentLinenum);
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
				PushTokenList( mOperators[op], op, mCurrentLinenum);
			}

			else if (mOperators.find(op) != mOperators.end())
			{
				PushTokenList( mOperators[op], op, mCurrentLinenum);
			}
		}

		if (inputFile.eof()) break; // 파일 끝에 도달 시 종료
	}

	inputFile.close();
}

const std::vector<TokenWord>& Lexer::GetTokenList() const
{
    return mTokens;
}