#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <stack>
#include <cctype> 

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

vector<Token> tokens;
vector<Error> errors;

/*
unordered_map<string, TokenType> currentScopeSymbols;     // 현재 스코프의 심볼 테이블
stack<unordered_map<string, TokenType>> symbolTableStack; // 스코프 스택
*/

int lineNumber = 1;

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

/*
void enterScope(){
    symbolTableStack.push(currentScopeSymbols);
    currentScopeSymbols.clear();
}

void exitScope(){
    if(!symbolTableStack.empty())
    {
        currentScopeSymbols = symbolTableStack.top();
        symbolTableStack.pop();
    }
}
*/

int main() 
{
    ifstream inputFile("testfile.txt"); 
    char currentChar;

    while (inputFile.get(currentChar)) 
    {
        if (isspace(currentChar)) 
        {
            if (currentChar == '\n') 
            {
                lineNumber++;
            }
            continue; 
        }

        // 주석 처리 및 나누기 처리
        else if(currentChar == '/')
        {
            char nextChar = inputFile.peek();

            // 주석인데 /*로 시작할 경우
            if(nextChar == '*')
            {
                inputFile.get(); // '*' 소비
                bool endofComment = false;

                while(inputFile.get(currentChar))
                {
                    if(currentChar == '\n')
                    {
                        lineNumber++;
                    }

                    else if(currentChar == '*')
                    {
                        if(inputFile.peek() == '/')
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
            else if(nextChar == '/')
            {
                inputFile.get(); // 두 번째 '/' 소비
                while(inputFile.get(currentChar))
                {
                    if(currentChar == '\n' || currentChar == '\r')
                    {
                        lineNumber++; // 줄 번호 증가
                        break;
                    }
                    if (inputFile.eof()) break; // 파일 끝에 도달 시 종료
                }

                continue;
            }

            // 나눗셈인 경우
            else
            {
                tokens.push_back({DIV, "/", lineNumber});
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
                tokens.push_back({keywords[ident], ident, lineNumber});
            }
            else 
            {
                // 식별자 토큰 추가
                tokens.push_back({IDENFR, ident, lineNumber});
            }
        }


        // 숫자 처리
        else if (isdigit(currentChar)) 
        {
            string number;
            number += currentChar;

            // EOF 체크 추가
            while (isdigit(inputFile.peek())) 
            {
                number += inputFile.get();
            }
            tokens.push_back({INTCON, number, lineNumber});
        }

        // 문자열 상수 처리
        else if (currentChar == '"') 
        {
            string strConst;
            strConst += currentChar;
            bool closed = false;

            while (inputFile.peek() != EOF) 
            {
                char c = inputFile.get();
                strConst += c;

                if (c == '"') 
                {
                    closed = true;
                    break;
                }

                if (c == '\n') 
                {
                    lineNumber++;
                    break;
                }

                if (inputFile.eof()) break; // 파일 끝에 도달 시 종료
            }

            if (closed) 
            {
                tokens.push_back({STRCON, strConst, lineNumber});
            }
        }

        // 문자 상수 처리
        else if (currentChar == '\'') 
        {
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

            if (inputFile.peek() == '\'') 
            {
                charConst += inputFile.get(); // 종료 따옴표 추가
                tokens.push_back({CHRCON, charConst, lineNumber});
            } 

            else
            {
                // 오류 발생 시 errors 벡터에 추가
                errors.push_back({lineNumber, 'a'}); // 오류 코드 설정
            }
        }
        
        // 연산자 및 구분자 처리
        else 
        {
            string op(1, currentChar);
            char nextChar = inputFile.peek();

            if (currentChar == '&') 
            {
                if(nextChar == '&')
                {
                    op += inputFile.get();
                    tokens.push_back({AND, op, lineNumber});
                }
                else
                {
                    errors.push_back({lineNumber, 'a'}); // 단일 '&' 오류
                }
            }

            else if (currentChar == '|') 
            {
                if(nextChar == '|')
                {
                    op += inputFile.get();
                    tokens.push_back({OR, op, lineNumber});
                }
                else
                {
                    errors.push_back({lineNumber, 'a'}); // 단일 '|' 오류
                }
            }

            else if ((currentChar == '!' || currentChar == '=' || currentChar == '<' || currentChar == '>') && nextChar == '=') 
            {
                op += inputFile.get();
                tokens.push_back({operators[op], op, lineNumber});
            }

            else if (operators.find(op) != operators.end()) 
            {
                tokens.push_back({operators[op], op, lineNumber});
            }
        }

        if (inputFile.eof()) break; // 파일 끝에 도달 시 종료
    }

    inputFile.close();

    // 오류 발생 여부에 따라 출력 파일 결정
    if (errors.empty())
    {
        ofstream outputFile("lexer.txt");
        for (const auto& token : tokens) 
        {
            outputFile << tokenTypeToString(token.type) << " " << token.value << endl;
        }
        outputFile.close();
    }
    else 
    {
        ofstream errorFile("error.txt");
        for (const auto& error : errors)
        {
            errorFile << error.lineNumber << " " << error.errorcode << endl;
        }
        errorFile.close();
    }

    return 0;
}

