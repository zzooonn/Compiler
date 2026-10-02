#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <stack>
#include <cctype> 
#include <algorithm>
#include <unordered_set>
#include <sstream>

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
    int scopelineNumber = 0; // 스코프 레벨
    string name;             // 식별자 이름
    string type;             // 데이터 타입 (예: Int, Char, IntArray 등)
    bool isConst = false;    // 상수 여부
    bool isArray = false;    // 배열 여부
    bool isFunction = false; // 함수 여부
    bool isInitialized = false;  // 초기화 여부 
    int address = -1;        // 메모리 주소
    int value = 0;           // 변수 또는 상수의 값 저장
    int arraySize = 0;       // 배열 크기
};

class Label {
//레이블이 가리키는 코드 리스트 내의 위치(주소)
private:
    int add = 0;

public:
    Label() { }

    // 레이블의 주소 값을 반환
    int getAdd() {
        return add;
    }
    // 레이블의 주소 값을 설정
    void setAdd(int add) {
        this->add = add;
    }
};

class Pcode {
private:
    string opcode;
    string operand;
    int lineNumber;
    bool isContinue = false;
    int addr = 0;
    int scope = 0;
    string print = "";
    Label* label = nullptr;

public:
    string getopcode() {
        return opcode;
    }

    string getoperand() {
        return operand;
    }

    int getlineNumber() {
        return lineNumber;
    }

    bool getContinue() {
        return isContinue;
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

    Label* getLabel() {
        return label;
    }

    void setopcode(const string& op) {
        opcode = op;
    }

    void setoperand(const string& opnd) {
        operand = opnd;
    }

    void setlineNumber(int line) {
        lineNumber = line;
    }

    void setisContinue(bool cont) {
        isContinue = cont;
    }
};

class Value {
private:
    int value;
    string Svalue;
    bool isInt;
public:
    // 정수 값 생성자
    Value(int value) : value(value), isInt(true) {}

    // 문자열 값 생성자
    Value(const string& Svalue) : Svalue(Svalue), isInt(false) {}

    // 정수 값 반환
    int getValue() const {
        if (isInt) {
            return value;
        }
        // 예외 처리 또는 기본값 반환
        cerr << "Error: Attempted to get integer value from a non-integer Value." << endl;
        return 0;
    }

    // 문자열 값 반환
    string getSValue() const {
        if (!isInt) {
            return Svalue;
        }
        // 예외 처리 또는 기본값 반환
        cerr << "Error: Attempted to get string value from an integer Value." << endl;
        return "";
    }

    // 정수 값 설정
    void setValue(int value) {
        this->value = value;
        isInt = true;
    }

    // 문자열 값 설정
    void setSValue(const string& Svalue) {
        this->Svalue = Svalue;
        isInt = false;
    }

    // 타입 확인
    bool isInteger() const {
        return isInt;
    }

    bool isString() const {
        return !isInt;
    }

    // operator<< 오버로딩
    friend ostream& operator<<(ostream& os, const Value& value) {
        if (value.isInt) {
            os << value.value;  // 정수 값 출력
        }
        else {
            os << "\"" << value.Svalue << "\"";  // 문자열 값 출력
        }
        return os;
    }
};


// 함수
void lexicalAnalysis(const string& filename);
void outputToken(const Token& token);
void advanceToken();
int parseExp();
void parseNumber();
void parseCharacter();
int parseLVal();
int parsePrimaryExp();
size_t parseFuncRParams(vector<string>& passedParamTypes, vector<int>& passedParamValues);
void parseCond();
void parseUnaryOp(Token& opToken);
int parseUnaryExp();
int parseConstExp();
int parseMulExp();
int parseAddExp();
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
string parseFuncFParam(string& paramType, const string& funcName);
void parseFuncDef();
size_t parseFuncFParams(vector<string>& paramTypes, const string& funcName);
void parseMainFuncDef();
int parseConstInitVal(int baseAddr);
void parseVarDecl();
void parseVarDef(const string& baseType);
int parseInitVal(int baseAddr);
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
stack<int> nextAddressStack; // 스코프별 주소 관리 스택

int CurrentScope = 1;
int NextScope = 2;
string currentBaseType;
int scopeCounter = 1;
string currentReturnType; // 현재 리턴 타입 저장 
bool inLoopContext = false, inIfContext = false;
unordered_map<string, vector<string>> functionParamsTypes; // 함수 이름과 매개변수 타입을 저장
vector<string> passedParamTypes;  // 전달된 매개변수의 타입을 저장하는 벡터
vector<int> passedParamValues; // 매개변수 값 저장
unordered_set<int> errorLines; // 이미 오류가 보고된 줄을 추적
unordered_map<int, int> scopeParent;
stack<string> returnTypeStack;

vector<Pcode> pcodeList;
unordered_map<string, int> variables;
vector<string> interpretResult;
const int STACK_SIZE = 100000;
int executionStack[STACK_SIZE];
int dstack[STACK_SIZE] = { 0 };
int BAddr = 0, at = 0, sp = -1, bp = 0;     // 베이스 주소, 프로그램 카운터, 스택 포인터
int currentConstValue = 0;
int address = 0, nextaddress = 0;
unordered_map<string, int> functionMap; // 함수 이름과 시작 P-Code 인덱스를 매핑
unordered_map<string, vector<pair<string, int>>> functionParams; // 함수 이름 -> (매개변수 이름, 주소) 쌍 벡터
unordered_map<string, vector<int>> functionParamAddresses;
stack<unordered_map<string, Symbol>> symbolTableStack;
unordered_set<int> initializedVariables;
stack<pair<int, int>> callStack;
string varName;
int varAddr;
stack<int> loopStartLabels;
vector<int> loopEndStack; // 루프의 끝 위치를 저장하는 스택
stack<vector<int>> breakPatchesStack;
stack<vector<int>> continuePatchesStack;
int currentIncrementStartIdx = -1; // 전역 변수로 선언
int currentConditionCheckIdx = -1; // 조건 검사 위치 인덱스를 저장하는 변수
stack<int> conditionCheckIdxStack;

const int MAX_VARIABLES = 1000;  // 변수 저장 용량
bool updated = false;
int currentValue = 0;
Value ExpValue();
Value AddValue();
Value MulValue();
Value UnaryExp();
Value PrimaryExp();
Value LValValue();

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

int getNextAddress() {
    if (nextAddressStack.empty()) {
        cerr << "[ERROR] nextAddressStack이 비어 있습니다. 스코프를 올바르게 초기화했는지 확인하세요." << endl;
        exit(1); // 적절한 에러 처리
    }
    int addr = nextAddressStack.top();
    nextAddressStack.pop();
    nextAddressStack.push(addr + 1);
    cout << "[DEBUG] getNextAddress: 현재 주소 = " << addr << ", 다음 주소 = " << (addr + 1) << endl;
    return addr;
}

int getVariableValue(const string& varName) {
    // 심볼 테이블을 역순으로 순회 (가장 최근에 정의된 변수부터 확인)
    for (auto it = symbolTable.rbegin(); it != symbolTable.rend(); ++it) {
        if (it->name == varName && it->scopelineNumber <= CurrentScope) {
            // 변수의 주소가 유효한지 확인
            if (it->address != -1 && it->address < STACK_SIZE) {
                cout << "[디버그] getVariableValue: dstack[" << it->address << "] 값 = " << dstack[it->address] << endl;
                // 스택에서 변수 값을 반환
                return dstack[it->address];
            }

            // 스택 값이 없으면 심볼 테이블에 저장된 값을 반환
            cout << "[디버그] getVariableValue: 심볼 테이블에서 " << varName << " 값 반환, 값 = " << it->value << endl;
            return it->value;
        }
    }

    // 변수 이름을 심볼 테이블에서 찾을 수 없는 경우
    cerr << "[오류] getVariableValue: 변수 " << varName << " 을(를) 찾을 수 없습니다." << endl;
    return 0; // 기본값 반환
}

// 심볼 추가 함수
void addSymbol(const string& name, const string& type, bool isFunction = false) {
    Symbol sym;
    sym.name = name;
    sym.type = type;
    sym.scopelineNumber = CurrentScope;
    sym.isFunction = isFunction;
    sym.isConst = false; // 기본값
    sym.isArray = false; // 기본값
    sym.value = 0; // 기본값

    if (!isFunction) {
        sym.address = getNextAddress();
    }
    else {
        sym.address = -1; // 함수의 경우 후에 설정
    }

    symbolTable.push_back(sym);

    // cout << "Symbol added: " << sym.name << " | Type: " << sym.type << " | Scope: " << sym.scopelineNumber << " | IsFunction: " << (isFunction ? "Yes" : "No") << endl;
}

void enterScope() {
    nextAddressStack.push(nextAddressStack.top());

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
    if (!nextAddressStack.empty()) {
        nextAddressStack.pop();
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

        // cout << "Symbol added: " << sym.name << " | Type: " << sym.type
        //     << " | Scope: " << sym.scopelineNumber
        //     << " | IsFunction: " << (isFunction ? "Yes" : "No") << endl;
    }
}

void parseErrorPrintD(const Token& token) {
    addError(token.lineNumber, 'd');
}

void parseErrorPrintE() {
    Token token = getCurrentToken();
    addError(token.lineNumber, 'e');
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

    if (expType == pasType) {
        return true;
    }

    // char가 int로 변환 가능한 경우 허용
    if (expType == "Int" && pasType == "Char") {
        return true;
    }

    // int가 char로 변환 가능한 경우 허용 (하위 8비트)
    if (expType == "Char" && pasType == "Int") {
        return true;
    }

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

int getSymbolAddress(const string& varName) {
    // 현재 스코프부터 상위 스코프까지 검색
    int scope = CurrentScope;
    while (scope != 0) { // 전역 스코프가 0이라고 가정
        for (auto it = symbolTable.rbegin(); it != symbolTable.rend(); ++it) {
            if (it->name == varName && it->scopelineNumber == scope) {
                cout << "[DEBUG] getSymbolAddress: " << varName << " -> " << it->address << endl;
                return it->address;
            }
        }
        if (scopeParent.find(scope) != scopeParent.end()) {
            scope = scopeParent[scope]; // 부모 스코프로 이동
        }
        else {
            break;
        }
    }

    cerr << "[ERROR] getSymbolAddress: 변수 '" << varName << "' 주소를 찾을 수 없습니다." << endl;
    return -1;
}

int getVariableAddress(const string& varName) {
    for (auto it = symbolTable.rbegin(); it != symbolTable.rend(); ++it) {
        if (it->name == varName && it->scopelineNumber <= CurrentScope) {
            return it->address;
        }
    }
    // 변수를 찾을 수 없는 경우
    return -1;
}

// 심볼 테이블에서 특정 이름을 가진 심볼을 검색
Symbol getSymbolFromTable(const string& name) {
    // 현재 스코프부터 상위 스코프까지 검색
    int scope = CurrentScope;
    while (scope != 0) { // 전역 스코프가 0이라고 가정
        for (auto it = symbolTable.rbegin(); it != symbolTable.rend(); ++it) {
            if (it->name == name && it->scopelineNumber == scope) {
                cout << "[DEBUG] getSymbolFromTable: Found symbol '" << name
                    << "' in scope " << scope << endl;
                return *it; // 심볼 정보를 반환
            }
        }
        if (scopeParent.find(scope) != scopeParent.end()) {
            scope = scopeParent[scope]; // 부모 스코프로 이동
        }
        else {
            break;
        }
    }

    // 심볼을 찾지 못한 경우 기본값 반환
    cerr << "[ERROR] getSymbolFromTable: Symbol '" << name
        << "' not found in any scope." << endl;
    return Symbol(); // 기본값 반환 (address = -1)
}

// 심볼 테이블에서 변수 검색
bool findVariable(const string& name, int& addr, string& type, bool& isConst, int& constValue) {
    Symbol symbol = getSymbolFromTable(name); // getSymbolFromTable 함수 활용
    if (symbol.address != -1) {
        addr = symbol.address;
        type = symbol.type;
        isConst = symbol.isConst;
        constValue = symbol.value;
        return true;
    }
    return false;
}

void logSymbolTable() {
    cout << "[DEBUG] 심볼 테이블 상태:" << endl;
    for (const auto& sym : symbolTable) {
        cout << "이름: " << sym.name
            << ", 타입: " << sym.type
            << ", 주소: " << sym.address
            << ", 초기값: " << sym.value
            << ", 스코프 라인: " << sym.scopelineNumber
            << (sym.isConst ? ", 상수" : ", 변수")
            << (sym.isArray ? ", 배열 크기: " + to_string(sym.arraySize) : "")
            << endl;
    }
    cout << "----------------------------------------------------------------" << endl;
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

// Pcode
int emitPcode(const string& opcode, const string& operand, int lineNumber) {

    Pcode pcode;
    pcode.setopcode(opcode);    
    pcode.setoperand(operand);    
    pcode.setlineNumber(lineNumber); 
    pcode.setisContinue(true);     
    pcodeList.push_back(pcode);

    int pcodeIndex = pcodeList.size() - 1; // 현재 P-code의 인덱스
    //cout << "PCODE Emitted: " << opcode << " " << operand << " (Line " << lineNumber << ") at index " << pcodeIndex << endl;

    return pcodeIndex;
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
        outputToken(token);
        currentBaseType = "Int";
        currentValue = stoi(token.value);
        emitPcode("LIT", token.value, token.lineNumber);
        advanceToken();
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

            emitPcode("LIT", std::to_string(static_cast<int>(character)), token.lineNumber);
            advanceToken();
        }
    }
    outputFile << "<Character>" << endl;
}

// LVal → Ident ['[' Exp ']'] // k
int parseLVal() {
    Token token = getCurrentToken();
    int value = 0;

    if (token.type == IDENFR) {
        // 식별자 선언 여부 확인
        if (!isSymbolDefined(token.value)) {
            parseErrorPrintC(token);
        }

        outputToken(token);
        string varName = token.value;
        cout << "\n[DEBUG] parseLVal: 현재 변수 이름 = " << varName << endl;

        advanceToken();

        // 심볼 테이블에서 변수 타입 및 주소 가져오기
        int varAddr = -1;
        string varType;
        bool isConst = false;
        int constValue = 0;

        // 상수를 우선적으로 찾습니다.
        for (auto it = symbolTable.rbegin(); it != symbolTable.rend(); ++it) {
            if (it->name == varName && it->scopelineNumber <= CurrentScope) {
                if (it->isConst) {
                    // 상수인 경우 우선적으로 참조
                    varType = it->type;
                    varAddr = it->address;
                    isConst = it->isConst;
                    constValue = it->value;
                    cout << "[DEBUG] 상수 참조: 이름 = " << varName << ", 주소 = " << varAddr << ", 값 = " << constValue << endl;
                    break;
                }
            }
        }

        // 만약 상수를 찾지 못한 경우 변수 검색
        if (varAddr == -1) {
            for (auto it = symbolTable.rbegin(); it != symbolTable.rend(); ++it) {
                if (it->name == varName && it->scopelineNumber <= CurrentScope) {
                    varType = it->type;
                    varAddr = it->address;
                    isConst = it->isConst;
                    constValue = it->value;
                    break;
                }
            }
        }

        if (varAddr == -1) {
            cout << "[DEBUG] Error: Variable " << varName << " not found in symbol table." << endl;
            return 0;
        }

        cout << "[DEBUG] 변수 저장: " << varName << " -> 주소: " << varAddr << endl;

        // 상수 처리
        if (isConst) {
            value = constValue;
            cout << "[DEBUG] CONST 참조: 이름 = " << varName << ", 값 = " << value << endl;
            emitPcode("LIT", to_string(value), token.lineNumber);
            return value;
        }

        bool isArrayAccess = false;

        // 배열 요소 접근 처리
        token = getCurrentToken();
        if (token.type == LBRACK) {
            outputToken(token);
            advanceToken();

            int indexValue = parseExp();
            token = getCurrentToken();
            if (token.type == RBRACK) {
                outputToken(token);
                advanceToken();
                isArrayAccess = true;

                emitPcode("LDA", to_string(varAddr), token.lineNumber); // 배열 시작 주소 로드
                emitPcode("ADD", "", token.lineNumber);                 // 인덱스와 주소를 더함
                emitPcode("LODS", "", token.lineNumber);                // 해당 주소의 값을 로드

                cout << "[DEBUG] 배열 접근: 변수 = " << varName
                    << ", 인덱스 = " << indexValue
                    << ", 주소 = " << (varAddr + indexValue) << endl;
            }
            else {
                parseErrorPrintfK();
            }
        }
        else {
            // 변수의 주소에서 값을 로드
            if (!isArrayAccess) {
                emitPcode("LOD", to_string(varAddr), token.lineNumber);
                cout << "[DEBUG] 변수 로드 완료: 이름 = " << varName
                    << ", 주소 = " << varAddr << endl;

                value = getVariableValue(varName);
            }
        }

        currentBaseType = varType;

        outputFile << "<LVal>" << endl;
        return value;
    }
    else {
        return value; // LVal이 아님을 표시
    }
}

Value LValValue() {
    Token token = getCurrentToken();

    if (token.type == IDENFR) {
        // 식별자 선언 여부 확인
        if (!isSymbolDefined(token.value)) {
            parseErrorPrintC(token);
        }

        outputToken(token);
        string varName = token.value;
        cout << "[DEBUG] parseLVal: 현재 변수 이름 = " << varName << endl;

        advanceToken();

        // 심볼 테이블에서 변수 타입 및 주소 가져오기
        string varType = getSymbolType(varName);
        int varAddr = getVariableAddress(varName);
        if (varAddr == -1) {
            cerr << "[DEBUG] Error: Variable " << varName << " not found in symbol table." << endl;
            return Value(0);
        }

        cout << "[DEBUG] 변수 정보: 이름 = " << varName << ", 주소 = " << varAddr << ", 타입 = " << varType << endl;

        bool isArrayAccess = false;
        int loadedValue = 0;

        // 배열 요소 접근 확인
        token = getCurrentToken();
        if (token.type == LBRACK) {
            outputToken(token);
            advanceToken();

            Value indexValue = ExpValue();
            int index = indexValue.getValue();

            token = getCurrentToken();
            if (token.type == RBRACK) {
                outputToken(token);
                advanceToken();
                isArrayAccess = true;

                // 배열 접근
                int effectiveAddr = varAddr + index;

                emitPcode("LDA", to_string(varAddr), token.lineNumber); // 배열 시작 주소 로드
                emitPcode("LIT", to_string(index), token.lineNumber);   // 인덱스 값 로드
                emitPcode("ADD", "", token.lineNumber);                // 주소 계산
                emitPcode("LODS", "", token.lineNumber);               // 해당 주소의 값을 로드

                cout << "[DEBUG] 배열 접근: 변수 = " << varName << ", 인덱스 = " << index << endl;
            }
            else {
                addError(token.lineNumber, 'g'); // ']' 누락
            }
        }
        else {
            // 변수 값 로드 (emitPcode가 값을 처리함)
            emitPcode("LOD", to_string(varAddr), token.lineNumber);
            cout << "[DEBUG] 변수 접근: 변수 = " << varName << ", 주소 = " << varAddr << endl;
        }

        // 타입 설정
        if (isArrayAccess) {
            if (varType == "IntArray") {
                currentBaseType = "Int";
            }
            else if (varType == "CharArray") {
                currentBaseType = "Char";
            }
            else {
                currentBaseType = EraseConst(varType); // 상수 배열 처리
            }
        }
        else {
            currentBaseType = varType;
        }

        outputFile << "<LVal>" << endl;
        cout << "[DEBUG] LValValue 반환: 변수 = " << varName << endl;
        return Value(loadedValue); // 로드된 값을 반환
    }

    cerr << "[ERROR] LValValue: 예상치 못한 토큰 " << token.value << " (유형: " << token.type << ")" << endl;
    return Value(0); // 기본값 반환
}

// LOrExp → LAndExp { '||' LAndExp }
void parseLOrExp() {
    parseLAndExp();
    outputFile << "<LOrExp>" << endl;

    // OR 평가 결과를 위한 label 추적
    int endOfOrChainIndex = -1;

    while (getCurrentToken().type == OR) {
        outputToken(getCurrentToken());
        advanceToken();

        // 여기서 스택 top이 1(참)일 경우 OR 결과는 이미 참이므로 나머지를 평가할 필요 없이 종료
        // NOT 명령어를 emit해서 참->거짓, 거짓->참 으로 뒤집은 뒤 JPC 사용
        emitPcode("NOT", "", getCurrentToken().lineNumber);

        int jpcIndex = pcodeList.size();
        emitPcode("JPC", "0", getCurrentToken().lineNumber);  // 이제 top이 참이었다면(원래 참을 NOT하니 거짓=0) 점프하게 되어 끝으로 가게 됨.

        // 원래 조건이 참이면 여기서 점프해버릴 것이고, 그렇지 않으면 다음 LAndExp 평가
        parseLAndExp();

        // OR 체인의 끝에 점프하는 JMP를 등록
        if (endOfOrChainIndex == -1) {
            endOfOrChainIndex = pcodeList.size();
            emitPcode("JMP", "0", getCurrentToken().lineNumber);
        }

        // 이전 JPC 명령어의 점프 위치 백패치(다음 조건 평가 끝난 후 지점)
        pcodeList[jpcIndex].setoperand(to_string(pcodeList.size()));
    }

    // OR 평가 끝난 후 endOfOrChainIndex 백패치
    if (endOfOrChainIndex != -1) {
        pcodeList[endOfOrChainIndex].setoperand(to_string(pcodeList.size()));
    }

    // 마지막으로 OR 연산 결과(0 또는 1)가 스택 top에 남아있어야 함.
    // 현재 코드에서는 별도로 pop하지 않고 그대로 두면 이전 LAndExp, EqExp들이 남긴 결과가 top에 있을 것.
    // 꼭 필요하다면 여기서 스택 top이 결과임을 확실히 할 수 있는 주석 추가.
}

// LAndExp → EqExp { '&&' EqExp }
void parseLAndExp() {
    parseEqExp();

    while (getCurrentToken().type == AND) {
        Token andToken = getCurrentToken();
        advanceToken();

        // 현재 top이 0이면 (거짓) 다음 조건 평가를 건너뛰어야 함
        // 다음 조건 평가 코드(즉 parseEqExp() 실행 결과로 나온 P코드) 앞에 JPC를 넣어야 함
        int jpcIndex = pcodeList.size();
        emitPcode("JPC", "0", andToken.lineNumber);

        // 여기서 parseEqExp()를 호출하면, top이 1(참)일 때만 여기에 도달하게 됨(거짓이면 점프하므로)
        parseEqExp();

        // 이제 JPC가 거짓일 때 건너뛸 위치를 backpatch
        pcodeList[jpcIndex].setoperand(to_string(pcodeList.size()));
    }
}

// RelExp → AddExp | RelExp ('<' | '>' | '<=' | '>=') AddExp 
void parseRelExp() {
    parseAddExp();
    outputFile << "<RelExp>" << endl;

    while (getCurrentToken().type == LSS || getCurrentToken().type == LEQ || getCurrentToken().type == GRE || getCurrentToken().type == GEQ) {
        Token token = getCurrentToken();
        outputToken(token);
        advanceToken();
        parseEqExp();

        if (token.type == LSS) {
            emitPcode("LT", "", token.lineNumber);
        }
        else if (token.type == LEQ) {
            emitPcode("LE", "", token.lineNumber);
        }
        else if (token.type == GRE) {
            emitPcode("GT", "", token.lineNumber);
        }
        else if (token.type == GEQ) {
            emitPcode("GE", "", token.lineNumber);
        }
    }
}

// EqExp → RelExp | EqExp ('==' | '!=') RelExp 
void parseEqExp() {
    parseRelExp();
    outputFile << "<EqExp>" << endl;

    while (getCurrentToken().type == EQL || getCurrentToken().type == NEQ) {
        Token token = getCurrentToken();
        outputToken(token);
        advanceToken();
        parseRelExp();

        if (token.type == EQL) {
            emitPcode("EQ", "", token.lineNumber);
        }
        else if (token.type == NEQ) {
            emitPcode("NE", "", token.lineNumber);
        }
    }
}

// Exp → AddExp 
int parseExp() {
    int value = parseAddExp();
    outputFile << "<Exp>" << endl;
    return value;
}

Value ExpValue() {
    Value value = AddValue();
    outputFile << "<Exp>" << endl;
    return value;
}

// AddExp → MulExp | AddExp ('+' | '?') MulExp 
int parseAddExp() {

    int value = parseMulExp();

    while (getCurrentToken().type == PLUS || getCurrentToken().type == MINU) {
        Token opToken = getCurrentToken();
        outputToken(opToken);
        advanceToken();
        int nextvalue = parseMulExp();

        if (opToken.type == PLUS) {
            value += nextvalue;
            emitPcode("ADD", "", opToken.lineNumber);
        }
        else if (opToken.type == MINU) {
            value -= nextvalue;
            emitPcode("SUB", "", opToken.lineNumber);
        }
    }

    outputFile << "<AddExp>" << endl;
    return value;
}

Value AddValue() {
    Value value1 = MulValue();
    int val = value1.getValue();

    outputFile << "<AddExp>" << endl;

    while (getCurrentToken().type == PLUS || getCurrentToken().type == MINU) {
        Token opToken = getCurrentToken();
        outputToken(opToken);
        advanceToken();

        Value value2 = MulValue();
        int nextVal = value2.getValue();

        if (opToken.type == PLUS) {
            val += nextVal;
            emitPcode("ADD", "", opToken.lineNumber);
        }
        else if (opToken.type == MINU) {
            val -= nextVal;
            emitPcode("SUB", "", opToken.lineNumber);
        }
    }

    return Value(val);
}

// MulExp →  MulExp → UnaryExp | MulExp ('*' | '/' | '%') UnaryExp
int parseMulExp() {

    int value = parseUnaryExp();

    while (getCurrentToken().type == MULT || getCurrentToken().type == DIV || getCurrentToken().type == MOD) {
        Token opToken = getCurrentToken();
        outputToken(opToken);
        advanceToken();

        int nextvalue = parseUnaryExp();

        if (opToken.type == MULT) {
            emitPcode("MUL", "", opToken.lineNumber);
        }
        else if (opToken.type == DIV) {
            emitPcode("DIV", "", opToken.lineNumber);
        }
        else if (opToken.type == MOD) {
            emitPcode("MOD", "", opToken.lineNumber);
        }
    }

    outputFile << "<MulExp>" << endl;
    return value;
}

Value MulValue() {
    Value value1 = UnaryExp();
    int val = value1.getValue();
    cout << "" << val << endl;
    outputFile << "<MulExp>" << endl;

    while (getCurrentToken().type == MULT || getCurrentToken().type == DIV || getCurrentToken().type == MOD) {
        Token opToken = getCurrentToken();
        outputToken(opToken);
        advanceToken();

        Value value2 = UnaryExp();
        int nextVal = value2.getValue();
        cout << "" << nextVal << endl;

        if (opToken.type == MULT) {
            val *= nextVal;
            emitPcode("MUL", "", opToken.lineNumber);
        }
        else if (opToken.type == DIV) {
            if (nextVal != 0) {
                emitPcode("DIV", "", opToken.lineNumber);
            }
            else {
                cout << "Error: Division by zero at line " << opToken.lineNumber << endl;
                emitPcode("DIV", "", opToken.lineNumber);
            }
        }
        else if (opToken.type == MOD) {
            if (nextVal != 0) {
                emitPcode("MOD", "", opToken.lineNumber);
            }
            else {
                cout << "Error: Modulus by zero at line " << opToken.lineNumber << endl;
                emitPcode("MOD", "", opToken.lineNumber);
            }
        }
    }
    return Value(val);
}

// UnaryOp → '+' | '?' | '!' 注：'!'?出?在?件表?式中 
void parseUnaryOp(Token& opToken) {
    Token token = getCurrentToken();

    if (token.type == PLUS || token.type == MINU || token.type == NOT)
    {
        opToken = token; // 연산자 토큰 저장
        outputToken(token);
        advanceToken();
    }
    outputFile << "<UnaryOp>" << endl;
}

// UnaryExp → PrimaryExp | Ident '(' [FuncRParams] ')' | UnaryOp UnaryExp // j
int parseUnaryExp() {

    Token token = getCurrentToken();
    int value = 0;

    // UnaryOp UnaryExp
    if (token.type == PLUS || token.type == MINU || token.type == NOT) {
        cout << "[디버그] 단항 연산자 발견: 연산자 = " << token.value << ", 라인 = " << token.lineNumber << endl;

        Token opToken;
        parseUnaryOp(opToken);

        int operandValue = parseUnaryExp(); // 피연산자 파싱
        cout << "[디버그] 단항 연산자 피연산자 값: " << operandValue << endl;

        // 연산자에 대한 P-code 생성
        if (opToken.type == MINU) {
            emitPcode("LIT", "-1", opToken.lineNumber); // 스택에 -1 푸시
            emitPcode("MUL", "", opToken.lineNumber);   // 곱셈으로 부호 반전
            cout << "[디버그] MINU 연산 처리 완료" << endl;
        }
        else if (opToken.type == NOT) {
            emitPcode("NOT", "", opToken.lineNumber);   // 논리 부정
            cout << "[디버그] NOT 연산 처리 완료" << endl;
        }
    }

    // Ident '(' [FuncRParams] ')' 처리
    else if (token.type == IDENFR) {
        cout << "[디버그] 식별자 발견: 이름 = " << token.value << ", 라인 = " << token.lineNumber << endl;

        // 함수 호출인지 확인
        if (tokenIndex + 1 < tokens.size() && tokens[tokenIndex + 1].type == LPARENT) {
            cout << "[디버그] 함수 호출로 판단: 함수 이름 = " << token.value << endl;

            if (!isSymbolDefined(token.value)) {
                cout << "[디버그] 오류: 정의되지 않은 함수 = " << token.value << endl;
                parseErrorPrintC(token);
            }

            string currentFuncName = token.value; // 함수 이름을 지역 변수로 저장
            outputToken(getCurrentToken());
            advanceToken();
            outputToken(getCurrentToken());
            advanceToken();

            // 함수 호출 시 전달되는 매개변수 타입을 저장할 벡터 초기화
            vector<string> passedParamTypes;
            vector<int> passedParamValues;
            size_t paramCount = 0;

            // [FuncRParams] 처리
            if (getCurrentToken().type != RPARENT) {
                cout << "[디버그] 함수 매개변수 파싱 시작" << endl;
                paramCount = parseFuncRParams(passedParamTypes, passedParamValues);
                cout << "[디버그] 함수 매개변수 파싱 완료: 개수 = " << paramCount << endl;
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
                    cout << "[디버그] 함수 매개변수 개수 불일치: 기대값 = " << expectedParamTypes.size() << ", 실제값 = " << paramCount << endl;
                    parseErrorPrintD(token); // 매개변수 개수 불일치
                }
                else {
                    bool typeMismatchFound = false;
                    for (size_t i = 0; i < paramCount; ++i) {
                        if (!areTypesCompatible(expectedParamTypes[i], passedParamTypes[i])) {
                            cout << "[디버그] 매개변수 타입 불일치: 인덱스 = " << i << ", 기대 타입 = " << expectedParamTypes[i] << ", 실제 타입 = " << passedParamTypes[i] << endl;
                            parseErrorPrintE(); // 매개변수 타입 불일치 에러
                            typeMismatchFound = true;
                            break;
                        }
                    }

                    if (!typeMismatchFound) {
                        emitPcode("CAL", currentFuncName, token.lineNumber); // 함수 호출
                    }
                }
            }
            else {
                cout << "[디버그] 오류: 왐마 시발 = " << currentFuncName << endl;
            }
        }
        else {
            value = parsePrimaryExp();
        }
    }

    // PrimaryExp
    else {
        value = parsePrimaryExp();
    }

    outputFile << "<UnaryExp>" << endl;

    return value; // 실제 계산을 하지 않으므로 0을 반환
}

Value UnaryExp() {
    Token token = getCurrentToken();

    // 단항 연산자 처리
    if (token.type == PLUS || token.type == MINU || token.type == NOT) {
        outputToken(token);
        advanceToken();

        Value operand = UnaryExp(); // 피연산자 파싱
        int val = operand.getValue(); // 피연산자의 값 (컴파일 타임 값일 경우)

        if (token.type == MINU) {
            emitPcode("LIT", "-1", token.lineNumber); // 스택에 -1 푸시
            emitPcode("MUL", "", token.lineNumber);    // -1과 곱하여 부호 반전
            val = -val;
        }
        else if (token.type == NOT) {
            emitPcode("NOT", "", token.lineNumber);    // 논리 부정
            val = (val == 0) ? 1 : 0;
        }
        return Value(val);
    }

    // 함수 호출 처리
    else if (token.type == IDENFR && (tokenIndex + 1 < tokens.size()) && tokens[tokenIndex + 1].type == LPARENT) {
        string funcName = token.value;
        outputToken(token);
        advanceToken(); // 함수 이름 토큰 소비

        outputToken(getCurrentToken()); // '(' 출력
        advanceToken(); // '(' 토큰 소비

        // 함수 매개변수 파싱
        vector<string> passedParamTypes;
        vector<int> passedParamValues;  // 전달된 매개변수 값

        size_t paramCount = 0;

        if (getCurrentToken().type != RPARENT) {
            paramCount = parseFuncRParams(passedParamTypes, passedParamValues);
        }

        // ')' 처리
        if (getCurrentToken().type == RPARENT) {
            outputToken(getCurrentToken());
            advanceToken();
        }
        else {
            parseErrorPrintfJ();
        }

        // 매개변수 타입 검증
        if (functionParamsTypes.find(funcName) != functionParamsTypes.end()) {
            const vector<string>& expectedParamTypes = functionParamsTypes[funcName];

            if (paramCount != expectedParamTypes.size()) {
                cout << "[디버그] 함수 매개변수 개수 불일치: 기대값 = " << expectedParamTypes.size() << ", 실제값 = " << paramCount << endl;
                parseErrorPrintD(token);
            }
            else {
                bool typeMismatch = false;
                for (size_t i = 0; i < paramCount; ++i) {
                    if (!areTypesCompatible(expectedParamTypes[i], passedParamTypes[i])) {
                        cout << "[디버그] 매개변수 타입 불일치: 인덱스 = " << i << ", 기대 타입 = " << expectedParamTypes[i] << ", 실제 타입 = " << passedParamTypes[i] << endl;
                        parseErrorPrintE();
                        typeMismatch = true;
                        break;
                    }
                }

                if (!typeMismatch) {
                    // 함수의 주소를 심볼 테이블에서 가져오기
                    int funcAddr = getSymbolAddress(funcName);
                    if (funcAddr == -1) {
                        cout << "[디버그] 오류: 함수 " << funcName << "의 주소를 찾을 수 없습니다." << endl;
                    }
                    else {

                        emitPcode("CAL", funcName, token.lineNumber); // 함수 호출
                        cout << "[디버그] 함수 호출 완료: 함수 이름 = " << funcName << ", 주소 = " << funcAddr << endl;
                    }
                }
            }
        }
        else {
            cout << "[디버그] 오류: 정의되지 않은 함수 " << funcName << endl;
        }
        return Value(0); // 함수 반환값 필요 시 수정
    }

    // PrimaryExp 처리
    else {
        cout << "[디버그] PrimaryExp 호출: 토큰 = " << token.value << ", 타입 = " << tokenTypeToString(token.type) << endl;
        Value primaryValue = PrimaryExp();
        cout << "[디버그] PrimaryExp 반환값: " << primaryValue.getValue() << endl;
        return primaryValue;
    }
}

// PrimaryExp → '(' Exp ')' | LVal | Number | Character // j
int parsePrimaryExp() {
    int value = 0;

    // '(' Exp ')'
    if (getCurrentToken().type == LPARENT) {
        outputToken(getCurrentToken());  // '(' 출력
        advanceToken();

        value = parseExp();  // Exp 파싱

        // ')' 확인
        if (getCurrentToken().type == RPARENT) {
            outputToken(getCurrentToken());
            advanceToken();
        }
        else {
            parseErrorPrintfJ();
        }
    }
    // LVal 처리
    else if (getCurrentToken().type == IDENFR) {
        value = parseLVal();
    }
    // Number 처리
    else if (getCurrentToken().type == INTCON) {
        Token token = getCurrentToken();  // 현재 토큰 저장
        outputToken(token);
        currentBaseType = "Int";  // 상수는 Int로 처리
        value = stoi(token.value);  // 토큰 값 변환
        emitPcode("LIT", token.value, token.lineNumber); // **LIT 명령어 생성**
        advanceToken();  // 다음 토큰으로 이동
        outputFile << "<Number>" << endl;

        cout << "INTCON 파싱 완료 - 값: " << value << endl;
    }
    // Character 처리
    else if (getCurrentToken().type == CHRCON) {
        Token token = getCurrentToken();  // 현재 토큰 저장
        outputToken(token);
        currentBaseType = "Char";  // 상수는 Char로 처리
        value = static_cast<int>(token.value[1]);  // 문자를 정수로 변환
        emitPcode("LIT", std::to_string(value), token.lineNumber); // **LIT 명령어 생성**
        advanceToken();  // 다음 토큰으로 이동
        outputFile << "<Character>" << endl;

        cout << "CHRCON 파싱 완료 - 값: " << value << endl;
    }

    outputFile << "<PrimaryExp>" << endl;

    return value;
}

Value PrimaryExp() {
    Token token = getCurrentToken();

    if (token.type == LPARENT) {
        outputToken(token);
        advanceToken();

        Value value = ExpValue();

        token = getCurrentToken();
        if (token.type == RPARENT) {
            outputToken(token);
            advanceToken();
        }
        else {
            parseErrorPrintfJ();
        }

        return value;
    }

    else if (token.type == IDENFR) {
        Value value = parseLVal();
        return value;
    }

    else if (token.type == INTCON) {
        Token token = getCurrentToken();
        outputToken(token); // 현재 토큰 출력
        advanceToken();     // 다음 토큰으로 이동

        int intValue = stoi(token.value); // 숫자 변환
        emitPcode("LIT", token.value, token.lineNumber); // LIT P-code 생성

        outputFile << "<Number>" << endl;
        return Value(intValue);
    }

    else if (token.type == CHRCON) {
        Token token = getCurrentToken();
        outputToken(token); // 현재 토큰 출력
        advanceToken();     // 다음 토큰으로 이동

        int charValue = static_cast<int>(token.value[1]); // 문자 -> ASCII 변환
        emitPcode("LIT", std::to_string(charValue), token.lineNumber); // LIT P-code 생성

        outputFile << "<Character>" << endl;
        return Value(charValue);
    }
    else {
        cerr << "[에러] 예상치 못한 토큰: 값 = " << token.value << ", 타입 = " << tokenTypeToString(token.type) << ", 라인 = " << token.lineNumber << endl;
        return Value(0);
    }
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

        // 'JMP' 명령어를 생성하고, 오퍼랜드는 나중에 백패치하기 위해 0으로 설정
        int breakIdx = emitPcode("JMP", "0", getCurrentToken().lineNumber);
        if (!breakPatchesStack.empty()) {
            breakPatchesStack.top().push_back(breakIdx); // 현재 루프의 break 패치 리스트에 추가
        }
        else {
            cerr << "[DEBUG] 에러: 유효한 루프 컨텍스트 외부에서 'break'를 사용하려고 시도했습니다." << endl;
        }

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
        token = getCurrentToken();

        // 'JMP' 명령어를 생성하여 조건 검사 위치로 이동하도록 수정
        if (!continuePatchesStack.empty() && !conditionCheckIdxStack.empty()) {
            int conditionIdx = conditionCheckIdxStack.top();
            int continueIdx = emitPcode("JMP", to_string(conditionIdx), getCurrentToken().lineNumber); // 조건 검사 위치로 이동
            continuePatchesStack.top().push_back(continueIdx); // 현재 루프의 continue 패치 리스트에 추가
            cout << "[DEBUG] 'continue' 처리: JMP " << conditionIdx << " 생성 (continueIdx: " << continueIdx << ")" << endl;
        }
        else {
            cerr << "!!!!!!!!!!!!! 루프 외부에서 'continue' 사용 시도 !!!!!!!!!!!!!!!!!!!!!" << endl;
        }

        if (token.type == SEMICN) {
            outputToken(token);
            advanceToken();
        }
        else {
            parseErrorPrintfI();
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

        breakPatchesStack.push(vector<int>());
        continuePatchesStack.push(vector<int>());

        // 초기화 식 처리
        if (getCurrentToken().type != SEMICN) {
            parseForStmt(); // 초기화 식 파싱
        }

        if (getCurrentToken().type == SEMICN) {
            outputToken(getCurrentToken());
            advanceToken();
        }

        // 조건 검사 위치 설정
        int conditionCheckIdx = pcodeList.size();
        conditionCheckIdxStack.push(conditionCheckIdx);

        // 조건 식 처리
        int jmpToEndIndex = -1;

        if (getCurrentToken().type != SEMICN) {
            parseCond(); // 조건 식 파싱
            jmpToEndIndex = emitPcode("JPC", "0", getCurrentToken().lineNumber); // 조건 거짓일 때 루프 종료로 점프
        }
        else {
            emitPcode("LIT", "1", getCurrentToken().lineNumber); // 항상 참인 조건 생성
            jmpToEndIndex = emitPcode("JPC", "0", getCurrentToken().lineNumber); // 항상 참이므로 점프는 실행되지 않음
        }

        if (getCurrentToken().type == SEMICN) {
            outputToken(getCurrentToken());
            advanceToken();
        }

        // 증감식 처리
        int incrementStartIdx = pcodeList.size();
        if (getCurrentToken().type != RPARENT) {
            parseForStmt();
        }

        // ')' 처리
        if (getCurrentToken().type == RPARENT) {
            outputToken(getCurrentToken());
            advanceToken();
        }

        // 루프 본문 시작 위치 저장
        int loopBodyStart = pcodeList.size();

        // 루프 본문 처리
        bool previousLoopContext = inLoopContext;
        inLoopContext = true;

        parseStmt(); // 루프 본문 파싱
        inLoopContext = previousLoopContext;

        // continue 문 백패치 (조건 검사로 점프하도록 수정)
        vector<int>& continuePatches = continuePatchesStack.top();
        for (int idx : continuePatches) {
            pcodeList[idx].setoperand(to_string(conditionCheckIdxStack.top())); // 조건 검사 위치로 점프
        }
        continuePatchesStack.pop();

        // 증감식 P-code를 실행하고 조건 검사로 점프
        emitPcode("JMP", to_string(conditionCheckIdxStack.top()), getCurrentToken().lineNumber);
        conditionCheckIdxStack.pop(); // 조건 검사 인덱스 스택에서 현재 루프의 값을 제거

        // 루프 종료 위치 설정
        int loopEnd = pcodeList.size();
        cout << "[DEBUG] 루프 종료 위치 설정: P-code 인덱스 " << loopEnd << endl;
        if (jmpToEndIndex != -1) {
            pcodeList[jmpToEndIndex].setoperand(to_string(loopEnd));
        }

        // 'break' 문 백패치
        if (!breakPatchesStack.empty()) {
            vector<int>& breakPatches = breakPatchesStack.top();
            for (int idx : breakPatches) {
                pcodeList[idx].setoperand(to_string(loopEnd)); // 루프 종료 위치로 점프
                std::cout << "[DEBUG] 'break' 백패치: 인덱스 " << idx << " -> " << loopEnd << std::endl;
            }
            breakPatchesStack.pop(); // 현재 루프의 'break' 패치 리스트 제거
        }
    }

    // 'return' [Exp] ';' 처리
    else if (token.type == RETURNTK) {
        outputToken(token);
        advanceToken();

        token = getCurrentToken(); // 다음 토큰 가져오기

        if (token.type == SEMICN) {
            outputToken(token);
            advanceToken();

            if (currentReturnType != "void") {
                parseErrorPrintF(token);
            }

            // P-code: 반환값 없음, 단순히 함수 종료
            emitPcode("RET_VOID", "", token.lineNumber);
            cout << "return 문이 함수 종료." << endl;
        }

        else if (isExpressionStartToken(token.type)) {
            // 'return' 뒤에 표현식이 오는 경우
            if (currentReturnType == "void") {
                parseErrorPrintF(token);
            }
            parseExp();

            // P-code: 반환값을 반환 레지스터에 저장
            emitPcode("RET", "", token.lineNumber);

            token = getCurrentToken(); // 표현식 파싱 후 토큰 업데이트
            if (token.type == SEMICN) {
                outputToken(token);
                advanceToken();
            }
            else {
                parseErrorPrintfI();
            }

            cout << "return 문이 함수 종료." << endl;
        }
        else {
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

        int jzPcodeIndex = pcodeList.size();
        emitPcode("JPC", "0", getCurrentToken().lineNumber);

        parseStmt();  // 본문 Stmt 처리

        if (getCurrentToken().type == ELSETK) {
            outputToken(getCurrentToken());
            advanceToken();

            int jmpPcodeIndex = pcodeList.size();
            emitPcode("JMP", "0", getCurrentToken().lineNumber);

            pcodeList[jzPcodeIndex].setoperand(to_string(pcodeList.size()));
            parseStmt();  // else 구문 처리
            pcodeList[jmpPcodeIndex].setoperand(to_string(pcodeList.size()));
        }

        else {
            // 'else' 블록이 없는 경우, JZ 명령어가 'if' 블록의 끝으로 점프하도록 백패치
            pcodeList[jzPcodeIndex].setoperand(to_string(pcodeList.size()));
        }

    }

    // printf‘(’ FormatString {, Exp}’)’ ‘;’
    else if (token.type == PRINTFTK) {
        outputToken(token);
        advanceToken();

        if (getCurrentToken().type == LPARENT) {
            outputToken(getCurrentToken());
            advanceToken();
        }

        int formatSpecifierCount = 0;
        int expressionCount = 0;
        string formatString = "";
        vector<string> expressions;

        // 포맷 문자열 확인
        if (getCurrentToken().type == STRCON) {
            formatString = getCurrentToken().value;
            formatSpecifierCount = countFormatSpecifiers(formatString); // 포맷 specifier 개수 세기
            outputToken(getCurrentToken());
            advanceToken();
        }

        // 표현식들 확인
        while (getCurrentToken().type == COMMA) {
            outputToken(getCurrentToken());
            advanceToken();
            parseExp();
            expressions.push_back(""); // 표현식 개수를 추적하기 위해 빈 문자열 추가

            expressionCount++;
        }

        if (expressionCount != formatSpecifierCount) {
            Token printfToken = getPreviousToken();
            parseErrorPrintL();
        }

        // RPARENT ')' 처리 
        if (getCurrentToken().type == RPARENT) {
            outputToken(getCurrentToken());
            advanceToken();
        }
        else {
            parseErrorPrintfJ();
        }

        // SEMICOLON ';' 처리
        if (getCurrentToken().type == SEMICN) {
            outputToken(getCurrentToken());
            advanceToken();
        }
        else {
            parseErrorPrintfI();
        }

        vector<pair<bool, string>> formatParts;
        string s = formatString;
        s.erase(remove(s.begin(), s.end(), '\"'), s.end()); // 따옴표 제거

        size_t pos = 0;
        size_t prevPos = 0;
        int exprIndex = 0;

        while (pos < s.length()) {
            if (s[pos] == '%') {
                if (pos + 1 < s.length() && (s[pos + 1] == 'd' || s[pos + 1] == 'c')) {
                    // 포맷 지정자 이전의 텍스트 부분 추가
                    if (pos > prevPos) {
                        string text = s.substr(prevPos, pos - prevPos);
                        formatParts.push_back({ false, text });
                        cout << "[DEBUG] 텍스트 추가: " << text << endl;
                    }
                    // 포맷 지정자 추가 (%d 또는 %c)
                    formatParts.push_back({ true, s.substr(pos, 2) });
                    cout << "[DEBUG] 포맷 지정자 추가: " << s.substr(pos, 2) << endl;
                    pos += 2; // %d 또는 %c를 건너뜀
                    prevPos = pos;
                }
                else {
                    // 지원하지 않는 포맷 지정자는 넘어감
                    pos++;
                }
            }
            else if (s[pos] == '\\' && pos + 1 < s.length() && s[pos + 1] == 'n') {
                // 개행 문자 처리
                pos += 2;
            }
            else {
                pos++;
            }
        }

        // 남은 텍스트 부분 추가
        if (prevPos < s.length()) {
            string text = s.substr(prevPos);
            formatParts.push_back({ false, text });
        }

        // P-code 명령어 생성
        for (auto& part : formatParts) {
            if (part.first == false) {
                // 텍스트 부분
                // 이스케이프 시퀀스 처리
                string text = part.second;
                size_t pos = 0;
                while ((pos = text.find("\\n", pos)) != string::npos) {
                    text.replace(pos, 2, "\n");
                    pos += 1;
                }
                emitPcode("WRTS", text, getCurrentToken().lineNumber);
            }
            else {
                // 포맷 지정자 부분
                if (part.second == "%d") {
                    emitPcode("WRT", to_string(expressionCount), getCurrentToken().lineNumber);
                }
                else if (part.second == "%c") {
                    emitPcode("WRT_C", to_string(expressionCount), getCurrentToken().lineNumber);
                }
            }
        }
    }

    // Block 처리
    else if (token.type == LBRACE) {
        parseBlock();
    }

    else if (token.type == SEMICN) {
        outputToken(token);
        advanceToken();
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
            if (token.type == GETINTTK) {
                outputToken(token);
                advanceToken();

                // '(' 처리
                if (getCurrentToken().type == LPARENT) {
                    outputToken(getCurrentToken());
                    advanceToken();
                }
                else {
                    parseErrorPrintfJ();
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

                // 'getint'에 대한 P-code 생성
                int lvalAddr = getSymbolAddress(lvalName);
                emitPcode("READ_INT", "", token.lineNumber); // 입력 값을 스택에 푸시
                emitPcode("STO", to_string(lvalAddr), token.lineNumber); // 스택의 값을 변수에 저장
                cout << "디버그: 변수 " << lvalName << "에 getint() 결과 저장 - 주소: " << lvalAddr << endl;
            }

            // 'getchar' 처리
            else if (token.type == GETCHARTK) {
                outputToken(token);
                advanceToken();

                // '(' 처리
                if (getCurrentToken().type == LPARENT) {
                    outputToken(getCurrentToken());
                    advanceToken();
                }
                else {
                    parseErrorPrintfJ();
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

                // 'getchar'에 대한 P-code 생성
                int lvalAddr = getSymbolAddress(lvalName);
                emitPcode("READ_CHAR", "", token.lineNumber); // 입력 값을 스택에 푸시
                emitPcode("STO", to_string(lvalAddr), token.lineNumber); // 스택의 값을 변수에 저장
                cout << "디버그: 변수 " << lvalName << "에 getchar() 결과 저장 - 주소: " << lvalAddr << endl;
            }

            // LVal '=' Exp ';' 처리
            else {
                Value exp = ExpValue();
                // 스택에서 값을 저장할 변수 주소를 찾음
                int lvalAddr = getVariableAddress(lvalName);
                int value = exp.getValue();

                cout << "업데이트를 할 lvalAddr: " << lvalAddr << endl;
                dstack[lvalAddr] = value; // **dstack에 직접 저장**
                cout << "[DEBUG] 메모리 업데이트: dstack[" << lvalAddr << "] = " << dstack[lvalAddr] << endl;

                emitPcode("STO", to_string(lvalAddr), token.lineNumber);
                cout << "디버그: 변수 " << lvalName << " 업데이트 - 주소: " << lvalAddr << ", 값: " << value << endl;

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

// ForStmt → LVal '=' Exp 
void parseForStmt() {
    parseLVal(); // 변수 이름 파싱
    Token lvalToken = getPreviousToken(); // 이전 토큰
    string lvalName = lvalToken.value; // 변수 이름
    int lvalAddr = getVariableAddress(lvalName); // 변수 주소를 심볼 테이블에서 가져옴

    cout << "[DEBUG] parseForStmt: 변수 이름=" << lvalName << ", 주소=" << lvalAddr << endl;

    if (getCurrentToken().type == ASSIGN) {
        outputToken(getCurrentToken());
        advanceToken();
    }
    else {
        advanceToken();
    }

    // 상수 변수인지 확인
    if (isConstLVal(lvalName)) {
        parseErrorPrintH(); // 상수를 변경하려고 하면 오류
    }

    parseExp(); // 표현식 파싱 (값 푸시)

    emitPcode("STO", to_string(lvalAddr), getCurrentToken().lineNumber); // 값 저장
    cout << "[DEBUG] STO: 변수=" << lvalName << ", 주소=" << lvalAddr << "에 값 저장" << endl;

    // 나머지 초기화 식 처리
    while (getCurrentToken().type == COMMA) {
        outputToken(getCurrentToken());
        advanceToken();

        parseLVal();
        lvalToken = getPreviousToken(); // 변수 업데이트
        lvalName = lvalToken.value;
        lvalAddr = getVariableAddress(lvalName); // 여기서도 변수 주소를 심볼 테이블에서 가져옴

        cout << "[DEBUG] parseForStmt: 변수 이름=" << lvalName << ", 주소=" << lvalAddr << endl;

        if (getCurrentToken().type == ASSIGN) {
            outputToken(getCurrentToken());
            advanceToken();
        }
        else {
            advanceToken();
        }

        parseExp();
        emitPcode("STO", to_string(lvalAddr), getCurrentToken().lineNumber); // 값 저장
        cout << "[DEBUG] STO: 변수=" << lvalName << ", 주소=" << lvalAddr << "에 값 저장" << endl;
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
string parseFuncFParam(string& paramType, const string& funcName) {
    // 매개변수의 기본 타입 파싱
    parseBType();
    Token token = getCurrentToken();
    string paramName;

    // 현재 파싱한 타입을 매개변수 타입으로 설정
    paramType = currentBaseType;

    // 매개변수 이름 파싱
    if (token.type == IDENFR) {
        paramName = token.value;
        outputToken(token);
        advanceToken();
    }
    else {
        // 오류 처리: 매개변수 이름이 아님
        parseErrorPrintC(token);
    }

    // 배열 여부 체크
    bool isArray = false;

    if (getCurrentToken().type == LBRACK) {
        outputToken(getCurrentToken());
        advanceToken();

        if (getCurrentToken().type == RBRACK) {
            outputToken(getCurrentToken());
            advanceToken();
            isArray = true;
            paramType += "Array";
        }
        else {
            parseErrorPrintfK(); // ']' 누락 에러
        }
    }

    // 스코프별 nextAddress 사용
    int paramAddr = getNextAddress();

    // 디버그 로그 추가
    cout << "[DEBUG] 함수 '" << funcName << "' 매개변수: 이름='" << paramName << "', 타입='" << paramType << "', 배열="
        << (isArray ? "True" : "False") << ", 주소=" << paramAddr << endl;

    // 중복 확인 및 심볼 테이블에 추가
    if (isSymbolDefinedInCurrentScope(paramName)) {
        parseErrorPrintB(token); // 동일 스코프에서 중복된 변수 이름이 있을 경우 에러 출력
    }
    else {
        // 새로운 매개변수 심볼 추가
        Symbol paramSymbol;
        paramSymbol.name = paramName;
        paramSymbol.type = paramType;
        paramSymbol.address = paramAddr;
        paramSymbol.scopelineNumber = CurrentScope; // 현재 스코프 번호
        paramSymbol.isArray = isArray;
        paramSymbol.isFunction = false;
        symbolTable.push_back(paramSymbol);

        // functionParams 맵에 매개변수 저장
        functionParams[funcName].emplace_back(make_pair(paramName, paramAddr));
        cout << "[DEBUG] 매개변수 '" << paramName << "' 추가됨. 주소: " << paramSymbol.address << ", 타입: " << paramType << endl;
    }

    // 결과 출력
    outputFile << "<FuncFParam>" << endl;

    return paramName;
}

// FuncRParams → Exp { ',' Exp } 
size_t parseFuncRParams(vector<string>& passedParamTypes, vector<int>& passedParamValues) {

    size_t paramCount = 0;
    Value value = ExpValue();
    int paramValue = value.getValue();
    passedParamTypes.push_back(currentBaseType);  // 함수 매개변수 타입 저장
    passedParamValues.push_back(paramValue);      // 함수 매개변수 값 저장
    paramCount++;

    cout << "[디버그] 매개변수 " << paramCount << "번째: 타입 = " << currentBaseType << ", 값 = " << value << endl;

    while (getCurrentToken().type == COMMA) {

        outputToken(getCurrentToken()); // ',' 출력
        advanceToken();
        value = ExpValue();
        paramValue = value.getValue();

        passedParamTypes.push_back(currentBaseType); // 함수 매개변수 타입 저장
        passedParamValues.push_back(paramValue);     // 함수 매개변수 값 저장
        paramCount++;

        cout << "[디버그] 매개변수 " << paramCount << "번째: 타입 = " << currentBaseType << ", 값 = " << value << endl;
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
    cout << "\n현재 함수 리턴 타입 : " << currentReturnType << endl;

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
    string paramName = parseFuncFParam(paramType, funcName);

    // 매개변수 추가
    paramTypes.push_back(paramType);
    paramCount++;

    // 중복된 이름 있는지 체크 
    if (paramNames.find(paramName) == paramNames.end()) {
        paramNames.insert(paramName);
    }

    while (getCurrentToken().type == COMMA) {
        outputToken(getCurrentToken());
        advanceToken();

        paramName = parseFuncFParam(paramType, funcName); // 함수 이름 전달
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
    if (token.type == IDENFR) {
        string currentFuncName = token.value;

        // 함수 중복 정의 확인
        if (isSymbolDefinedInCurrentScope(currentFuncName)) {
            parseErrorPrintB(token); // 중복된 함수 정의
            advanceToken();

            // 중복 정의는 본문을 건너뛰고 종료
            while (getCurrentToken().type != RPARENT && tokenIndex < tokens.size()) {
                advanceToken();
            }

            if (getCurrentToken().type == RPARENT) {
                advanceToken();
            }
            parseBlock(); // 블록을 읽어서 파싱 완료
            outputFile << "<FuncDef>" << endl;
            return;
        }

        // 심볼 테이블에 함수 추가
        addSymbol(currentFuncName, currentReturnTypeLocal, true);

        // 함수 주소 기록 (현재 pcodeList.size())
        int funcAddr = pcodeList.size();
        functionMap[currentFuncName] = funcAddr;

        // 심볼 테이블에서 마지막으로 추가된 심볼 확인 및 주소 설정
        bool addedToSymbolTable = false;
        for (auto& sym : symbolTable) {
            if (sym.name == currentFuncName && sym.isFunction) {
                sym.address = funcAddr;
                addedToSymbolTable = true;
                cout << "[DEBUG] 함수 '" << currentFuncName << "'의 주소를 " << funcAddr << "로 설정했습니다." << endl;
                break;
            }
        }

        logSymbolTable();

        if (!addedToSymbolTable) {
            cerr << "[ERROR] 심볼 테이블에 함수 '" << currentFuncName << "' 추가 실패." << endl;
        }

        outputToken(token);
        advanceToken();

        // 매개변수 파싱 전 스코프 진입
        enterScope();

        // 매개변수 파싱
        vector<string> paramTypes;
        size_t paramCount = 0;

        if (getCurrentToken().type == LPARENT) {
            outputToken(getCurrentToken());
            advanceToken();

            if (getCurrentToken().type != RPARENT) {
                // 매개변수 파싱 시 함수 이름 전달
                paramCount = parseFuncFParams(paramTypes, currentFuncName);
            }

            if (getCurrentToken().type == RPARENT) {
                outputToken(getCurrentToken());
                advanceToken();
            }
            else {
                parseErrorPrintfJ(); // ')' 누락 에러
            }
        }

        // functionParamsTypes 맵에 매개변수 타입 저장
        functionParamsTypes[currentFuncName] = paramTypes;

        // 매개변수 주소는 parseFuncFParam 함수 내에서 저장됨 (functionParams 맵)

        cout << "[DEBUG] 함수 시작: 이름 = " << currentFuncName
            << ", 매개변수 개수 = " << paramCount << ", P-Code 생성 중" << endl;

        emitPcode("FUNC", currentFuncName, token.lineNumber);
        emitPcode("INT", to_string(paramCount), token.lineNumber);

        returnTypeStack.push(currentReturnType);
        currentReturnType = currentReturnTypeLocal;

        bool hasReturn = parseBlock();

        // 반환 타입 검사
        if (currentReturnTypeLocal != "void" && !hasReturn) {
            Token lastToken = getPreviousToken();
            parseErrorPrintG(lastToken); // 반환문 없는 함수
        }

        if (currentReturnTypeLocal == "void") {
            emitPcode("RET_VOID", "", token.lineNumber);
        }
        else {
            emitPcode("RET", "", token.lineNumber);
        }

        emitPcode("END_FUNC", currentFuncName, token.lineNumber);

        cout << "[DEBUG] 함수 종료: 이름 = " << currentFuncName
            << ", 리턴 타입 = " << currentReturnTypeLocal
            << ", 매개변수 개수 = " << paramCount << endl;

        outputFile << "<FuncDef>" << endl;

        // 스코프 종료
        currentReturnType = returnTypeStack.top();
        returnTypeStack.pop();
        exitScope();
    }
}

// Cond → LOrExp 
void parseCond() {
    parseLOrExp();
    outputFile << "<Cond>" << endl;
}

// ConstExp → AddExp  비고: 사용하는 Ident는 상수
int parseConstExp() {
    parseAddExp();
    outputFile << "<ConstExp>" << endl;
    return 0;
}

Value ConstExpValue() {
    Value value = AddValue();
    outputFile << "<ConstExp>" << endl;
    return value;
}

// ConstDef → Ident [ '[' ConstExp ']' ] '=' ConstInitVal // k
void parseConstDef() {
    Token token = getCurrentToken();
    string constName;
    int initValue = 0; // 초기화 값을 저장할 변수

    // Ident 처리
    if (token.type == IDENFR) {
        constName = token.value;
        cout << "디버그: 상수 이름 발견 - " << constName << ", 라인: " << token.lineNumber << endl;
        outputToken(token);
        advanceToken();
    }

    bool isArray = false;
    string declarationType = currentBaseType;
    int arraySize = 1;

    // '[' ConstExp ']' 처리 
    if (getCurrentToken().type == LBRACK) {
        outputToken(getCurrentToken());
        advanceToken();

        arraySize = parseConstExp();

        if (getCurrentToken().type == RBRACK) {
            outputToken(getCurrentToken());
            advanceToken();
            isArray = true;
        }
        else {
            parseErrorPrintfK();
        }
    }

    int constAddr = nextaddress;
    nextaddress += arraySize;
    cout << "상수 " << constName << " 주소 할당 - 주소: " << constAddr << ", 타입: " << declarationType
        << (isArray ? ("[" + to_string(arraySize) + "]") : "") << endl;
    // 배열 크기에 따라 P-code 발행
    emitPcode("INT", to_string(arraySize), token.lineNumber);

    // '=' 처리
    if (getCurrentToken().type == ASSIGN) {
        outputToken(getCurrentToken());
        advanceToken();

        // 초기화 값을 parseConstInitVal로부터 가져옴
        initValue = parseConstInitVal(constAddr);

        if (isArray) {
            // 배열 초기화 로그 출력
            for (int i = 0; i < arraySize; ++i) {
                cout << "디버그: 배열 상수 " << constName << "[" << i << "] 초기화 - 값: " << initValue << ", 주소: " << (constAddr + i) << ", 타입: " << declarationType << endl;
            }
        }
        else {
            // 단일 상수 초기화 로그
            cout << "디버그: 상수 " << constName << " 초기화 값: " << initValue << ", 주소: " << constAddr << ", 타입: " << declarationType << endl;
        }
    }
    else {
        if (isArray) {
            for (int i = 0; i < arraySize; ++i) {
                cout << "디버그: 배열 상수 " << constName << "[" << i << "] 기본값 0으로 초기화 - 주소: " << (constAddr + i) << ", 타입: " << declarationType << endl;
            }
        }
        else {
            cout << "디버그: 상수 " << constName << " 기본값 0으로 초기화 - 주소: " << constAddr << ", 타입: " << declarationType << endl;
        }
    }

    // 중복 검사: 현재 스코프에 동일한 이름의 상수가 있는지 확인
    if (isSymbolDefinedInCurrentScope(constName)) {
        parseErrorPrintB(token); // 중복 시 에러 'b' 출력
    }
    else {
        string typeName = SymbolType(true, declarationType, isArray, false); // 배열 여부 포함
        Token identToken = { IDENFR, constName, token.lineNumber };
        addSymbolWithCheck(identToken, typeName);

        // 심볼 테이블에서 해당 심볼의 address 필드를 업데이트
        bool updated = false;
        for (auto& sym : symbolTable) {
            if (sym.name == constName && sym.scopelineNumber == CurrentScope) {
                sym.address = constAddr;  // 주소 설정
                sym.isArray = isArray;
                sym.arraySize = arraySize;
                sym.value = initValue;  // 초기화 값 설정
                sym.isConst = true;
                updated = true;

                cout << "[DEBUG] 심볼 테이블 업데이트: 이름=" << sym.name
                    << ", 값=" << sym.value << ", 주소=" << sym.address << endl;
                break;
            }
        }

        if (!updated) {
            cout << "[DEBUG] Error: Constant " << constName << " not found in symbol table for current scope." << endl;
        }
    }

    outputFile << "<ConstDef>" << endl;
}

// ConstInitVal → ConstExp | '{' [ ConstExp { ',' ConstExp } ] '}' | StringConst
int parseConstInitVal(int baseAddr) {
    Token token = getCurrentToken();
    int value = 0; // 초기화 값을 저장할 변수

    // '{' [ ConstExp { ',' ConstExp } ] '}'
    if (getCurrentToken().type == LBRACE) {
        outputToken(getCurrentToken());
        advanceToken();

        int initIndex = 0; // 초기화 인덱스

        if (getCurrentToken().type != RBRACE) {

            // 첫 번째 ConstExp
            Value valObj = ConstExpValue();
            value = valObj.getValue();
            if (baseAddr != -1) {
                emitPcode("STO", to_string(baseAddr + initIndex), getCurrentToken().lineNumber);
                initIndex++;
            }

            // { ',' ConstExp }
            while (getCurrentToken().type == COMMA) {
                outputToken(getCurrentToken());
                advanceToken();

                valObj = ConstExpValue();
                value = valObj.getValue();
                if (baseAddr != -1) {
                    emitPcode("STO", to_string(baseAddr + initIndex), getCurrentToken().lineNumber);
                    initIndex++;
                }
            }
        }

        // '}' 처리
        if (getCurrentToken().type == RBRACE) {
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
        Value valObj = ConstExpValue();
        value = valObj.getValue();
        emitPcode("STO", to_string(baseAddr), getCurrentToken().lineNumber);
    }

    outputFile << "<ConstInitVal>" << endl;
    cout << "[DEBUG] parseConstInitVal 종료: 최종 값= " << value << endl;
    return value;
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
    int initValue = 0;

    // Ident (변수 이름) 확인
    if (token.type == IDENFR) {
        varName = token.value;
        cout << "디버그: 변수 이름 발견 - " << varName << ", 라인: " << token.lineNumber << endl;
        outputToken(token);
        advanceToken();
    }

    bool isArray = false;
    int arraySize = 1;

    // 배열 구문 처리: '[' ConstExp ']'
    if (getCurrentToken().type == LBRACK) {
        outputToken(getCurrentToken());
        advanceToken();
        arraySize = parseConstExp(); // ConstExp를 파싱하여 arraySize를 결정

        if (getCurrentToken().type == RBRACK) {
            outputToken(getCurrentToken());
            advanceToken();
            isArray = true;
        }
        else {
            parseErrorPrintfK();
        }
    }

    // 변수 주소 할당
    int varAddr = nextaddress;
    nextaddress += arraySize;
    cout << "변수 " << varName << " 주소 할당 - 주소: " << varAddr << ", 타입: " << baseType << (isArray ? ("[" + to_string(arraySize) + "]") : "") << endl;
    // 배열 크기에 따라 P-code 발행
    emitPcode("INT", to_string(arraySize), token.lineNumber);

    // 초기화 구문 처리: '=' InitVal
    if (getCurrentToken().type == ASSIGN) {
        outputToken(getCurrentToken());
        advanceToken();

        // 초기화 값을 parseInitVal로부터 가져옴
        initValue = parseInitVal(varAddr);

        if (isArray) {
            // 배열 초기화 로그 출력
            for (int i = 0; i < arraySize; ++i) {
                cout << "디버그: 배열 " << varName << "[" << i << "] 초기화 - 값: " << initValue << ", 주소: " << (varAddr + i) << ", 타입: " << baseType << endl;
                dstack[varAddr + i] = initValue;  // 배열 각 요소를 메모리에 저장
            }
        }
        else {
            // 단일 변수 초기화 로그
            cout << "디버그: 변수 " << varName << " 초기화 값: " << initValue << ", 주소: " << varAddr << ", 타입: " << baseType << endl;
            dstack[varAddr] = initValue;  // 단일 변수 초기화 값을 메모리에 저장
        }
    }
    else {
        if (isArray) {
            for (int i = 0; i < arraySize; ++i) {
                cout << "디버그: 배열 " << varName << "[" << i << "] 기본값 0으로 초기화 - 주소: " << (varAddr + i) << ", 타입: " << baseType << endl;
                dstack[varAddr + i] = initValue;  // 배열 각 요소를 메모리에 저장
            }
        }
        else {
            cout << "디버그: 변수 " << varName << " 기본값 0으로 초기화 - 주소: " << varAddr << ", 타입: " << baseType << endl;
            dstack[varAddr] = initValue;  // 단일 변수 초기화 값을 메모리에 저장
        }
    }

    // 중복 검사: 현재 스코프에 동일한 이름의 변수가 있는지 확인
    if (isSymbolDefinedInCurrentScope(varName)) {
        parseErrorPrintB(token);
    }
    else {
        string typeName = SymbolType(false, baseType, isArray, false); // 배열 여부에 따른 타입 설정
        Token identToken = { IDENFR, varName, token.lineNumber };
        addSymbolWithCheck(identToken, typeName);

        // Symbol 테이블 업데이트
        bool updated = false;
        for (auto& sym : symbolTable) {
            if (sym.name == varName && sym.scopelineNumber == CurrentScope) {
                sym.address = varAddr;  // 주소 설정
                sym.isArray = isArray;
                sym.arraySize = arraySize;
                sym.value = initValue;  // 초기화 값 설정
                sym.isConst = false;
                updated = true;

                cout << "[DEBUG] 심볼 테이블 업데이트: 이름=" << sym.name << ", 값=" << sym.value << ", 주소=" << sym.address << endl;
                break;
            }
        }

        if (!updated) {
            cout << "[DEBUG] Error: Variable " << varName << " not found in symbol table for current scope." << endl;
        }
    }
    outputFile << "<VarDef>" << endl;
}

// InitVal → Exp | '{' [ Exp { ',' Exp } ] '}' | StringConst
int parseInitVal(int baseAddr) {
    Token token = getCurrentToken();
    int value = 0; // 초기화 값을 저장할 변수

    // StringConst  문자열 초기화 해야함
    if (getCurrentToken().type == STRCON) {
        outputToken(getCurrentToken());
        advanceToken();
    }

    // '{' [ Exp { ',' Exp } ] '}'
    else if (getCurrentToken().type == LBRACE) {
        outputToken(getCurrentToken());
        advanceToken();

        int initIndex = 0; // 초기화 인덱스

        // [ Exp { ',' Exp } ]
        if (getCurrentToken().type != RBRACE) {

            // 첫 번째 Exp
            Value valObj = ExpValue();
            value = valObj.getValue();
            if (baseAddr != -1) {
                emitPcode("STO", to_string(baseAddr + initIndex), getCurrentToken().lineNumber);
                initIndex++;
            }

            // { ',' Exp }
            while (getCurrentToken().type == COMMA) {
                outputToken(getCurrentToken());
                advanceToken();

                valObj = ExpValue();
                value = valObj.getValue();
                if (baseAddr != -1) {
                    emitPcode("STO", to_string(baseAddr + initIndex), getCurrentToken().lineNumber);
                    initIndex++;
                }
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
        Value valObj = ExpValue();
        value = valObj.getValue();
        emitPcode("STO", to_string(baseAddr), getCurrentToken().lineNumber);
    }

    outputFile << "<InitVal>" << endl;
    cout << "[DEBUG] parseInitVal 종료: 최종 값= " << value << endl;

    return value;
}

// MainFuncDef → 'int' 'main' '(' ')' Block // k
void parseMainFuncDef() {
    Token token = getCurrentToken();

    // 'int' 확인
    if (token.type == INTTK) {
        outputToken(token);
        advanceToken();
        currentReturnType = "int";
    }

    // 'main' 확인
    token = getCurrentToken();
    if (token.type == MAINTK) {
        outputToken(token);
        advanceToken();
    }
    else {
        // 'main'이 아닌 경우 오류 처리
        parseErrorPrintfK();
    }

    // '(' 처리
    token = getCurrentToken();
    if (token.type == LPARENT) {
        outputToken(token);
        advanceToken();
    }
    else {
        parseErrorPrintfJ();
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

    cout << "\n~~~~~~~~~~~~~~~~~~~~~~~~ 메인 함수 시작 ~~~~~~~~~~~~~~~~~~~~~~~~" << endl;

    // 'FUNC main' P-Code 생성
    emitPcode("FUNC", "main", token.lineNumber);
    functionMap["main"] = pcodeList.size() - 1; // 'FUNC' 명령어의 인덱스

    // 'INT 0' P-Code 생성 (main 함수의 매개변수 개수는 0)
    emitPcode("INT", "0", token.lineNumber);

    // 반환 타입 스택 관리
    returnTypeStack.push(currentReturnType);
    currentReturnType = "int"; // main 함수의 반환 타입 설정

    // 함수 본문 파싱
    bool hasReturn = parseBlock();
    if (!hasReturn) {
        Token lastToken = getPreviousToken();  // 함수 정의의 마지막 토큰
        parseErrorPrintG(lastToken);
    }

    // 'END_FUNC' P-Code 생성 (RET는 parseBlock()에서 이미 처리)
    emitPcode("END_FUNC", "main", token.lineNumber); // 함수 종료

    // 반환 타입 복원
    if (!returnTypeStack.empty()) {
        currentReturnType = returnTypeStack.top();
        returnTypeStack.pop();
    }
    else {
        currentReturnType = ""; // 기본값 또는 전역 반환 타입 설정
    }

    exitScope();

    cout << "~~~~~~~~~~~~~~~~~~~~~~~~ 메인 함수 종료 ~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
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

void interpretPcode(const string& inputFile, const string& outputFileName) {

    nextAddressStack.push(0);

    ofstream outputFile(outputFileName);

    int pc = 0;     // 프로그램 카운터
    int returnValue = 0;
    int BAddr = 0;

    int variableSpace = nextaddress;
    sp = variableSpace - 1;

    // 변수 주소 매핑 (심볼 테이블과 일치하도록 수정)
    for (const Symbol& sym : symbolTable) {
        if (!sym.isFunction && sym.address != -1 && !sym.isConst) {
            string scopedName = to_string(sym.scopelineNumber) + "_" + sym.name;

            if (variables.find(scopedName) == variables.end()) {
                variables[scopedName] = sym.address;
            }
            else {
                cerr << "Warning!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! " << sym.name << " in scope " << sym.scopelineNumber << endl;
            }
        }
    }

    // 프로그램 시작 시 main 함수 위치로 설정
    if (functionMap.find("main") != functionMap.end()) {
        pc = functionMap["main"];
        cout << "\n메인 함수 실행을 시작합니다. PC: " << pc << endl;
    }

    // 인터프리터 루프
    while (pc < pcodeList.size()) {
        Pcode currentPcode = pcodeList[pc];
        string opcode = currentPcode.getopcode();
        string operand = currentPcode.getoperand();

        //cout << "stackpoint " << pc << endl;
        //cout << "code.getName() = " << currentPcode.getName() << endl;
        //cout << "code.getScope() = " << currentPcode.getScope() << endl;
        //cout << "code.getAddr() = " << currentPcode.getaddr() << endl;
        //cout << "code.getPrint() = " << currentPcode.getPrint() << endl;

        if (opcode == "INT" || opcode == "INT_L") {
            // INT_L은 함수 호출 전에 지역 변수 공간을 예약
            int n = stoi(operand);
            sp += n;
            if (sp >= STACK_SIZE) {
                throw overflow_error("Stack overflow on " + opcode);
            }
            cout << "[" << opcode << "] 스택 포인터 증가, SP=" << sp << endl;
            pc++;
        }

        else if (opcode == "DOWN") {
            // 스택 포인터를 n만큼 감소시켜 공간을 해제
            int n = stoi(operand);
            sp -= n;
            if (sp < -1) {
                cerr << "Error: Stack underflow at PC " << pc << endl;
                sp = -1;
            }
            pc++;
        }

        else if (opcode == "LOD") {
            try {
                int addr = stoi(operand);

                cout << "[DEBUG] LOD 명령어 실행 - 주소: " << addr << ", 값: " << dstack[addr] << endl;

                sp++;
                if (currentPcode.getScope() == 0) {
                    addr = BAddr + addr;
                }
                else {
                    addr = addr;
                }

                dstack[sp] = dstack[addr];
                cout << "[DEBUG] 스택에 값 로드 완료: SP=" << sp << ", 값=" << dstack[sp] << endl;

                pc++;
            }

            catch (const out_of_range& e) {
                cerr << "Error: " << e.what() << " - 주소: " << operand << " at PC " << pc << endl;
                break;
            }
        }

        else if (opcode == "LDA") {
            try {
                int addr = stoi(operand);

                cout << "[DEBUG] LOD 명령어 실행 - 주소: " << addr << ", 값: " << dstack[addr] << endl;

                sp++;
                if (currentPcode.getScope() == 0) {
                    addr = BAddr + addr;
                }
                else {
                    addr = addr;
                }

                dstack[sp] = addr;
                cout << "value == " << dstack[dstack[sp]] << endl;

                pc++;
            }

            catch (const out_of_range& e) {
                cerr << "Error: " << e.what() << " - 주소: " << operand << " at PC " << pc << endl;
                break;
            }
        }


        else if (opcode == "ADD") {
            // 스택에 두 개 이상의 값이 있어야 ADD를 수행할 수 있음
            if (sp < 1) {
                cerr << "[DEBUG ERROR] Not enough operands on stack for ADD at PC "
                    << pc << ", Current Address: " << currentPcode.getlineNumber() << endl;
                break;
            }
            int right = dstack[sp--];
            int left = dstack[sp--];
            dstack[++sp] = left + right;

            // 디버깅 출력
            cout << "[DEBUG] ADD operation performed: "
                << left << " + " << right << " = " << dstack[sp]
                << ". SP=" << sp << ", PC=" << pc
                << ", Current Address: " << currentPcode.getlineNumber() << endl;

            pc++;
        }

        else if (opcode == "SUB") {
            if (sp < 1) {
                std::cerr << "Error: Not enough operands on stack for SUB at PC " << pc << std::endl;
                break;
            }
            int right = dstack[sp--];
            int left = dstack[sp--];
            dstack[++sp] = left - right;
            cout << "SUB: " << left << " - " << right << " = " << dstack[sp] << ". SP=" << sp << endl;
            pc++;
        }

        else if (opcode == "MUL") {
            if (sp < 1) {
                cerr << "Error: Not enough operands on stack for MUL at PC " << pc << endl;
                break;
            }
            int right = dstack[sp--];  // 스택에서 오른쪽 피연산자를 추출
            int left = dstack[sp--];   // 스택에서 왼쪽 피연산자를 추출
            dstack[++sp] = left * right;  // 연산 결과를 다시 스택에 저장
            cout << "MUL: " << left << " * " << right << " = " << dstack[sp] << ". SP=" << sp << endl;
            pc++;
        }

        else if (opcode == "DIV") {
            if (sp < 1) {
                cerr << "Error: Not enough operands on stack for DIV at PC " << pc << endl;
                break;
            }
            int right = dstack[sp--]; // 오른쪽 피연산자
            int left = dstack[sp--];  // 왼쪽 피연산자

            if (right == 0) {
                cerr << "Error: Division by zero at PC " << pc << endl;
                dstack[++sp] = 0; // 안전하게 0으로 처리
            }
            else {
                dstack[++sp] = left / right;
                cout << "DIV: " << left << " / " << right << " = " << dstack[sp] << ". SP=" << sp << endl;
            }
            pc++;
        }

        else if (opcode == "LIT") {
            if (sp + 1 >= STACK_SIZE) {
                cerr << "Stack overflow at PC " << pc << endl;
                break;
            }
            dstack[++sp] = stoi(operand);
            cout << "[DEBUG] LIT 명령어 실행 - 값 " << operand << "을(를) 스택에 푸시합니다. SP=" << sp << endl;
            pc++;
        }

        else if (opcode == "READ_INT") {
            int value;
            cout << "정수를 입력하세요: ";
            cin >> value;
            sp++;
            if (sp >= STACK_SIZE) {
                cerr << "Error: Stack overflow at PC " << pc << endl;
                break;
            }
            dstack[sp] = value;
            cout << "READ_INT: 입력 값 " << value << "을(를) 스택에 푸시합니다. SP=" << sp << endl;
            pc++;
        }

        else if (opcode == "READ_CHAR") {
            char ch;
            cout << "문자를 입력하세요: ";
            cin >> ch;
            sp++;
            if (sp >= STACK_SIZE) {
                cerr << "Error: Stack overflow at PC " << pc << endl;
                break;
            }
            dstack[sp] = static_cast<int>(ch);
            cout << "READ_CHAR: 입력 문자 '" << ch << "'의 아스키 코드 " << dstack[sp] << "을(를) 스택에 푸시합니다. SP=" << sp << endl;
            pc++;
        }

        else if (opcode == "JMP") {
            // 무조건 점프
            int target = stoi(operand);
            if (target >= 0 && target < pcodeList.size()) {
                cout << "[DEBUG] JMP 실행. 대상 주소: " << target << " (현재 PC: " << pc << ")" << endl;
                pc = target; // 대상 주소로 점프
            }
            else {
                cerr << "Error: Invalid jump target " << target << " at PC " << pc << endl;
                break;
            }
        }

        else if (opcode == "JPC") {
            if (sp >= 0) {
                int condition = dstack[sp--];
                cout << "[DEBUG] 조건 평가: " << condition << " (JPC 명령어 실행)" << endl;

                if (condition == 0) {
                    pc = stoi(operand);
                    cout << "[DEBUG] 조건이 거짓이므로 PC를 " << pc << "으로 점프합니다." << endl;
                }
                else {
                    cout << "[DEBUG] 조건이 참이므로 점프하지 않습니다." << endl;
                    pc++;
                }
            }
            else {
                cerr << "[ERROR] 조건부 점프를 수행하려 했으나 스택에 값이 없습니다! (PC=" << pc << ")" << endl;
            }
        }

        else if (opcode == "LEA") {
            // 변수의 주소를 스택에 로드
            int addr = stoi(operand);
            sp++;
            if (sp >= STACK_SIZE) {
                cerr << "Error: Stack overflow at PC " << pc << endl;
                break;
            }
            dstack[sp] = addr;
            pc++;
        }

        else if (opcode == "STO") {
            int addr = stoi(operand);
            if (addr < 0 || addr >= variableSpace) {
                cerr << "Error: Invalid address " << addr << " at PC " << pc << endl;
                break;
            }
            if (sp >= 0) {
                cout << "[DEBUG] STO 명령어 실행 - 값 " << dstack[sp] << "을(를) 주소 " << addr << "에 저장합니다." << endl;
                dstack[addr] = dstack[sp];
                sp--; // 스택에서 값 제거
            }
            else {
                cerr << "Error: Stack underflow on STO at PC " << pc << endl;
            }
            pc++;
        }

        else if (opcode == "WRT") {
            // operand에 저장된 expressionCount를 가져옴
            int expressionCount = stoi(operand);
            int startsp = sp - expressionCount + 1;

            // 스택에서 값 확인
            if (sp >= 0) {
                // 최상단 값 출력
                cout << "\n[OUTPUT] " << dstack[startsp] << endl;
                interpretResult.push_back(to_string(dstack[startsp])); // 결과 저장

                sp++;
            }
            else {
                cerr << "Error: Not enough values on the stack for WRT at PC " << pc << endl;
                break;
            }

            pc++;
        }

        else if (opcode == "WRT_C") {
            // operand에 저장된 expressionCount를 가져옴
            int expressionCount = stoi(operand);

            int startsp = sp - expressionCount + 1; // startsp 계산

            if (sp >= 0) {
                // 스택에서 최상단 값 가져오기
                int value = dstack[startsp];
                if (value >= 0 && value <= 255) {
                    char charValue = static_cast<char>(value); // 아스키 값을 문자로 변환

                    // 출력
                    cout << "[OUTPUT] " << charValue << endl;
                    interpretResult.push_back(string(1, charValue)); // 결과 저장
                }
                else {
                    cerr << "[ERROR] Invalid ASCII value on stack: " << value << endl;
                }

                sp++; // 스택 포인터 감소
            }
            else {
                cerr << "[ERROR] Stack underflow on WRT_C at PC " << pc << endl;
                break;
            }

            pc++;
        }

        else if (opcode == "WRTS") {
            // 문자열 출력
            string s = operand;
            // 따옴표(") 제거 및 개행 문자("\n") 처리
            s.erase(remove(s.begin(), s.end(), '\"'), s.end());
            size_t pos = 0;
            while ((pos = s.find("\\n", pos)) != string::npos) {
                s.replace(pos, 2, "\n");
                pos += 1;
            }
            cout << s; // 출력
            interpretResult.push_back(s); // 결과 저장
            pc++;
        }

        else if (opcode == "LODS") {
            // LODS: Load indirect
            if (sp >= 0) {
                dstack[sp] = dstack[dstack[sp]];
                sp--;
            }
            else {
                cerr << "Error: Stack underflow on LODS at PC " << pc << endl;
            }
            pc++;
        }

        else if (opcode == "LDC") {
            // LDC: Load constant
            int constant = stoi(operand);
            sp++;
            if (sp >= STACK_SIZE) {
                cerr << "Error: Stack overflow at PC " << pc << endl;
                break;
            }
            dstack[sp] = constant;
            pc++;
        }

        else if (opcode == "MOD") {
            // MOD: Modulo
            if (sp >= 1) {
                dstack[sp - 1] = dstack[sp - 1] % dstack[sp];
                sp--;
            }
            else {
                cerr << "Error: Stack underflow on MOD at PC " << pc << endl;
            }
            pc++;
        }

        else if (opcode == "JTM") {
            // JTM: Jump to method/function
            // BAddr = sp + 1;
            // at = label_point;
            BAddr = sp + 1;
            pc = stoi(operand); // operand는 label_point로 가정
        }

        else if (opcode == "RET") {
            if (!callStack.empty()) {
                // 반환 주소와 이전 sp를 함께 꺼냄
                pair<int, int> returnInfo = callStack.top();
                callStack.pop();

                int returnAddress = returnInfo.first;
                int previousSP = returnInfo.second;

                cout << "[DEBUG] RET 명령어 실행: returnAddress = "
                    << returnAddress << ", previous SP = " << previousSP << endl;

                // 반환값을 스택에서 추출
                if (sp >= 0) {
                    int retVal = dstack[sp--]; // 반환값 추출
                    cout << "[DEBUG] 반환값: " << retVal << " (RET 명령어 실행)" << endl;

                    // 스택 포인터를 이전 상태로 복원
                    sp = previousSP;

                    // 반환값을 호출자 스택에 푸시
                    dstack[++sp] = retVal;
                    cout << "[DEBUG] 반환값 " << retVal << "을(를) 스택에 푸시했습니다. SP = " << sp << endl;
                }
                else {
                    cerr << "[ERROR] 반환값이 스택에서 누락되었습니다! (PC=" << pc << ")" << endl;
                    // 스택 포인터를 이전 상태로 복원
                    sp = previousSP;
                    // 기본값으로 0 저장
                    dstack[++sp] = 0;
                    cout << "[DEBUG] 기본값 0을 스택에 푸시했습니다. SP = " << sp << endl;
                }

                // 현재 스코프 종료
                exitScope();

                // 호출자 주소로 복귀
                pc = returnAddress;
                cout << "[DEBUG] 호출자로 복귀. PC = " << pc << ", SP = " << sp << endl;
            }
            else {
                cout << "[DEBUG] 메인 함수에서 RET 호출. 프로그램 종료." << endl;
                break; // 프로그램 종료
            }
        }

        else if (opcode == "RET_VOID") {
            if (!callStack.empty()) {
                pair<int, int> returnInfo = callStack.top();
                callStack.pop();

                int returnAddress = returnInfo.first;
                int previousSP = returnInfo.second;
                // 현재 스코프 종료
                exitScope();

                pc = returnAddress; // 호출자로 복귀
                cout << "[DEBUG] RET_VOID: 호출자로 복귀. PC = " << pc << endl;
            }
            else {
                // 메인 함수에서 RET_VOID 호출 -> 프로그램 종료
                cout << "[DEBUG] 메인 함수에서 RET_VOID 호출. 프로그램 종료." << endl;
                break;
            }
        }

        else if (opcode == "CAL") {
            string funcName = operand;

            // 함수가 정의되어 있는지 확인
            if (functionMap.find(funcName) != functionMap.end()) {
                // 현재 PC를 호출 스택에 저장하고 함수 시작 주소로 점프
                pair<int, int> returnInfo = make_pair(pc + 1, sp);
                callStack.push(returnInfo);
                pc = functionMap[funcName];

                // 매개변수 개수 계산 (functionParams에서 벡터의 크기)
                size_t paramCount = functionParams[funcName].size();

                // 매개변수의 개수가 스택에 존재하는지 확인 (범위 초과 방지)
                if (sp < static_cast<int>(paramCount) - 1) {
                    cerr << "[ERROR] Stack underflow: 매개변수 개수가 부족합니다. 함수 '" << funcName << "' 호출 실패!" << endl;
                    break;
                }

                if (paramCount > 0) {
                    // 매개변수 정보 확인
                    if (functionParams.find(funcName) == functionParams.end()) {
                        cerr << "[ERROR] 함수 '" << funcName << "'의 매개변수 정보를 찾을 수 없습니다." << endl;
                        break;
                    }

                    const vector<pair<string, int>>& params = functionParams[funcName];

                    if (params.size() != paramCount) {
                        cerr << "[ERROR] 함수 '" << funcName << "'의 매개변수 개수와 저장된 매개변수 개수가 일치하지 않습니다." << endl;
                        break;
                    }

                    // 매개변수를 올바른 주소에 할당
                    for (size_t i = 0; i < paramCount; ++i) {
                        int argValue = dstack[sp - paramCount + i + 1]; // 스택에서 값 가져오기
                        int paramAddr = params[i].second; // 매개변수 주소

                        cout << "[DEBUG] 매개변수 전달 - 매개변수 인덱스: " << i << ", 스택 위치: " << (sp - paramCount + i + 1) << ", 값: " << argValue << endl;
                        cout << "[DEBUG] 매개변수에 값 " << argValue << "을(를) 주소 " << paramAddr << "에 저장합니다." << endl;

                        dstack[paramAddr] = argValue; // 매개변수 주소에 값 할당
                    }

                    sp -= static_cast<int>(paramCount); // 매개변수 제거
                }

                // 함수 호출 시 새로운 스코프 진입
                enterScope();
            }
            else {
                cerr << "[ERROR] 정의되지 않은 함수 '" << funcName << "' 호출!" << endl;
                break;
            }
        }

        else if (opcode == "J") {
            // J: Jump
            pc = stoi(operand); // operand는 label_point로 가정
        }

        else if (opcode == "JP0") {
            // JP0: Jump if zero
            if (sp >= 0) {
                if (dstack[sp] == 0) {
                    pc = stoi(operand); // operand는 label_point로 가정
                }
                else {
                    pc++;
                }
                sp--;
            }
            else {
                cerr << "Error: Stack underflow on JP0 at PC " << pc << endl;
                pc++;
            }
        }

        else if (opcode == "NOT") {
            // NOT: Logical NOT - 스택에 값이 있는지 확인
            if (sp >= 0) {
                dstack[sp] = (dstack[sp] == 0) ? 1 : 0;
            }
            else {
                // 현재 PCode 명령어의 lineNumber 정보를 사용해 오류 발생 위치 출력
                cerr << "Error: Stack underflow on NOT at PC " << pc
                    << " (Line " << currentPcode.getlineNumber() << ")" << endl;
            }
            pc++;
        }

        // **비교 연산자 처리 
        else if (opcode == "LT") { // Less Than
            if (sp < 1) {
                cerr << "Error: Not enough operands on stack for LT at PC " << pc << endl;
                break;
            }
            int right = dstack[sp--];
            int left = dstack[sp--];
            dstack[++sp] = (left < right) ? 1 : 0;
            cout << "LT: " << left << " < " << right << " = " << dstack[sp] << ". SP=" << sp << endl;
            pc++;
        }

        else if (opcode == "GT") { // Greater Than
            if (sp < 1) {
                cerr << "Error: Not enough operands on stack for GT at PC " << pc << endl;
                break;
            }
            int right = dstack[sp--];
            int left = dstack[sp--];
            dstack[++sp] = (left > right) ? 1 : 0;
            cout << "GT: " << left << " > " << right << " = " << dstack[sp] << ". SP=" << sp << endl;
            pc++;
        }

        else if (opcode == "LE") { // Less or Equal
            if (sp < 1) {
                cerr << "Error: Not enough operands on stack for LE at PC " << pc << endl;
                break;
            }
            int right = dstack[sp--];
            int left = dstack[sp--];
            dstack[++sp] = (left <= right) ? 1 : 0;
            cout << "LE: " << left << " <= " << right << " = " << dstack[sp] << ". SP=" << sp << endl;
            pc++;
        }

        else if (opcode == "GE") { // Greater or Equal
            if (sp < 1) {
                cerr << "Error: Not enough operands on stack for GE at PC " << pc << endl;
                break;
            }
            int right = dstack[sp--];
            int left = dstack[sp--];
            dstack[++sp] = (left >= right) ? 1 : 0;
            cout << "GE: " << left << " >= " << right << " = " << dstack[sp] << ". SP=" << sp << endl;
            pc++;
        }

        else if (opcode == "EQ") { // Equal
            if (sp < 1) {
                cerr << "Error: Not enough operands on stack for EQ at PC " << pc << endl;
                break;
            }
            int right = dstack[sp--];
            int left = dstack[sp--];
            dstack[++sp] = (left == right) ? 1 : 0;
            cout << "EQ: " << left << " == " << right << " = " << dstack[sp] << ". SP=" << sp << endl;
            pc++;
        }

        else if (opcode == "NE") { // Not Equal
            if (sp < 1) {
                cerr << "Error: Not enough operands on stack for NE at PC " << pc << endl;
                break;
            }
            int right = dstack[sp--];
            int left = dstack[sp--];
            dstack[++sp] = (left != right) ? 1 : 0;
            cout << "NE: " << left << " != " << right << " = " << dstack[sp] << ". SP=" << sp << endl;
            pc++;
        }

        else if (opcode == "FUNC") {
            // 함수 정의는 호출 시점에 처리되므로 건너뜀
            pc++;
            continue;
        }

        else if (opcode == "END_FUNC") {
            // END_FUNC 명령어는 함수 정의 종료를 나타내므로 무시
            pc++;
            continue;
        }

        else {
            cerr << "Fucking Error: Unsupported PCODE '" << opcode << "' at PC " << pc << endl;
            pc++;
        }

        // 스택 오버플로우 및 언더플로우 방지
        if (sp >= STACK_SIZE) {
            cerr << "Error: Stack overflow after PC " << pc << endl;
            break;
        }

        if (sp < -1) {
            cerr << "Error: Stack underflow after PC " << pc << endl;
            sp = -1;
            break;
        }
    }

    // 출력 결과를 파일에 기록
    for (const string& res : interpretResult) {
        outputFile << res;
    }
    outputFile.close();
}

int main() {
    nextAddressStack.push(0);

    // 어휘 분석 수행
    lexicalAnalysis("testfile.txt");

    // // 어휘 분석
    // outputFile.open("lexer.txt");
    // for (const Token& token : tokens) {
    //     outputToken(token);
    // }
    // outputFile.close();

    // 구문 분석
    outputFile.open("parser.txt");
    parseCompUnit();
    outputFile.close();

    //parseCompUnit();

    // // symbolTable을 scopelineNumber 기준으로 오름차순 정렬
    // stable_sort(symbolTable.begin(), symbolTable.end(), [](const Symbol& a, const Symbol& b) -> bool {
    //     return a.scopelineNumber < b.scopelineNumber;
    //     });

    // // 정렬된 symbolTable을 symbol.txt에 출력
    // outputFile.open("symbol.txt");
    // for (const Symbol& sym : symbolTable) {
    //     outputFile << sym.scopelineNumber << " " << sym.name << " " << sym.type << endl;
    // }
    // outputFile.close();

    // // 오류 처리
    // if (!errors.empty()) {
    //     // lineNumber 기준으로 정렬
    //     sort(errors.begin(), errors.end(), [](const Error& a, const Error& b) {
    //         return a.lineNumber < b.lineNumber; // lineNumber가 작은 순서대로 정렬
    //         });

    //     ofstream errorFile("error.txt");
    //     for (vector<Error>::const_iterator it = errors.begin(); it != errors.end(); ++it) {
    //         errorFile << it->lineNumber << " " << it->errorcode << endl;
    //     }
    //     errorFile.close();
    // }

    // P-code 해석 및 실행 결과를 pcoderesult.txt에 출력
    interpretPcode("", "pcoderesult.txt");

    return 0;
}