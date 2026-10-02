#pragma once
#include <string>
#include <iostream>
#include <memory>

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

struct FuncParam 
{
	int type;				//0 = int, 1 = char, 2 = void
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
	PCodeLabel() { }

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

class PCode 
{

public:
	PCode() {}

	PCode(std::string name) : name(name) 
	{}
	PCode(std::string name, int addr) : name(name), addr(addr)
	{}
	PCode(std::string name, std::string print) : name(name), print(print) 
	{}
	PCode(std::string name, std::shared_ptr<PCodeLabel> label) : name(name), label(label) 
	{}
	PCode(std::string name, int scope, int addr) : name(name), scope(scope), addr(addr) 
	{}

	string GetName()
	{
		return name;
	}

	int GetScope() 
	{
		return scope;
	}

	int Getaddr()
	{
		return addr;
	}

	string GetPrint()
	{
		return print;
	}

	shared_ptr<PCodeLabel> GetLabel()
	{
		return label;
	}
private:
	string name;
	int scope = 0;
	int addr = 0;
	string print = "";
	shared_ptr<PCodeLabel> label = nullptr;
};

class Value {
private:
	int value;
	std::string Svalue;
	bool isInt;
public:
	Value(int value) : value(value), isInt(true) {}
	Value(std::string Svalue) : Svalue(Svalue), isInt(false) {}

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

	std::string getSValue() {
		if (!isInt) {
			return Svalue;
		}

		return 0;
	}

	void setSValue(std::string Svalue) {
		this->Svalue = Svalue;
		isInt = false;
	}
};