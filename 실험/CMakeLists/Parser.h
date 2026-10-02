#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <string>
#include <stack>
#include "DataDef.h"
#include "Lexer.h"
#include "SymbolTableManager.h"
#include "ErrorHandler.h"

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
    int mSaveArrExp = 0;
    bool mIsfuncDef = false;
    bool mIsforincontinue = false;
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
    void MoveToPreviousToken ();

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



#endif