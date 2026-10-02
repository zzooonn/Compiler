// .h名的file为header file

#ifndef LEXER_H     // if not define, 如已有Lexer.h同名的file为忽视。
#define LEXER_H

#include <string>
#include <vector>
#include <unordered_map>
using namespace std;

// 所有token type
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
    // 逻辑或运算符
    OR,
    // 乘法运算符
    MULT,
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
    // 单个&
    EAND,
    // 单个|
    EOR
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

    void LexicalAnalysis();
    void LexicalAnalysis2();

    const vector<TokenWord>& GetTokenList() const;

    void WriteToFile(string_view filePath);
private:
    string mInputFilePath;
    string mInputText;
    int mCurpos;
    int mCurrentLinenum;

    vector<TokenWord> mTokens;

    void PushTokenList(const string& token, const string& word);
    void PushTokenList(EnumTokenType tokenType, const string& word, int lineNum);

    static string TokenTypeToString(EnumTokenType token);
    static EnumTokenType StringToTokenType(string_view token);
private:
    static unordered_map<string, EnumTokenType> mKeywords;
    static unordered_map<string, EnumTokenType> mOperators;
};

#endif