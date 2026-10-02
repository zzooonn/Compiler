#ifndef SYMBOLTABLEMANAGER_H
#define SYMBOLTABLEMANAGER_H

#include <string>
#include <vector>

using namespace std;

struct SymbolTable 
{
    string name;
    int type = -1;	        //0 = int, 1 = char, 2 = void
    int kind = -1;	        //0 = var, 1 = func, 2 = param, 3 = arr
    int isconst = 0;	    //cosnt = 1, not const = 0
    int scope;
    int pcodearr = 0;
    int arrsize = 0;
};

class SymbolTableManager {
private:
    vector<SymbolTable> symbolTable;
    SymbolTable SymbolTemp;

public:
    SymbolTableManager();
    const SymbolTable& GetTempSymbol() { return SymbolTemp; }

    void add_s_Table(const string& name, int type, int kind, int isconst, int mPcodeAddress, bool mIsGlbScope, bool mIsEqualScope, int mGlobalSymbolScope, int mCurScope, int mSaveArrExp);
    void print_symbol_list();

    SymbolTable* findSymbol(const string& name, int scope);
    void findAndSetSymbol(const string& tokenName);
};

#endif