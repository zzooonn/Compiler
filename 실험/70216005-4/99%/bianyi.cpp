#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <stack>
#include <cctype> 
#include <algorithm>
#include <unordered_set>

using namespace std;

enum TokenType {
    IDENFR, INTCON, STRCON, CHRCON, MAINTK, CONSTTK, INTTK, CHARTK, BREAKTK,
    CONTINUETK, IFTK, ELSETK, FORTK, GETINTTK, GETCHARTK, PRINTFTK, RETURNTK,
    VOIDTK, NOT, AND, OR, MULT, DIV, MOD, LSS, LEQ, GRE, GEQ, EQL, NEQ, ASSIGN,
    PLUS, MINU, SEMICN, COMMA, LPARENT, RPARENT, LBRACK, RBRACK, LBRACE, RBRACE
};

unordered_map<string, TokenType> keywords = {
    {"const", CONSTTK}, {"int", INTTK}, {"char", CHARTK}, {"break", BREAKTK},
    {"continue", CONTINUETK}, {"if", IFTK}, {"else", ELSETK}, {"for", FORTK},
    {"getint", GETINTTK}, {"getchar", GETCHARTK}, {"printf", PRINTFTK},
    {"return", RETURNTK}, {"void", VOIDTK}, {"main", MAINTK}
};

unordered_map<string, TokenType> operators = {
    {"+", PLUS}, {"-", MINU}, {"*", MULT}, {"/", DIV}, {"%", MOD},
    {"<", LSS}, {"<=", LEQ}, {">", GRE}, {">=", GEQ}, {"==", EQL},
    {"!=", NEQ}, {"=", ASSIGN}, {"&&", AND}, {"||", OR}, {"!", NOT},
    {";", SEMICN}, {",", COMMA}, {"(", LPARENT}, {")", RPARENT},
    {"[", LBRACK}, {"]", RBRACK}, {"{", LBRACE}, {"}", RBRACE}
};

struct Token {
    TokenType type;
    string value;
    int lineNumber;
};

struct Error {
    int lineNumber;
    char errorcode;
};

struct Symbol {
    int scopelineNumber = 0;
    string name;
    string type;
    bool isFunction = false;    // 함수 여부
};

// 함수
void lexicalAnalysis(const string& filename);
void outputToken(const Token& token);
void advanceToken();
bool parseExp();
void parseNumber();
void parseCharacter();
bool parseLVal();
bool parsePrimaryExp();
size_t parseFuncRParams(vector<string>& passedParamTypes);
void parseCond();
void parseUnaryOp();
bool parseUnaryExp();
void parseConstExp();
bool parseMulExp();
bool parseAddExp();
void parseRelExp();
void parseEqExp();
void parseLAndExp();
void parseLOrExp();
void parseForStmt();
bool parseStmt();
void parseFuncType();
void parseBType();
void parseConstDecl();
void parseConstDef();
string parseFuncFParam(string& paramType);
void parseFuncDef();
size_t parseFuncFParams(vector<string>& paramTypes, const string& funcName);
void parseMainFuncDef();
void parseConstInitVal();
void parseVarDecl();
void parseVarDef(const string& baseType);
void parseInitVal();
bool parseBlockItem();
bool parseBlock();
void parseDecl();
void parseCompUnit();

// 전역 변수 선언
vector<Token> tokens;
vector<Error> errors;
size_t tokenIndex = 0;
int lineNumber = 1;
ofstream outputFile;
bool inConditionalContext = false;

vector<Symbol> symbolTable;
stack<int> scopeStack;  // 스코프 번호 스택 
int CurrentScope = 1;
int NextScope = 2;
string currentBaseType;
int scopeCounter = 1;
unordered_map<string, size_t> functionParams;
string currentReturnType; // 현재 리턴 타입 저장 
bool inLoopContext = false;
unordered_map<string, vector<string>> functionParamsTypes; // 함수 이름과 매개변수 타입을 저장
vector<string> passedParamTypes;  // 전달된 매개변수의 타입을 저장하는 벡터
unordered_set<int> errorLines; // 이미 오류가 보고된 줄을 추적
unordered_map<int, int> scopeParent;
stack<string> returnTypeStack;

string tokenTypeToString(TokenType type) {
    switch (type) {
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
    case OR: return "OR";
    case MULT: return "MULT";
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

string SymbolType(bool isConst, const string& baseType, bool isArray, bool isFunc) {
    string typeName = "";

    if (isConst) {
        typeName += "Const";
    }

    if (isFunc) {
        if (baseType == "void") {
            typeName += "VoidFunc";
        }
        else if (baseType == "Int") {
            typeName += "IntFunc";
        }
        else if (baseType == "Char") {
            typeName += "CharFunc";
        }
    }
    else {
        if (isArray) {
            if (baseType == "Int") {
                typeName += "IntArray";
            }
            else if (baseType == "Char") {
                typeName += "CharArray";
            }
        }
        else {
            if (baseType == "Int") {
                typeName += "Int";
            }
            else if (baseType == "Char") {
                typeName += "Char";
            }
        }
    }
    return typeName;
}

// 심볼 추가 함수
void addSymbol(const string& name, const string& type, bool isFunction = false) {
    Symbol sym;
    sym.name = name;
    sym.type = type;
    sym.scopelineNumber = CurrentScope;
    sym.isFunction = isFunction;
    symbolTable.push_back(sym);

    cout << "Symbol added: " << sym.name << " | Type: " << sym.type << " | Scope: " << sym.scopelineNumber << " | IsFunction: " << (isFunction ? "Yes" : "No") << endl;
}

void enterScope() {
    scopeCounter++;
    scopeParent[scopeCounter] = CurrentScope; // 새로운 스코프의 부모 설정
    scopeStack.push(CurrentScope);
    CurrentScope = scopeCounter;
}

void exitScope() {
    if (!scopeStack.empty()) {
        CurrentScope = scopeStack.top();
        scopeStack.pop();
    }
}

// 현재 위치의 토큰을 가져옴
Token getCurrentToken() {
    if (tokenIndex < tokens.size()) return tokens[tokenIndex];
    else exit(1);
}

// 이전 토큰을 반환하는 함수
Token getPreviousToken() {
    if (tokenIndex > 0 && tokenIndex - 1 < tokens.size())
        return tokens[tokenIndex - 1];
    else
        return Token{ IDENFR, "", 0 }; // 기본값 반환 (필요에 따라 수정 가능)
}

// 해당 줄에 이미 오류가 보고되었는지 확인( 에러는 한 줄에 한 개의 에러만 존재 )
void addError(int lineNumber, char errorcode) {
    if (errorLines.find(lineNumber) == errorLines.end()) {
        errors.push_back({ lineNumber, errorcode });
        errorLines.insert(lineNumber);
        // 에러 추가 성공 시 디버그 출력
        cout << "DEBUG: Error '" << errorcode << "' added at line " << lineNumber << endl;
    }
    else {
        // 이미 에러가 추가된 경우 디버그 출력
        cout << "DEBUG: Error not added because line " << lineNumber << " already has an error." << endl;
    }
}

// 변수 이름 중복 오류 
void parseErrorPrintB(const Token& token) {
    addError(token.lineNumber, 'b');
}

// 현재 스코프에 동일한 심볼이 있는지
bool isSymbolDefinedInCurrentScope(const string& name) {
    // 심볼 테이블을 역순으로 검색
    for (vector<Symbol>::reverse_iterator it = symbolTable.rbegin(); it != symbolTable.rend(); ++it) {
        if (it->name == name && it->scopelineNumber == CurrentScope) {
            return true;
        }
    }
    return false;
}

string getSymbolType(const string& name) {
    int scope = CurrentScope;
    while (scope != 0) { // 전역 스코프가 0이라고 가정
        for (auto it = symbolTable.rbegin(); it != symbolTable.rend(); ++it) {
            if (it->name == name && it->scopelineNumber == scope) {
                return it->type;
            }
        }
        if (scopeParent.find(scope) != scopeParent.end()) {
            scope = scopeParent[scope]; // 부모 스코프로 이동
        }
        else {
            break;
        }
    }
    return ""; // 찾지 못한 경우 빈 문자열 반환
}

// 식별자가 있는지 판단
bool isSymbolDefined(const string& name) {
    int scope = CurrentScope;
    while (scope != 0) { // 전역 스코프가 0이라고 가정
        for (auto it = symbolTable.rbegin(); it != symbolTable.rend(); ++it) {
            if (it->name == name && it->scopelineNumber == scope) {
                return true;
            }
        }
        if (scopeParent.find(scope) != scopeParent.end()) {
            scope = scopeParent[scope]; // 부모 스코프로 이동
        }
        else {
            break;
        }
    }
    return false;
}


// 상수 여부 확인 
bool isConstLVal(const string& name) {
    // 현재 스코프부터 바깥쪽 스코프까지 확인
    for (vector<Symbol>::reverse_iterator it = symbolTable.rbegin(); it != symbolTable.rend(); ++it) {
        if (it->name == name && it->scopelineNumber <= CurrentScope) {
            if (it->type.find("Const") != string::npos) {
                return true;
            }
            return false;
        }
    }
    return false;
}

// 상위 스코프에서도 중복된 변수가 있는지 확인하는 함수 추가
bool isSymbolDefinedInAnyScope(const string& name) {
    // 심볼 테이블을 역순으로 검색하여 상위 스코프에서도 중복된 변수가 있는지 확인
    for (vector<Symbol>::reverse_iterator it = symbolTable.rbegin(); it != symbolTable.rend(); ++it) {
        if (it->name == name) {
            return true;
        }
    }
    return false;
}

// 변수 식별자 없는 경우 
void parseErrorPrintC(const Token& token) {
    addError(token.lineNumber, 'c');
}

// 심볼 추가 시 중복 검사 후 추가 또는 오류 처리
void addSymbolWithCheck(const Token& token, const string& type, bool isFunction = false) {
    const string& name = token.value;

    // 현재 스코프에서만 중복된 이름이 있는지 확인
    if (isSymbolDefinedInCurrentScope(name)) {
        parseErrorPrintB(token);
    }
    else {
        Symbol sym;
        sym.name = name;
        sym.type = type;
        sym.scopelineNumber = CurrentScope; // 현재 스코프 번호 저장
        sym.isFunction = isFunction;
        symbolTable.push_back(sym);

        cout << "Symbol added: " << sym.name << " | Type: " << sym.type
            << " | Scope: " << sym.scopelineNumber
            << " | IsFunction: " << (isFunction ? "Yes" : "No") << endl;
    }
}


void parseErrorPrintD(const Token& token) {
    addError(token.lineNumber, 'd');
}

void parseErrorPrintE() {
    Token token = getCurrentToken();
    addError(token.lineNumber, 'e');
}

void recoverAndValidateReturn() {
    Token token = getCurrentToken();
    Token previousToken = getPreviousToken();

    // void 함수에서 반환값이 있을 때 에러 'f' 추가
    if (currentReturnType == "void") {
        if (token.type == INTCON || token.type == CHRCON) {
            addError(previousToken.lineNumber, 'f');  // void 함수에서 잘못된 반환 값이 있을 때
        }
    }

    // 모든 반환 타입에서 세미콜론이 없는 경우 에러 'i' 추가
    if (token.type != SEMICN) {
        outputFile << "SEMICN ;" << endl;
        addError(previousToken.lineNumber, 'i');  // 누락된 세미콜론 에러 'i'
    }
}

// Const 삭제 후 비교
string EraseConst(const string& type) {
    string basetype = type;
    string prefix = "Const";

    if (basetype.find(prefix) == 0) {
        basetype.erase(0, prefix.length()); // 'Const' 제거
        cout << "EraseConst: Original type = \"" << type << "\", Erased type = \"" << basetype << "\"" << endl;
    }

    return basetype;
}

// 타입 호환성 검사 - 상수 여부와 상관없이 비교
bool areTypesCompatible(const string& expectedType, const string& passedType) {
    string expType = EraseConst(expectedType);
    string pasType = EraseConst(passedType);

    cout << "Comparing types: Expected = \"" << expType << "\", Passed = \"" << pasType << "\"" << endl;

    // Const가 제거된 기본 타입이 일치하면 true 반환
    if (expType == pasType) {
        return true;
    }

    // 배열의 경우도 Const를 제거한 상태로 비교
    if ((expType == "IntArray" && pasType == "IntArray") ||
        (expType == "CharArray" && pasType == "CharArray")) {
        return true;
    }

    // 모든 다른 경우에는 호환되지 않음
    return false;
}

void parseErrorPrintF(const Token& token) {
    addError(token.lineNumber, 'f');
}


void parseErrorPrintG(const Token& token) {
    addError(token.lineNumber, 'g');
}

// const 
void parseErrorPrintH() {
    Token token = getCurrentToken();
    addError(token.lineNumber, 'h');
}

// ; 누락 
void parseErrorPrintfI() {
    Token currentToken = getCurrentToken();
    Token previousToken = getPreviousToken(); // 이전 토큰을 가져오는 함수 필요

    if (currentToken.type == SEMICN) {
        outputToken(currentToken);
        advanceToken();
    }
    else {
        addError(previousToken.lineNumber, 'i');
    }
}

// ) 누락
void parseErrorPrintfJ() {
    Token token = getCurrentToken();
    Token previousToken = getPreviousToken(); // 이전 토큰을 가져오는 함수 필요

    if (token.type == RPARENT) {
        outputToken(token);
        advanceToken();
    }

    else {
        addError(previousToken.lineNumber, 'j');
    }
}

// ] 누락
void parseErrorPrintfK() {
    Token token = getCurrentToken();

    if (token.type == RBRACK) {
        outputToken(token);
        advanceToken();
    }

    else {
        addError(token.lineNumber, 'k');
    }
}

void parseErrorPrintL() {
    Token token = getCurrentToken();
    addError(token.lineNumber, 'l');
}

void parseErrorPrintM() {
    Token token = getCurrentToken();
    addError(token.lineNumber, 'm');
}

bool isExpressionStartToken(TokenType type) {
    return (type == IDENFR || type == INTCON || type == CHRCON ||
        type == LPARENT || type == PLUS || type == MINU || type == NOT);
}


// 어휘 분석구문
void lexicalAnalysis(const string& filename) {

    ifstream inputFile(filename);
    char currentChar;

    while (inputFile.get(currentChar)) {

        if (isspace(currentChar)) {
            if (currentChar == '\n') {
                lineNumber++;
            }
            continue;
        }

        // 주석 처리 및 나누기 처리
        else if (currentChar == '/') {
            char nextChar = inputFile.peek();

            // 주석인데 /*로 시작할 경우
            if (nextChar == '*') {

                inputFile.get(); // '*' 소비
                bool endofComment = false;

                while (inputFile.get(currentChar)) {
                    if (currentChar == '\n') {
                        lineNumber++;
                    }

                    else if (currentChar == '*') {
                        if (inputFile.peek() == '/') {
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
            else if (nextChar == '/') {
                inputFile.get(); // 두 번째 '/' 소비

                while (inputFile.get(currentChar)) {
                    if (currentChar == '\n') {
                        lineNumber++; // 줄 번호 증가
                        break;
                    }

                    if (inputFile.eof()) break; // 파일 끝에 도달 시 종료
                }
                continue;
            }

            // 나눗셈인 경우
            else {
                tokens.push_back({ DIV, "/", lineNumber });
            }
        }

        // 식별자 처리
        else if (isalpha(currentChar) || currentChar == '_')
        {
            string ident;   // 식별자 선언 
            ident += currentChar;

            while (inputFile.peek() != EOF && (isalnum(inputFile.peek()) || inputFile.peek() == '_'))
            {
                ident += inputFile.get();
            }

            // 키워드인지 판단
            if (keywords.find(ident) != keywords.end())
            {
                tokens.push_back({ keywords[ident], ident, lineNumber });
            }
            else
            {
                // 식별자 토큰 추가
                tokens.push_back({ IDENFR, ident, lineNumber });
            }
        }

        // 숫자 처리
        else if (isdigit(currentChar)) {
            string number;
            number += currentChar;

            // EOF 체크 추가
            while (isdigit(inputFile.peek())) {
                number += inputFile.get();
            }
            tokens.push_back({ INTCON, number, lineNumber });
        }

        // 문자열 상수 처리
        else if (currentChar == '"') {
            string strConst;
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
                    lineNumber++;
                    break;
                }

                if (inputFile.eof()) break; // 파일 끝에 도달 시 종료
            }

            if (closed) {
                tokens.push_back({ STRCON, strConst, lineNumber });
            }
            else {
                errors.push_back({ lineNumber, 'a' });
            }
        }

        // 문자 상수 처리
        else if (currentChar == '\'') {

            string charConst;
            charConst += currentChar;       // 첫 번째 따옴표 추가
            char c = inputFile.get();
            charConst += c;

            if (c == '\\') // 이스케이프 시퀀스 시작
            {
                char nextChar = inputFile.get();
                charConst += nextChar; // 이스케이프 시퀀스의 두 번째 문자 추가
                c = nextChar; // c를 업데이트하여 다음 검사에서 사용
            }

            if (inputFile.peek() == '\'') {
                charConst += inputFile.get(); // 종료 따옴표 추가
                tokens.push_back({ CHRCON, charConst, lineNumber });
            }
            else {
                // 오류 발생 시 errors 벡터에 추가
                errors.push_back({ lineNumber, 'a' }); // 오류 코드 설정
            }
        }

        // 연산자 및 구분자 처리
        else {
            string op(1, currentChar);
            char nextChar = inputFile.peek();

            if (currentChar == '&') {

                if (nextChar == '&') {
                    op += inputFile.get();
                    tokens.push_back({ AND, op, lineNumber });
                }

                else {
                    errors.push_back({ lineNumber, 'a' });
                    tokens.push_back({ AND, "&&", lineNumber });
                    continue;
                }
            }

            else if (currentChar == '|') {

                if (nextChar == '|') {
                    op += inputFile.get();
                    tokens.push_back({ OR, op, lineNumber });
                }

                else {
                    errors.push_back({ lineNumber, 'a' });
                    tokens.push_back({ AND, "&&", lineNumber });
                    continue;
                }
            }

            else if ((currentChar == '!' || currentChar == '=' || currentChar == '<' || currentChar == '>') && nextChar == '=') {
                op += inputFile.get();
                tokens.push_back({ operators[op], op, lineNumber });
            }

            else if (operators.find(op) != operators.end()) {
                tokens.push_back({ operators[op], op, lineNumber });
            }
        }

        if (inputFile.eof()) break; // 파일 끝에 도달 시 종료
    }

    inputFile.close();
}

// 출력
void outputToken(const Token& token) {
    outputFile << tokenTypeToString(token.type) << " " << token.value << endl;
}

// %d, %c인 경우만 개수를 세는 함수
int countFormatSpecifiers(const string& formatString) {
    int count = 0;
    for (size_t i = 0; i < formatString.length(); ++i) {
        if (formatString[i] == '%' && i + 1 < formatString.length()) {
            // %%는 '%' 하나로 간주하여 카운트하지 않음
            if (formatString[i + 1] == '%') {
                i++;
                continue;
            }
            // %d 또는 %c인 경우에만 카운트 증가
            if (formatString[i + 1] == 'd' || formatString[i + 1] == 'c') {
                count++;
                i++;
            }
        }
    }
    return count;
}

// 토큰 인덱스 증가
void advanceToken() {
    if (tokenIndex < tokens.size())
    {
        tokenIndex++;
    }
}

// Number → IntConst 
void parseNumber() {
    Token token = getCurrentToken();

    if (token.type == INTCON) {
        if (token.value.length() > 1 && token.value[0] == '0') {
            return;
        }
        else {
            outputToken(token);
            currentBaseType = "int";
            advanceToken();
        }
    }
}

// Character → CharConst
void parseCharacter() {
    Token token = getCurrentToken();

    if (token.type == CHRCON) {
        char character = token.value[1];

        if ((character >= 32 && character <= 126) ||
            character == '\a' || character == '\b' || character == '\t' ||
            character == '\n' || character == '\v' || character == '\f' ||
            character == '\r' || character == '\\' || character == '\'' ||
            character == '\"')
        {
            outputToken(token);
            currentBaseType = "char";
            advanceToken();
        }
    }
    outputFile << "<Character>" << endl;
}

// LVal → Ident ['[' Exp ']'] // k
bool parseLVal() {
    Token token = getCurrentToken();

    if (token.type == IDENFR) {
        // 식별자가 선언되었는지 확인
        if (!isSymbolDefined(token.value)) {
            parseErrorPrintC(token);
        }

        outputToken(token);
        string varName = token.value;
        advanceToken();

        // 심볼 테이블에서 변수 타입 가져오기
        string varType = getSymbolType(varName);
        bool isArrayAccess = false;

        // 배열 요소인지 확인
        token = getCurrentToken();
        if (token.type == LBRACK) {
            outputToken(token);
            advanceToken();

            parseExp();

            token = getCurrentToken();
            if (token.type == RBRACK) {
                outputToken(token);
                advanceToken();
                isArrayAccess = true;
            }
            else {
                parseErrorPrintfK();
            }
        }

        // 현재 식의 타입 설정
        if (isArrayAccess) {
            if (varType == "IntArray") {
                currentBaseType = "Int";
            }
            else if (varType == "CharArray") {
                currentBaseType = "Char";
            }
            else {
                currentBaseType = EraseConst(varType);
            }
        }
        else {
            currentBaseType = varType;
        }

        outputFile << "<LVal>" << endl;
        return true;
    }

    else {
        return false; // LVal이 아님을 표시
    }
}

// LOrExp → LAndExp { '||' LAndExp }
void parseLOrExp() {

    parseLAndExp();
    outputFile << "<LOrExp>" << endl;

    if (getCurrentToken().type == OR) {
        outputToken(getCurrentToken());
        advanceToken();
        parseLOrExp();
    }

}

// LAndExp → EqExp { '&&' EqExp }
void parseLAndExp() {

    parseEqExp();
    outputFile << "<LAndExp>" << endl;

    if (getCurrentToken().type == AND) {
        outputToken(getCurrentToken());
        advanceToken();
        parseLAndExp();
    }

}

// UnaryOp → '+' | '?' | '!' 注：'!'?出?在?件表?式中 
void parseUnaryOp() {
    Token token = getCurrentToken();

    if (token.type == PLUS || token.type == MINU || token.type == NOT)
    {
        outputToken(token);
        advanceToken();
    }
    outputFile << "<UnaryOp>" << endl;
}

// UnaryExp → PrimaryExp | Ident '(' [FuncRParams] ')' | UnaryOp UnaryExp // j
bool parseUnaryExp() {
    Token token = getCurrentToken();

    // UnaryOp UnaryExp
    if (token.type == PLUS || token.type == MINU || token.type == NOT) {
        parseUnaryOp();
        return parseUnaryExp();
    }

    // Ident '(' [FuncRParams] ')'
    else if (getCurrentToken().type == IDENFR) {
        // 함수 호출인지 확인
        if (tokenIndex + 1 < tokens.size() && tokens[tokenIndex + 1].type == LPARENT) {
            if (!isSymbolDefined(token.value)) {
                parseErrorPrintC(token);
            }

            string currentFuncName = token.value; // 함수 이름을 지역 변수로 저장

            outputToken(getCurrentToken());
            advanceToken();
            outputToken(getCurrentToken());
            advanceToken();

            // 함수 호출 시 전달되는 매개변수 타입을 저장할 벡터 초기화
            vector<string> passedParamTypes;
            size_t paramCount = 0;

            // [FuncRParams] 처리
            if (getCurrentToken().type != RPARENT) {
                paramCount = parseFuncRParams(passedParamTypes);
            }

            // ')' 처리
            if (getCurrentToken().type == RPARENT) {
                outputToken(getCurrentToken());
                advanceToken();
            }
            else {
                parseErrorPrintfJ();
            }

            // 함수 정의와 매개변수 타입 비교
            if (functionParamsTypes.find(currentFuncName) != functionParamsTypes.end()) {
                const vector<string>& expectedParamTypes = functionParamsTypes[currentFuncName];

                // 매개변수 개수 비교
                if (paramCount != expectedParamTypes.size()) {
                    cout << "Param count mismatch: expected " << expectedParamTypes.size() << ", got " << paramCount << endl;
                    parseErrorPrintD(token); // 매개변수 개수 불일치
                }
                else {
                    bool typeMismatchFound = false;
                    for (size_t i = 0; i < paramCount; ++i) {
                        if (!areTypesCompatible(expectedParamTypes[i], passedParamTypes[i])) {
                            cout << "Param type mismatch at index " << i << ": expected " << expectedParamTypes[i] << ", got " << passedParamTypes[i] << endl;
                            parseErrorPrintE(); // 매개변수 타입 불일치 에러
                            typeMismatchFound = true;
                            break;
                        }
                    }

                    if (!typeMismatchFound) {
                        // 디버깅용 메시지 - 매개변수 개수와 타입 모두 정상
                        cout << "Function call is valid: " << currentFuncName << "(";
                        for (size_t i = 0; i < paramCount; ++i) {
                            cout << passedParamTypes[i];
                            if (i < paramCount - 1) {
                                cout << ", ";
                            }
                        }
                        cout << ")" << endl;
                    }
                }
            }
        }
        else {
            return parsePrimaryExp();
        }
    }
    else {
        return parsePrimaryExp();
    }

    outputFile << "<UnaryExp>" << endl;
    return true;
}

// Exp → AddExp 
bool parseExp() {
    if (!parseAddExp()) {
        // Exp 함수는 오류를 추가하지 않음
        return false;
    }
    outputFile << "<Exp>" << endl;
    return true;
}


// AddExp → MulExp | AddExp ('+' | '?') MulExp 
bool parseAddExp() {

    if (!parseMulExp()) {
        return false;
    }
    outputFile << "<AddExp>" << endl;

    if (getCurrentToken().type == PLUS || getCurrentToken().type == MINU) {
        outputToken(getCurrentToken());
        advanceToken();

        if (!isExpressionStartToken(getCurrentToken().type)) {
            // 표현식 누락 감지, 실패 반환
            return false;
        }

        parseAddExp();
    }

    return true;
}

// PrimaryExp → '(' Exp ')' | LVal | Number | Character // j
bool parsePrimaryExp() {

    // '(' Exp ')'
    if (getCurrentToken().type == LPARENT) {
        outputToken(getCurrentToken());  // '(' 출력
        advanceToken();

        if (!parseExp()) { 
            return false; 
        }

        // ')' 확인
        if (getCurrentToken().type == RPARENT) {
            outputToken(getCurrentToken());
            advanceToken();
        }
        else {
            parseErrorPrintfJ();
            return false;
        }
    }

    // LVal 처리
    else if (getCurrentToken().type == IDENFR) {
        parseLVal();
    }

    // Number 처리
    else if (getCurrentToken().type == INTCON) {
        outputToken(getCurrentToken());
        currentBaseType = "Int";  // 상수는 Int로 처리
        advanceToken();
        outputFile << "<Number>" << endl;
    }

    // Character 처리
    else if (getCurrentToken().type == CHRCON) {
        outputToken(getCurrentToken());
        currentBaseType = "Char";  // 상수는 Char로 처리
        advanceToken();
        outputFile << "<Character>" << endl;
    }

    else {
        return false; // Not a valid PrimaryExp start token
    }

    outputFile << "<PrimaryExp>" << endl;
    return true;
}

// MulExp →  MulExp → UnaryExp | MulExp ('*' | '/' | '%') UnaryExp
bool  parseMulExp() {

    if (!parseUnaryExp()) {
        return false; // UnaryExp failed
    }
    outputFile << "<MulExp>" << endl;

    if (getCurrentToken().type == MULT || getCurrentToken().type == DIV || getCurrentToken().type == MOD) {
        outputToken(getCurrentToken());
        advanceToken();

        parseMulExp();
    }

    return true;
}

// RelExp → AddExp | RelExp ('<' | '>' | '<=' | '>=') AddExp 
void parseRelExp() {

    parseAddExp();
    outputFile << "<RelExp>" << endl;

    if (getCurrentToken().type == LSS || getCurrentToken().type == LEQ || getCurrentToken().type == GRE || getCurrentToken().type == GEQ) {
        outputToken(getCurrentToken());
        advanceToken();
        parseRelExp();
    }

}

// EqExp → RelExp | EqExp ('==' | '!=') RelExp 
void parseEqExp() {

    parseRelExp();
    outputFile << "<EqExp>" << endl;

    if (getCurrentToken().type == EQL || getCurrentToken().type == NEQ) {
        outputToken(getCurrentToken());
        advanceToken();
        parseEqExp();
    }
}

// ForStmt → LVal '=' Exp 
void parseForStmt() {

    string lvalName = getCurrentToken().value;
    parseLVal();

    if (getCurrentToken().type == ASSIGN)
    {
        outputToken(getCurrentToken());
        advanceToken();
    }
    else {
        advanceToken();
    }

    if (isConstLVal(lvalName)) {
        parseErrorPrintH();  // 상수 값을 변경하려고 할 때 오류 'h' 추가
    }

    parseExp();

    while (getCurrentToken().type == COMMA) {
        outputToken(getCurrentToken());
        advanceToken();

        lvalName = getCurrentToken().value;
        parseLVal();

        if (getCurrentToken().type == ASSIGN) {
            outputToken(getCurrentToken());
            advanceToken();
        }
        else {
            advanceToken();
        }

        // if (isConstLVal(lvalName)) {
        //     parseErrorPrintH();
        // }

        parseExp();
    }
    outputFile << "<ForStmt>" << endl;
}

// BType → 'int' | 'char' 
void parseBType() {
    Token token = getCurrentToken();

    if (token.type == INTTK || token.type == CHARTK)
    {
        if (token.type == INTTK) {
            currentBaseType = "Int";
        }
        else {
            currentBaseType = "Char";
        }

        outputToken(token);
        advanceToken();
        token = getCurrentToken();
    }
}

// ConstDecl → 'const' BType ConstDef { ',' ConstDef } ';'  // i
void parseConstDecl() {

    if (getCurrentToken().type == CONSTTK) {
        outputToken(getCurrentToken());
        advanceToken();
    }

    parseBType();
    parseConstDef();

    while (getCurrentToken().type == COMMA) {
        outputToken(getCurrentToken());
        advanceToken();
        parseConstDef();
    }

    if (getCurrentToken().type == SEMICN) {
        outputToken(getCurrentToken());
        advanceToken();
    }
    else {
        parseErrorPrintfI();
    }

    outputFile << "<ConstDecl>" << endl;
}

// Decl → ConstDecl | VarDecl 
void parseDecl() {

    Token token = getCurrentToken();

    if (token.type == CONSTTK) {
        parseConstDecl();
    }
    else if (token.type == INTTK || token.type == CHARTK) {
        parseVarDecl();
    }
}

// Block → '{' { BlockItem } '}'
bool parseBlock() {

    Token token = getCurrentToken();
    bool hasReturn = false;

    // '{'
    if (token.type == LBRACE) {
        outputToken(token);
        advanceToken();
        enterScope();
    }

    // { BlockItem }
    while (getCurrentToken().type != RBRACE) {
        bool stmthasReturn = parseBlockItem();

        if (!hasReturn) {
            hasReturn = stmthasReturn;
        }
    }

    // '}'
    token = getCurrentToken();
    if (getCurrentToken().type == RBRACE) {
        outputToken(getCurrentToken());
        advanceToken();
        exitScope();
    }

    outputFile << "<Block>" << endl;
    return hasReturn;
}

// BlockItem → Decl | Stmt
bool parseBlockItem() {
    Token token = getCurrentToken();

    if (token.type == CONSTTK || token.type == INTTK || token.type == CHARTK) {
        parseDecl();
        return false;
    }

    else {
        return parseStmt();
    }
}

// FuncFParam → BType Ident ['[' ']'] // k
string parseFuncFParam(string& paramType) {

    parseBType();
    Token token = getCurrentToken();
    string paramName;
    paramType = currentBaseType; // 매개변수의 기본 타입 저장

    if (token.type == IDENFR) {
        paramName = token.value;
        outputToken(token);
        advanceToken();
    }

    bool isArray = false;
    if (getCurrentToken().type == LBRACK) {
        outputToken(getCurrentToken());
        advanceToken();

        token = getCurrentToken();
        if (token.type == RBRACK) {
            outputToken(token);
            advanceToken();
            isArray = true;
            paramType += "Array";
        }
        else {
            parseErrorPrintfK();
        }
    }

    // 중복 검사
    if (isSymbolDefinedInCurrentScope(paramName)) {
        parseErrorPrintB(token);
    }
    else {
        string symbolType = SymbolType(false, currentBaseType, isArray, false);
        Token identToken = { IDENFR, paramName, token.lineNumber };
        addSymbolWithCheck(identToken, symbolType);
    }

    outputFile << "<FuncFParam>" << endl;

    return paramName;
}



// Stmt
bool parseStmt() {
    Token token = getCurrentToken();

    bool isReturn = false;

    // 'break' ';' // i
    if (token.type == BREAKTK) {

        if (!inLoopContext) {
            parseErrorPrintM();
        }

        outputToken(token);
        advanceToken();
        token = getCurrentToken();

        if (token.type == SEMICN) {
            outputToken(token);
            advanceToken();
        }
        else {
            parseErrorPrintfI();
        }
    }

    // 'continue' ';' // i
    else if (token.type == CONTINUETK) {

        if (!inLoopContext) {
            parseErrorPrintM();
        }

        outputToken(token);
        advanceToken();

        if (getCurrentToken().type == SEMICN) {
            outputToken(getCurrentToken());
            advanceToken();
        }
        else {
            parseErrorPrintfI();
        }
    }

    // 'return' [Exp] ';' 처리
    else if (token.type == RETURNTK) {

        outputToken(token);
        advanceToken();

        token = getCurrentToken(); // 다음 토큰 가져오기

        if (token.type == SEMICN) {
            // 'return;' 형태, 반환값 없음
            outputToken(token);
            advanceToken();
        }
        else if (isExpressionStartToken(token.type)) {
            // 'return' 뒤에 표현식이 오는 경우
            if (currentReturnType == "void") {
                parseErrorPrintF(token);
            }
            parseExp();

            token = getCurrentToken(); // 표현식 파싱 후 토큰 업데이트
            if (token.type == SEMICN) {
                outputToken(token);
                advanceToken();
            }
            else {
                parseErrorPrintfI();
            }
        }
        else {
            // 'return' 뒤에 세미콜론이 없고 표현식 시작 토큰도 아닌 경우
            // 세미콜론 누락으로 간주하고 'i' 에러 발생
            parseErrorPrintfI();
        }

        isReturn = true;
    }


    // 'if' '(' Cond ')' Stmt [ 'else' Stmt ] // j
    else if (token.type == IFTK) {
        outputToken(token);
        advanceToken();

        if (getCurrentToken().type == LPARENT) {
            outputToken(getCurrentToken());
            advanceToken();
        }

        parseCond();  // Cond 처리

        if (getCurrentToken().type == RPARENT) {
            outputToken(getCurrentToken());
            advanceToken();
        }
        else {
            parseErrorPrintfJ();
        }

        parseStmt();  // 본문 Stmt 처리

        if (getCurrentToken().type == ELSETK) {
            outputToken(getCurrentToken());
            advanceToken();
            parseStmt();  // else 구문 처리
        }
    }

    // 'for' '(' [ForStmt] ';' [Cond] ';' [ForStmt] ')' Stmt
    else if (token.type == FORTK) {
        outputToken(token);
        advanceToken();

        // '(' 처리
        if (getCurrentToken().type == LPARENT) {
            outputToken(getCurrentToken());
            advanceToken();
        }

        // [ForStmt] 처리
        if (getCurrentToken().type != SEMICN) {
            parseForStmt();
        }

        // ';' 처리
        if (getCurrentToken().type == SEMICN) {
            outputToken(getCurrentToken());
            advanceToken();
        }
        else {
            parseErrorPrintfI();
        }

        // [Cond] 처리 
        if (getCurrentToken().type != SEMICN) {
            parseCond();
        }

        // ';' 처리
        if (getCurrentToken().type == SEMICN) {
            outputToken(getCurrentToken());
            advanceToken();
        }
        else {
            parseErrorPrintfI();
        }

        // [ForStmt] 처리
        if (getCurrentToken().type != RPARENT) {
            parseForStmt();
        }

        // ')' 처리
        if (getCurrentToken().type == RPARENT) {
            outputToken(getCurrentToken());
            advanceToken();
        }
        else {
            parseErrorPrintfJ();
        }

        bool previousLoopContext = inLoopContext;
        inLoopContext = true;

        parseStmt();

        inLoopContext = previousLoopContext;
    }

    // Stmt → ‘printf’‘(’ FormatString {, Exp}’)’ ‘;’
    else if (token.type == PRINTFTK) {
        cout << "DEBUG: Entering printf statement parsing" << endl;
        outputToken(token);
        advanceToken();

        if (getCurrentToken().type == LPARENT) {
            cout << "DEBUG: Found LPARENT '('" << endl;
            outputToken(getCurrentToken());
            advanceToken();
        }

        int formatSpecifierCount = 0;
        int expressionCount = 0;

        // 포맷 문자열 확인
        if (getCurrentToken().type == STRCON) {
            cout << "DEBUG: Found string constant: " << getCurrentToken().value << endl;  // 포맷 문자열 출력
            string formatString = getCurrentToken().value;
            formatSpecifierCount = countFormatSpecifiers(formatString); // 포맷 specifier 개수 세기
            outputToken(getCurrentToken());
            advanceToken();
        }

        // 표현식들 확인
        while (getCurrentToken().type == COMMA) {
            cout << "DEBUG: Found COMMA ','." << endl;
            outputToken(getCurrentToken());
            advanceToken();
            parseExp();
            expressionCount++;
            cout << "DEBUG: Expression count: " << expressionCount << endl;
            cout << "DEBUG: After parseExp(), current token type: " << tokenTypeToString(getCurrentToken().type) << endl;
        }

        cout << "DEBUG: Format specifier count: " << formatSpecifierCount << ", Expression count: " << expressionCount << endl;
        if (expressionCount != formatSpecifierCount) {
            cout << "DEBUG: Error - Expression count does not match format specifier count!" << endl;
            Token printfToken = getPreviousToken();
            parseErrorPrintL();
        }

        // RPARENT ')' 처리 
        if (getCurrentToken().type == RPARENT) {
            cout << "DEBUG: Found RPARENT ')'" << endl;
            outputToken(getCurrentToken());
            advanceToken();
        }
        else {
            cout << "DEBUG: Missing RPARENT ')' Advancing token to recover." << endl;
            parseErrorPrintfJ();
        }

        // SEMICOLON ';' 처리
        if (getCurrentToken().type == SEMICN) {
            cout << "DEBUG: Found SEMICOLON ';'" << endl;
            outputToken(getCurrentToken());
            advanceToken();
        }
        else {
            // SEMICOLON이 없으면 오류 보고 후 다음 토큰으로 넘어감
            cout << "DEBUG: Missing SEMICOLON ';'... Advancing token to recover." << endl;
            parseErrorPrintfI();
        }

        cout << "DEBUG: Finished parsing printf statement." << endl;
    }

    // Block 처리
    else if (token.type == LBRACE) {
        parseBlock();
    }

    // LVal '=' Exp ';' 또는 LVal '=' 'getint' '(' ')' ';' 또는 'getchar' '(' ')' ';'
    else {
        int index = 0;
        bool isLVal = false;

        size_t tempIndex = tokenIndex;
        Token tempToken = tokens[tempIndex];
        bool isAssignStmt = false;

        // LVal 여부 판단 (다음 토큰이 '='인지 확인)
        if (tokenIndex + 1 < tokens.size()) {

            Token nextToken = tokens[tokenIndex + 1];

            while (true) {
                if (nextToken.type == ASSIGN) {
                    isLVal = true;
                    break;
                }

                if (nextToken.type == SEMICN || tokenIndex + index >= tokens.size()) {
                    break;
                }
                index++;
                nextToken = tokens[tokenIndex + index];
            }
        }

        // LVal인 경우 처리
        if (isLVal) {
            string lvalName = getCurrentToken().value;
            parseLVal(); // LVal 처리

            // '=' 출력 및 다음 토큰으로 이동
            outputToken(getCurrentToken());
            advanceToken();
            Token token = getCurrentToken();

            if (isConstLVal(lvalName)) {
                parseErrorPrintH();
            }

            // 'getint' 또는 'getchar' 처리
            if (token.type == GETINTTK || token.type == GETCHARTK) {
                outputToken(token);
                advanceToken();

                // '(' 처리
                if (getCurrentToken().type == LPARENT) {
                    outputToken(getCurrentToken());
                    advanceToken();
                }

                // ')' 처리
                if (getCurrentToken().type == RPARENT) {
                    outputToken(getCurrentToken());
                    advanceToken();
                }
                else {
                    parseErrorPrintfJ();
                }

                // ';' 처리
                if (getCurrentToken().type == SEMICN) {
                    outputToken(getCurrentToken());
                    advanceToken();
                }
                else {
                    parseErrorPrintfI();
                }
            }

            // LVal '=' Exp ';' 처리
            else {
                parseExp();

                // ';' 처리
                if (getCurrentToken().type == SEMICN) {
                    outputToken(getCurrentToken());
                    advanceToken();
                }

                else {
                    parseErrorPrintfI();
                }
            }
        }

        // [Exp] ';' 처리
        else {
            // LVal이 아닌 다른 표현식에 대입이 시도된 경우 오류 발생
            if (getCurrentToken().type == ASSIGN) {
                advanceToken();
                parseExp();

                if (getCurrentToken().type == SEMICN) {
                    outputToken(getCurrentToken());
                    advanceToken();
                }
                else {
                    parseErrorPrintfI();
                }
            }
            else {
                // 대입이 아닌 다른 일반적인 표현식은 그대로 처리
                parseExp();

                if (getCurrentToken().type == SEMICN) {
                    outputToken(getCurrentToken());
                    advanceToken();
                }
                else {
                    parseErrorPrintfI();
                }
            }
        }

    }
    outputFile << "<Stmt>" << endl;

    return isReturn;
}

// FuncRParams → Exp { ',' Exp } 
size_t parseFuncRParams(vector<string>& passedParamTypes) {

    size_t paramCount = 0;
    parseExp();
    paramCount++;
    passedParamTypes.push_back(currentBaseType);  // 함수 매개변수 저장

    while (getCurrentToken().type == COMMA) {
        outputToken(getCurrentToken()); // ',' 출력
        advanceToken();
        parseExp();

        paramCount++;
        passedParamTypes.push_back(currentBaseType); // 함수 매개변수 저장
    }

    outputFile << "<FuncRParams>" << endl;

    return paramCount;
}

// FuncType → 'void' | 'int' | 'char'
void parseFuncType() {
    Token token = getCurrentToken();

    if (token.type == VOIDTK) {
        currentBaseType = "void";
    }
    else if (token.type == INTTK) {
        currentBaseType = "Int";
    }
    else {
        currentBaseType = "Char";
    }

    currentReturnType = currentBaseType;
    cout << "현재 함수 리턴 타입 : " << currentReturnType << endl;

    outputToken(token);
    advanceToken();
    token = getCurrentToken();

    outputFile << "<FuncType>" << endl;
}

// FuncFParams → FuncFParam { ',' FuncFParam }
size_t parseFuncFParams(vector<string>& paramTypes, const string& funcName) {
    enterScope();

    unordered_set<string> paramNames;
    size_t paramCount = 0;
    string paramType;
    string paramName = parseFuncFParam(paramType);
    paramTypes.push_back(paramType);
    paramCount++;

    // 중복된 이름 있는지 체크 
    if (paramNames.find(paramName) == paramNames.end()) {
        paramNames.insert(paramName);
    }

    while (getCurrentToken().type == COMMA) {
        outputToken(getCurrentToken());
        advanceToken();

        paramName = parseFuncFParam(paramType);
        paramTypes.push_back(paramType);
        paramCount++;

        if (paramNames.find(paramName) == paramNames.end()) {
            paramNames.insert(paramName);
        }
    }

    scopeCounter--;
    CurrentScope = scopeCounter;

    exitScope();

    outputFile << "<FuncFParams>" << endl;

    return paramCount;
}


// FuncDef → FuncType Ident '(' [FuncFParams] ')' Block // j
void parseFuncDef() {
    parseFuncType();
    string currentReturnTypeLocal = currentBaseType; // 지역 변수로 리턴 타입 저장

    Token token = getCurrentToken();

    // 함수 이름 처리
    if (token.type == IDENFR) {
        string currentFuncName = token.value;

        // 전역 스코프에서 함수 중복 정의 검사
        if (isSymbolDefinedInCurrentScope(currentFuncName)) {
            parseErrorPrintB(token);
            advanceToken();

            // '(' 처리
            token = getCurrentToken();
            if (getCurrentToken().type == LPARENT) {
                outputToken(getCurrentToken());
                advanceToken();
            }

            // [FuncFParams] 생략하고 ')'로 이동
            while (getCurrentToken().type != RPARENT && tokenIndex < tokens.size()) {
                advanceToken();
            }

            // ')' 처리
            if (getCurrentToken().type == RPARENT) {
                outputToken(getCurrentToken());
                advanceToken();
            }
            else {
                parseErrorPrintfJ();
            }

            // 함수 본문 파싱
            parseBlock();

            outputFile << "<FuncDef>" << endl;
            return;
        }
        else {
            // 새로운 함수인 경우, 전역 스코프에 추가
            string funcType = SymbolType(false, currentReturnTypeLocal, false, true);  // 함수 타입 설정
            addSymbolWithCheck(token, funcType, true);
            outputToken(token);
            advanceToken();

            // 함수 매개변수 파싱
            vector<string> paramTypes; // 함수 매개변수 타입 리스트
            size_t paramCount = 0;     // 함수 매개변수 개수

            // '(' 처리
            if (getCurrentToken().type == LPARENT) {
                outputToken(getCurrentToken());
                advanceToken();

                // [FuncFParams]가 존재하면 파싱
                if (getCurrentToken().type != RPARENT) {
                    paramCount = parseFuncFParams(paramTypes, currentFuncName);
                }

                // ')' 처리
                token = getCurrentToken();
                if (token.type == RPARENT) {
                    outputToken(token);
                    advanceToken();
                }
                else {
                    parseErrorPrintfJ();
                }
            }

            functionParamsTypes[currentFuncName] = paramTypes;
            functionParams[currentFuncName] = paramCount;

            returnTypeStack.push(currentReturnType);
            currentReturnType = currentReturnTypeLocal;

            // 함수 본문 파싱
            bool hasReturn = parseBlock();

            // 함수 본문 파싱 후 이전 반환 타입을 복원
            if (!returnTypeStack.empty()) {
                currentReturnType = returnTypeStack.top();
                returnTypeStack.pop();
            }
            else {
                currentReturnType = ""; // 기본값 또는 전역 반환 타입 설정
            }

            // 반환 타입 검사
            if (currentReturnTypeLocal != "void" && !hasReturn) {
                Token lastToken = getPreviousToken();
                parseErrorPrintG(lastToken);
            }

            outputFile << "<FuncDef>" << endl;
        }
    }
}

// Cond → LOrExp 
void parseCond() {
    parseLOrExp();
    outputFile << "<Cond>" << endl;
}

// ConstDef → Ident [ '[' ConstExp ']' ] '=' ConstInitVal // k
void parseConstDef() {

    Token token = getCurrentToken();
    string constName;

    // Ident 처리
    if (token.type == IDENFR) {
        constName = token.value;
        outputToken(token);
        advanceToken();
    }

    bool isArray = false;
    string declarationType = currentBaseType;

    // '[' ConstExp ']' 처리 
    if (getCurrentToken().type == LBRACK) {
        outputToken(getCurrentToken());
        advanceToken();

        parseConstExp();

        if (getCurrentToken().type == RBRACK) {
            outputToken(getCurrentToken());
            advanceToken();
            isArray = true;
        }

        else parseErrorPrintfK();
    }

    // '=' 처리
    if (getCurrentToken().type == ASSIGN) {
        outputToken(getCurrentToken());
        advanceToken();
    }

    token = getCurrentToken();
    // 현재 선언 타입 저장
    cout << "parseConstDef: declarationType = " << declarationType << ", isArray = " << isArray << endl;

    parseConstInitVal();

    // 선언 타입 복원
    currentBaseType = declarationType;
    // 중복 검사: 현재 스코프에 동일한 이름의 상수가 있는지 확인
    if (isSymbolDefinedInCurrentScope(constName)) {
        parseErrorPrintB(token); // 중복 시 에러 'b' 출력
    }
    else {
        string typeName = SymbolType(true, declarationType, isArray, false); // 배열
        Token identToken = { IDENFR, constName, token.lineNumber };
        addSymbolWithCheck(identToken, typeName);
    }

    outputFile << "<ConstDef>" << endl;
}

// ConstExp → AddExp  비고: 사용하는 Ident는 상수
void parseConstExp() {
    parseAddExp();
    outputFile << "<ConstExp>" << endl;
}

// ConstInitVal → ConstExp | '{' [ ConstExp { ',' ConstExp } ] '}' | StringConst
void parseConstInitVal() {

    // '{' [ ConstExp { ',' ConstExp } ] '}'
    if (getCurrentToken().type == LBRACE) {
        outputToken(getCurrentToken());
        advanceToken();

        if (getCurrentToken().type != RBRACE) {
            parseConstExp();

            while (getCurrentToken().type == COMMA) {
                outputToken(getCurrentToken());
                advanceToken();

                parseConstExp();
            }

            // '}' 확인 
            if (getCurrentToken().type == RBRACE) {
                outputToken(getCurrentToken());
                advanceToken();
            }
        }
        else if (getCurrentToken().type == RBRACE) {
            outputToken(getCurrentToken());
            advanceToken();
        }
    }

    // StringConst 처리
    else if (getCurrentToken().type == STRCON) {
        outputToken(getCurrentToken());
        advanceToken();
    }

    // ConstExp 처리
    else {
        parseConstExp();
    }

    outputFile << "<ConstInitVal>" << endl;
}

// VarDecl → BType VarDef { ',' VarDef } ';' // i
void parseVarDecl() {

    parseBType();
    string baseType = currentBaseType;
    parseVarDef(baseType);

    Token token = getCurrentToken();

    // 여러 변수 정의가 있는 경우 (','로 구분되는 경우)
    while (token.type == COMMA) {
        outputToken(token);
        advanceToken();
        token = getCurrentToken();

        parseVarDef(baseType);
        token = getCurrentToken();
    }

    token = getCurrentToken();

    // 세미콜론 ';' 확인
    if (token.type == SEMICN) {
        outputToken(token);
        advanceToken();
    }
    else {
        parseErrorPrintfI();
    }

    outputFile << "<VarDecl>" << endl;
}

// VarDef → Ident [ '[' ConstExp ']' ] | Ident [ '[' ConstExp ']' ] '=' InitVal // k
void parseVarDef(const string& baseType) {

    Token token = getCurrentToken();
    string varName;

    // Ident (변수 이름) 확인
    if (token.type == IDENFR) {
        varName = token.value;
        outputToken(token);
        advanceToken();
    }

    bool isArray = false;
    // 배열 구문 처리: '[' ConstExp ']'
    if (getCurrentToken().type == LBRACK) {
        outputToken(getCurrentToken());
        advanceToken();
        parseConstExp();

        if (getCurrentToken().type == RBRACK) {
            outputToken(getCurrentToken());
            advanceToken();
            isArray = true;
        }
        else {
            parseErrorPrintfK();
        }
    }

    // 초기화 구문 처리: '=' InitVal
    if (getCurrentToken().type == ASSIGN) {
        outputToken(getCurrentToken());
        advanceToken();
        parseInitVal();
    }


    // 중복 검사: 현재 스코프에 동일한 이름의 변수가 있는지 확인
    if (isSymbolDefinedInCurrentScope(varName)) {
        parseErrorPrintB(token);
    }
    else {
        string typeName = SymbolType(false, baseType, isArray, false); // 배열 
        Token identToken = { IDENFR, varName, token.lineNumber };
        addSymbolWithCheck(identToken, typeName);
    }
    outputFile << "<VarDef>" << endl;
}

// InitVal → Exp | '{' [ Exp { ',' Exp } ] '}' | StringConst
void parseInitVal() {

    Token token = getCurrentToken();

    // StringConst
    if (getCurrentToken().type == STRCON) {
        outputToken(getCurrentToken());
        advanceToken();
    }

    // '{' [ Exp { ',' Exp } ] '}'
    else if (getCurrentToken().type == LBRACE) {
        outputToken(getCurrentToken());
        advanceToken();

        // [ Exp { ',' Exp } ]
        if (getCurrentToken().type != RBRACE) {

            // 첫 번째 Exp
            parseExp();

            // { ',' Exp }
            while (getCurrentToken().type == COMMA) {
                outputToken(getCurrentToken());
                advanceToken();

                parseExp();
            }
        }

        // '}'
        if (getCurrentToken().type == RBRACE) {
            outputToken(getCurrentToken());
            advanceToken();
        }
    }

    // Exp
    else {
        parseExp();
    }

    outputFile << "<InitVal>" << endl;
}

// MainFuncDef → 'int' 'main' '(' ')' Block // k
void parseMainFuncDef() {
    Token token = getCurrentToken();

    // int 확인
    if (token.type == INTTK) {
        outputToken(token);
        advanceToken();
        currentReturnType = "int";
    }

    // main 확인
    token = getCurrentToken();
    if (token.type == MAINTK) {
        outputToken(token);
        advanceToken();
    }
    else {
        advanceToken();
    }

    // ( 확인
    token = getCurrentToken();
    if (token.type == LPARENT) {
        outputToken(token);
        advanceToken();
    }

    // ) 확인
    token = getCurrentToken();
    if (token.type == RPARENT) {
        outputToken(token);
        advanceToken();
    }
    else {
        parseErrorPrintfJ();
    }

    bool hasReturn = parseBlock();
    if (!hasReturn) {
        Token lastToken = getPreviousToken();  // 함수 정의의 마지막 토큰
        parseErrorPrintG(lastToken);
    }

    exitScope();

    outputFile << "<MainFuncDef>" << endl;
}

// CompUnit → {Decl} {FuncDef} MainFuncDef
void parseCompUnit()
{
    Token token = getCurrentToken();

    // {Decl}
    while (true)
    {
        token = getCurrentToken();
        if (token.type == CONSTTK) {
            parseConstDecl();
        }

        else if (token.type == INTTK || token.type == CHARTK) {
            if (tokenIndex + 1 < tokens.size())
            {
                Token nextToken = tokens[tokenIndex + 1];
                if (nextToken.type == IDENFR)
                {
                    if (tokenIndex + 2 < tokens.size() && tokens[tokenIndex + 2].type == LPARENT)
                    {
                        break; // 함수 정의로 간주
                    }
                    else
                    {
                        parseVarDecl();
                    }
                }
                else if (nextToken.type == MAINTK)
                {
                    break; // 메인 함수로 간주
                }
                else
                {
                    parseVarDecl();
                }
            }
            else
            {
                parseVarDecl();
            }
        }
        else {
            break;
        }
    }

    // {FuncDef}
    while (true)
    {
        token = getCurrentToken();
        if (token.type == VOIDTK || token.type == INTTK || token.type == CHARTK)
        {
            if (tokenIndex + 1 < tokens.size())
            {
                Token nextToken = tokens[tokenIndex + 1];
                if (nextToken.type == IDENFR)
                {
                    if (tokenIndex + 2 < tokens.size() && tokens[tokenIndex + 2].type == LPARENT)
                    {
                        parseFuncDef(); // 함수 정의 파싱
                        continue;
                    }
                    else
                    {
                        break;
                    }
                }
                else if (nextToken.type == MAINTK)
                {
                    break; // 메인 함수로 간주
                }
                else
                {
                    break;
                }
            }
            else
            {
                break;
            }
        }
        else
        {
            break;
        }
    }

    // MainFuncDef
    parseMainFuncDef();

    outputFile << "<CompUnit>" << endl;
}

int main()
{
    // 어휘 분석 수행
    lexicalAnalysis("testfile.txt");


    // 어휘 분석
    outputFile.open("lexer.txt");
    for (const Token& token : tokens) {
        outputToken(token);
    }
    outputFile.close();

    // 구문 분석
    outputFile.open("parser.txt");
    parseCompUnit();
    outputFile.close();

    //parseCompUnit();

    // symbolTable을 scopelineNumber 기준으로 오름차순 정렬
    stable_sort(symbolTable.begin(), symbolTable.end(), [](const Symbol& a, const Symbol& b) -> bool {
        return a.scopelineNumber < b.scopelineNumber;
        });

    // 정렬된 symbolTable을 symbol.txt에 출력
    outputFile.open("symbol.txt");
    for (const Symbol& sym : symbolTable) {
        outputFile << sym.scopelineNumber << " " << sym.name << " " << sym.type << endl;
    }
    outputFile.close();

    // 오류 처리
    if (!errors.empty()) {
        // lineNumber 기준으로 정렬
        sort(errors.begin(), errors.end(), [](const Error& a, const Error& b) {
            return a.lineNumber < b.lineNumber; // lineNumber가 작은 순서대로 정렬
            });

        ofstream errorFile("error.txt");
        for (vector<Error>::const_iterator it = errors.begin(); it != errors.end(); ++it) {
            errorFile << it->lineNumber << " " << it->errorcode << endl;
        }
        errorFile.close();
    }


    return 0;
}
