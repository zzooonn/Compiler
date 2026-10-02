#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <stack>
#include <cctype> 
#include <algorithm>

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

void lexicalAnalysis(const string& filename);
void outputToken(const Token& token);
void advanceToken();
void parseExp();
void parseNumber();
void parseCharacter();
void parseLVal();
void parsePrimaryExp();
void parseFuncRParams();
void parseCond();
void parseUnaryOp();
void parseUnaryExp();
void parseConstExp();
void parseMulExp();
void parseAddExp();
void parseRelExp();
void parseEqExp();
void parseLAndExp();
void parseLOrExp();
void parseForStmt();
void parseStmt();
void parseFuncType();
void parseBType();
void parseConstDecl();
void parseConstDef();
void parseFuncFParam();
void parseFuncDef();
void parseFuncFParams();
void parseMainFuncDef();
void parseConstInitVal();
void parseVarDecl();
void parseVarDef();
void parseInitVal();
void parseBlockItem();
void parseBlock();
void parseDecl();
void parseCompUnit();

vector<Token> tokens;
vector<Error> errors;
size_t tokenIndex = 0;
int lineNumber = 1;
ofstream outputFile;
bool inConditionalContext = false;

/*
期中模拟考试部分
string toLowerCase(const string& str) {   如果是upper，将lower换成upper即可。toupper就行。
    string lowerStr = str;
    transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::tolower);
    return lowerStr;
}
if (keywords.find(toLowerCase(ident)) != keywords.end()) {
    tokens.push_back({ keywords[toLowerCase(ident)], ident, lineNumber });
}
else {
    tokens.push_back({ IDENFR, ident, lineNumber });
}
*/

Token getCurrentToken()
{
    if (tokenIndex < tokens.size()) return tokens[tokenIndex];
    else exit(1);
}

Token getPreviousToken()
{
    if (tokenIndex > 0 && tokenIndex - 1 < tokens.size())
        return tokens[tokenIndex - 1];
    else
        return Token{ IDENFR, "", 0 };
}

void parseErrorPrintfI() {
    Token currentToken = getCurrentToken();
    Token previousToken = getPreviousToken();

    if (currentToken.type == SEMICN) {
        outputToken(currentToken);
        advanceToken();
    }
    else {
        errors.push_back({ previousToken.lineNumber, 'i' });
    }
}

void parseErrorPrintfJ() {
    Token currentToken = getCurrentToken();
    Token previousToken = getPreviousToken();

    if (currentToken.type == RPARENT) {
        outputToken(currentToken);
        advanceToken();
    }
    else {
        errors.push_back({ previousToken.lineNumber, 'j' });
    }
}

void parseErrorPrintfK() {
    Token token = getCurrentToken();

    if (token.type == RBRACK) {
        outputToken(token);
        advanceToken();
    }
    else {
        errors.push_back({ token.lineNumber, 'k' });
    }
}

// 将token type 换成 string type
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
    default: return "UNKNOWN";
    }
}

void lexicalAnalysis(const string& filename) {

    ifstream inputFile(filename);       // read file 
    char currentChar;

    while (inputFile.get(currentChar)) {     // read currentChar one word

        // isspace 
        if (isspace(currentChar)) {
            if (currentChar == '\n') {
                lineNumber++;
            }
            continue;
        }

        // 注释处理 : 分两个部分 + div的可能
        else if (currentChar == '/') {

            // /* ， */ 的可能
            char nextChar = inputFile.peek();
            if (nextChar == '*') {

                inputFile.get();
                bool endofComment = false;

                while (inputFile.get(currentChar)) {
                    if (currentChar == '\n') {
                        lineNumber++;
                    }

                    else if (currentChar == '*') {
                        if (inputFile.peek() == '/') {
                            inputFile.get();
                            endofComment = true;        // 说明注释结束
                            break;
                        }
                    }

                    if (inputFile.eof()) break;         // 当EOF时，就结束
                }
                continue;
            }

            // 单行的注释
            else if (nextChar == '/') {
                inputFile.get();

                while (inputFile.get(currentChar)) {
                    if (currentChar == '\n') {
                        lineNumber++;
                        break;
                    }

                    if (inputFile.eof()) break;
                }
                continue;
            }

            // div的可能
            else {
                tokens.push_back({ DIV, "/", lineNumber });
            }
        }

        // keywords 处理，或者ident处理
        else if (isalpha(currentChar) || currentChar == '_')
        {
            string ident;
            ident += currentChar;

            // isalnum ( a-z, A-Z , 0~9 ) 如果不是这个范围，说明不是ident。
            while (inputFile.peek() != EOF && (isalnum(inputFile.peek()) || inputFile.peek() == '_')) {
                ident += inputFile.get();
            }

            // 在 keyword 内找
            if (keywords.find(ident) != keywords.end()) {
                tokens.push_back({ keywords[ident], ident, lineNumber });
            }
            // 如果不是 keyword，说明ident名
            else {
                tokens.push_back({ IDENFR, ident, lineNumber });
            }
        }

        // 数字的可能
        else if (isdigit(currentChar)) {
            string number;
            number += currentChar;

            while (isdigit(inputFile.peek())) {
                number += inputFile.get();
            }
            // 数字分为INTCON
            tokens.push_back({ INTCON, number, lineNumber });
        }

        // string类 
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

                if (inputFile.eof()) break;
            }

            if (closed) {
                tokens.push_back({ STRCON, strConst, lineNumber });
            }
            else {
                errors.push_back({ lineNumber, 'a' });
            }
        }

        // 单个str
        else if (currentChar == '\'') {

            string charConst;
            charConst += currentChar;
            char c = inputFile.get();
            charConst += c;

            if (c == '\\')
            {
                char nextChar = inputFile.get();
                charConst += nextChar;
                c = nextChar;
            }

            if (inputFile.peek() == '\'') {
                charConst += inputFile.get();
                tokens.push_back({ CHRCON, charConst, lineNumber });
            }
            else {
                errors.push_back({ lineNumber, 'a' });
            }
        }
        // 计算和处理 &， | 部分
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

        if (inputFile.eof()) break;
    }

    inputFile.close();
}

void outputToken(const Token& token) {
    outputFile << tokenTypeToString(token.type) << " " << token.value << endl;
}

void advanceToken() {
    if (tokenIndex < tokens.size())
    {
        tokenIndex++;
    }
}

void parseNumber() {
    Token token = getCurrentToken();

    if (token.type == INTCON) {
        if (token.value.length() > 1 && token.value[0] == '0') {
            return;
        }
        else {
            outputToken(token);
            advanceToken();
        }
    }
}

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
            advanceToken();
        }
    }
    outputFile << "<Character>" << endl;
}

void parseLVal() {

    Token token = getCurrentToken();

    if (token.type == IDENFR) {
        outputToken(token);
        advanceToken();
        token = getCurrentToken();
    }

    if (token.type == LBRACK) {
        outputToken(token);
        advanceToken();
        token = getCurrentToken();

        parseExp();
        token = getCurrentToken();

        if (token.type == RBRACK) {
            outputToken(token);
            advanceToken();
            token = getCurrentToken();
        }

        else parseErrorPrintfK();
    }

    outputFile << "<LVal>" << endl;
}

void parseFuncRParams() {
    parseExp();

    while (getCurrentToken().type == COMMA) {
        outputToken(getCurrentToken());
        advanceToken();
        parseExp();
    }

    outputFile << "<FuncRParams>" << endl;
}

void parseLOrExp() {

    parseLAndExp();
    outputFile << "<LOrExp>" << endl;

    if (getCurrentToken().type == OR) {
        outputToken(getCurrentToken());
        advanceToken();
        parseLOrExp();
    }
}

void parseLAndExp() {

    parseEqExp();
    outputFile << "<LAndExp>" << endl;

    if (getCurrentToken().type == AND) {
        outputToken(getCurrentToken());
        advanceToken();
        parseLAndExp();
    }
}

void parseUnaryOp() {
    Token token = getCurrentToken();

    if (token.type == PLUS || token.type == MINU || token.type == NOT)
    {
        outputToken(token);
        advanceToken();
    }
    outputFile << "<UnaryOp>" << endl;
}

void parseUnaryExp() {
    Token token = getCurrentToken();

    if (token.type == PLUS || token.type == MINU || token.type == NOT) {
        parseUnaryOp();
        parseUnaryExp();
    }

    else if (getCurrentToken().type == IDENFR) {

        if (tokenIndex + 1 < tokens.size() && tokens[tokenIndex + 1].type == LPARENT) {
            outputToken(getCurrentToken());
            advanceToken();
            outputToken(getCurrentToken());
            advanceToken();

            if (getCurrentToken().type != RPARENT) {
                parseFuncRParams();
            }

            if (getCurrentToken().type == RPARENT) {
                outputToken(getCurrentToken());
                advanceToken();
            }
            else {
                parseErrorPrintfJ();
            }
        }

        else {
            parsePrimaryExp();
        }
    }

    else {
        parsePrimaryExp();
    }

    outputFile << "<UnaryExp>" << endl;
}

void parseExp() {
    parseAddExp();

    outputFile << "<Exp>" << endl;
}

void parseAddExp() {

    parseMulExp();
    outputFile << "<AddExp>" << endl;

    if (getCurrentToken().type == PLUS || getCurrentToken().type == MINU) {
        outputToken(getCurrentToken());
        advanceToken();
        parseAddExp();
    }
}

void parsePrimaryExp() {

    if (getCurrentToken().type == LPARENT) {
        outputToken(getCurrentToken());
        advanceToken();

        parseExp();

        if (getCurrentToken().type == RPARENT) {
            outputToken(getCurrentToken());
            advanceToken();
        }
        else {
            parseErrorPrintfJ();
        }
    }

    else if (getCurrentToken().type == IDENFR) {
        parseLVal();
    }

    else if (getCurrentToken().type == INTCON) {
        outputToken(getCurrentToken());
        advanceToken();
        outputFile << "<Number>" << endl;
    }

    else if (getCurrentToken().type == CHRCON) {
        outputToken(getCurrentToken());
        advanceToken();
        outputFile << "<Character>" << endl;
    }

    outputFile << "<PrimaryExp>" << endl;
}

void parseMulExp() {

    parseUnaryExp();
    outputFile << "<MulExp>" << endl;

    if (getCurrentToken().type == MULT || getCurrentToken().type == DIV || getCurrentToken().type == MOD) {
        outputToken(getCurrentToken());
        advanceToken();

        parseMulExp();
    }
}

void parseRelExp() {

    parseAddExp();
    outputFile << "<RelExp>" << endl;

    if (getCurrentToken().type == LSS || getCurrentToken().type == LEQ || getCurrentToken().type == GRE || getCurrentToken().type == GEQ) {
        outputToken(getCurrentToken());
        advanceToken();
        parseRelExp();
    }
}

void parseEqExp() {

    parseRelExp();
    outputFile << "<EqExp>" << endl;

    if (getCurrentToken().type == EQL || getCurrentToken().type == NEQ) {
        outputToken(getCurrentToken());
        advanceToken();
        parseEqExp();
    }
}

void parseForStmt() {

    parseLVal();

    if (getCurrentToken().type == ASSIGN)
    {
        outputToken(getCurrentToken());
        advanceToken();
    }
    else {
        advanceToken();
    }

    parseExp();

    outputFile << "<ForStmt>" << endl;
}

void parseFuncType() {
    Token token = getCurrentToken();

    if (token.type == VOIDTK || token.type == INTTK || token.type == CHARTK)
    {
        outputToken(token);
        advanceToken();
        token = getCurrentToken();
    }

    outputFile << "<FuncType>" << endl;
}

void parseBType() {

    Token token = getCurrentToken();

    if (token.type == INTTK || token.type == CHARTK)
    {
        outputToken(token);
        advanceToken();
        token = getCurrentToken();
    }
}

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

void parseDecl() {

    Token token = getCurrentToken();

    if (token.type == CONSTTK) {
        parseConstDecl();
    }
    else if (token.type == INTTK || token.type == CHARTK) {
        parseVarDecl();
    }
}

void parseBlock() {

    Token token = getCurrentToken();

    if (token.type == LBRACE) {
        outputToken(token);
        advanceToken();
    }

    while (getCurrentToken().type != RBRACE) {
        parseBlockItem();
    }

    token = getCurrentToken();
    if (getCurrentToken().type == RBRACE) {
        outputToken(getCurrentToken());
        advanceToken();
    }

    outputFile << "<Block>" << endl;
}

void parseBlockItem() {
    Token token = getCurrentToken();

    if (token.type == CONSTTK || token.type == INTTK || token.type == CHARTK) {
        parseDecl();
    }

    else {
        parseStmt();
    }
}

void parseFuncFParam() {
    parseBType();

    Token token = getCurrentToken();
    if (token.type == IDENFR) {
        outputToken(token);
        advanceToken();
    }

    if (getCurrentToken().type == LBRACK) {
        outputToken(getCurrentToken());
        advanceToken();

        token = getCurrentToken();
        if (token.type == RBRACK) {
            outputToken(token);
            advanceToken();
        }
        else {
            parseErrorPrintfK();
        }
    }

    outputFile << "<FuncFParam>" << endl;
}

void parseStmt() {
    Token token = getCurrentToken();

    if (token.type == BREAKTK) {
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

    else if (token.type == CONTINUETK) {
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

    else if (token.type == RETURNTK) {
        outputToken(token);
        advanceToken();

        if (getCurrentToken().type != SEMICN) {
            parseExp();
        }

        if (getCurrentToken().type == SEMICN) {
            outputToken(getCurrentToken());
            advanceToken();
        }

        else {
            parseErrorPrintfI();
        }
    }

    else if (token.type == IFTK) {
        outputToken(token);
        advanceToken();

        if (getCurrentToken().type == LPARENT) {
            outputToken(getCurrentToken());
            advanceToken();
        }

        parseCond();

        if (getCurrentToken().type == RPARENT) {
            outputToken(getCurrentToken());
            advanceToken();
        }
        else {
            parseErrorPrintfJ();
        }

        parseStmt();

        if (getCurrentToken().type == ELSETK) {
            outputToken(getCurrentToken());
            advanceToken();
            parseStmt();
        }
    }

    else if (token.type == FORTK) {
        outputToken(token);
        advanceToken();

        if (getCurrentToken().type == LPARENT) {
            outputToken(getCurrentToken());
            advanceToken();
        }

        if (getCurrentToken().type != SEMICN) {
            parseForStmt();
        }

        if (getCurrentToken().type == SEMICN) {
            outputToken(getCurrentToken());
            advanceToken();
        }
        else {
            parseErrorPrintfI();
        }

        if (getCurrentToken().type != SEMICN) {
            parseCond();
        }

        if (getCurrentToken().type == SEMICN) {
            outputToken(getCurrentToken());
            advanceToken();
        }
        else {
            parseErrorPrintfI();
        }

        if (getCurrentToken().type != RPARENT) {
            parseForStmt();
        }

        if (getCurrentToken().type == RPARENT) {
            outputToken(getCurrentToken());
            advanceToken();
        }
        else {
            parseErrorPrintfJ();
        }

        parseStmt();
    }

    else if (token.type == PRINTFTK) {
        outputToken(token);
        advanceToken();

        if (getCurrentToken().type == LPARENT) {
            outputToken(getCurrentToken());
            advanceToken();
        }

        if (getCurrentToken().type == STRCON) {
            outputToken(getCurrentToken());
            advanceToken();
        }

        while (getCurrentToken().type == COMMA) {
            outputToken(getCurrentToken());
            advanceToken();
            parseExp();
        }

        if (getCurrentToken().type == RPARENT) {
            outputToken(getCurrentToken());
            advanceToken();
        }
        else {
            parseErrorPrintfJ();
        }

        if (getCurrentToken().type == SEMICN) {
            outputToken(getCurrentToken());
            advanceToken();
        }
        else {
            parseErrorPrintfI();
        }
    }

    else if (token.type == LBRACE) {
        parseBlock();
    }

    else if (token.type == SEMICN) {
        outputToken(token);
        advanceToken();
    }

    else {
        int index = 0;
        bool isLVal = false;

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

        if (isLVal) {
            parseLVal();

            outputToken(getCurrentToken());
            advanceToken();
            Token token = getCurrentToken();

            if (token.type == GETINTTK || token.type == GETCHARTK) {
                outputToken(token);
                advanceToken();

                if (getCurrentToken().type == LPARENT) {
                    outputToken(getCurrentToken());
                    advanceToken();
                }

                if (getCurrentToken().type == RPARENT) {
                    outputToken(getCurrentToken());
                    advanceToken();
                }
                else {
                    parseErrorPrintfJ();
                }

                if (getCurrentToken().type == SEMICN) {
                    outputToken(getCurrentToken());
                    advanceToken();
                }
                else {
                    errors.push_back({ token.lineNumber, 'i' });
                }
            }

            else {
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

        else {
            if (getCurrentToken().type != SEMICN) {
                parseExp();
            }

            if (getCurrentToken().type == SEMICN) {
                outputToken(getCurrentToken());
                advanceToken();
            }
            else {
                parseErrorPrintfI();
            }
        }
    }
    outputFile << "<Stmt>" << endl;
}

void parseFuncFParams() {
    parseFuncFParam();

    Token token = getCurrentToken();
    while (token.type == COMMA)
    {
        outputToken(token);
        advanceToken();
        parseFuncFParam();
        token = getCurrentToken();
    }
    outputFile << "<FuncFParams>" << endl;
}

void parseFuncDef() {

    parseFuncType();

    Token token = getCurrentToken();
    if (token.type == IDENFR) {
        outputToken(token);
        advanceToken();
    }

    token = getCurrentToken();
    if (token.type == LPARENT) {
        outputToken(token);
        advanceToken();
    }

    token = getCurrentToken();
    if (token.type != RPARENT) {
        parseFuncFParams();
    }

    token = getCurrentToken();
    if (token.type == RPARENT) {
        outputToken(token);
        advanceToken();
    }
    else {
        parseErrorPrintfJ();
    }

    parseBlock();

    outputFile << "<FuncDef>" << endl;
}

void parseCond() {
    parseLOrExp();
    outputFile << "<Cond>" << endl;
}

void parseConstDef() {

    Token token = getCurrentToken();

    if (token.type == IDENFR) {
        outputToken(token);
        advanceToken();
    }

    if (getCurrentToken().type == LBRACK) {
        outputToken(getCurrentToken());
        advanceToken();

        parseConstExp();

        if (getCurrentToken().type == RBRACK) {
            outputToken(getCurrentToken());
            advanceToken();
        }

        else parseErrorPrintfK();
    }

    if (getCurrentToken().type == ASSIGN) {
        outputToken(getCurrentToken());
        advanceToken();
    }

    token = getCurrentToken();
    parseConstInitVal();

    outputFile << "<ConstDef>" << endl;
}

void parseConstExp() {
    parseAddExp();
    outputFile << "<ConstExp>" << endl;
}

void parseConstInitVal() {

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

    else if (getCurrentToken().type == STRCON) {
        outputToken(getCurrentToken());
        advanceToken();
    }

    else {
        parseConstExp();
    }

    outputFile << "<ConstInitVal>" << endl;
}

void parseVarDecl() {

    parseBType();
    parseVarDef();

    Token token = getCurrentToken();

    while (token.type == COMMA) {
        outputToken(token);
        advanceToken();
        token = getCurrentToken();

        parseVarDef();
        token = getCurrentToken();
    }

    token = getCurrentToken();

    if (token.type == SEMICN) {
        outputToken(token);
        advanceToken();
    }
    else {
        parseErrorPrintfI();
    }

    outputFile << "<VarDecl>" << endl;
}

void parseVarDef() {

    Token token = getCurrentToken();

    if (token.type == IDENFR) {
        outputToken(token);
        advanceToken();
    }

    if (getCurrentToken().type == LBRACK) {
        outputToken(getCurrentToken());
        advanceToken();
        parseConstExp();

        if (getCurrentToken().type == RBRACK) {
            outputToken(getCurrentToken());
            advanceToken();
        }
        else {
            parseErrorPrintfK();
        }
    }

    if (getCurrentToken().type == ASSIGN) {
        outputToken(getCurrentToken());
        advanceToken();
        parseInitVal();
    }

    outputFile << "<VarDef>" << endl;
}

void parseInitVal() {

    Token token = getCurrentToken();

    if (getCurrentToken().type == STRCON) {
        outputToken(getCurrentToken());
        advanceToken();
    }

    else if (getCurrentToken().type == LBRACE) {
        outputToken(getCurrentToken());
        advanceToken();

        if (getCurrentToken().type != RBRACE) {

            parseExp();

            while (getCurrentToken().type == COMMA) {
                outputToken(getCurrentToken());
                advanceToken();

                parseExp();
            }
        }

        if (getCurrentToken().type == RBRACE) {
            outputToken(getCurrentToken());
            advanceToken();
        }
    }

    else {
        parseExp();
    }

    outputFile << "<InitVal>" << endl;
}

void parseMainFuncDef() {
    Token token = getCurrentToken();

    if (token.type == INTTK) {
        outputToken(token);
        advanceToken();
    }

    token = getCurrentToken();
    if (token.type == MAINTK) {
        outputToken(token);
        advanceToken();
    }
    else {
        advanceToken();
    }

    token = getCurrentToken();
    if (token.type == LPARENT) {
        outputToken(token);
        advanceToken();
    }

    token = getCurrentToken();
    if (token.type == RPARENT) {
        outputToken(token);
        advanceToken();
    }
    else {
        parseErrorPrintfJ();
    }

    parseBlock();

    outputFile << "<MainFuncDef>" << endl;
}

void parseCompUnit()
{
    Token token = getCurrentToken();

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
                        break;
                    }
                    else
                    {
                        parseVarDecl();
                    }
                }
                else if (nextToken.type == MAINTK)
                {
                    break;
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
                        parseFuncDef();
                        continue;
                    }
                    else
                    {
                        break;
                    }
                }
                else if (nextToken.type == MAINTK)
                {
                    break;
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

    parseMainFuncDef();

    outputFile << "<CompUnit>" << endl;
}

int main()
{
    lexicalAnalysis("testfile.txt");

    outputFile.open("lexer.txt");
    for (const Token& token : tokens)
    {
        outputToken(token);
    }
    outputFile.close();

    outputFile.open("parser.txt");
    parseCompUnit();
    outputFile.close();


    if (!errors.empty())
    {
        sort(errors.begin(), errors.end(), [](const Error& a, const Error& b) {
            return a.lineNumber < b.lineNumber;
            });

        ofstream errorFile("error.txt");
        for (const Error& error : errors)
        {
            errorFile << error.lineNumber << " " << error.errorcode << endl;
        }
        errorFile.close();
    }

    return 0;
}
