#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>
#include <unordered_map>

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
    // 乘法运算符
    EOR,
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
    DMULT
};


struct TokenWord 
{
    EnumTokenType tokenType;
    std::string tokenName;
    std::string token;
    std::string word;
    int lineNum;
};

class Lexer 
{
public:
    Lexer(std::string_view filePath);

    void LexicalAnalysis();
    void LexicalAnalysis2();

    const std::vector<TokenWord>& GetTokenList() const;

    void WriteToFile(std::string_view filePath);
private:
    std::string mInputFilePath;
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

#endif