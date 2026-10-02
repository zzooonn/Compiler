#include "Interpreter.h"
#include <iostream>
#include <string>
#include <algorithm>
#include <fstream>

using namespace std;

// 把Dstack初始为0
Interpreter::Interpreter()
{
	mDStack.fill(0);
}

vector<string> Interpreter::Inter(const vector<shared_ptr<PCode>>& mCodelist)
{
	int addr;

	while (at < mCodelist.size()) {
		auto pCurCode = mCodelist[at];

		if (pCurCode->GetLabel() != nullptr) {
			cout << "code.GetLabel() = " << pCurCode->GetLabel()->GetAddr() << endl;
		}

		// INT和INT_L的主要区别在于INT直接使用固定地址分配栈空间，而 INT_L通过标签引用动态计算分配的栈空间。
		if (pCurCode->GetName() == "INT")
		{
			sp += pCurCode->Getaddr();
			at++;
		}
		else if (pCurCode->GetName() == "INT_L")
		{
			sp += pCurCode->GetLabel()->GetAddr();
			at++;
		}
		else if (pCurCode->GetName() == "DOWN")
		{
			sp -= pCurCode->Getaddr();
			at++;
		}

		// LOD는 특정 메모리 주소에서 있는 값을 가져와 스택에 push
		else if (pCurCode->GetName() == "LOD")
		{
			sp++;
			if (pCurCode->GetScope() == 0) {
				addr = BAddr + pCurCode->Getaddr();
			}
			else {
				addr = pCurCode->Getaddr();
			}
			mDStack[sp] = mDStack[addr];
			if (mainBAddr == BAddr) {
				mDStack[sp] = addr;
			}
			at++;
		}

		// LODS는 스택에 있는 주소값을 사용해서, 해당 메모리 위치의 값을 다시 스택에 push
		else if (pCurCode->GetName() == "LODS")
		{
			mDStack[sp] = mDStack[mDStack[sp]];
			at++;
		}

		// 변수 또는 배열의 주소를 로드
		else if (pCurCode->GetName() == "LOD_A")
		{
			bool iscasearr = false;
			sp++;
			if (pCurCode->GetScope() == 0) {
				addr = BAddr + pCurCode->Getaddr();
			}
			else if (pCurCode->GetScope() == 1) {
				addr = pCurCode->Getaddr();
			}
			else {
				addr = BAddr + pCurCode->Getaddr();
				iscasearr = true;
			}
			if (!iscasearr) {
				mDStack[sp] = addr;
			}
			else {
				mDStack[sp] = mDStack[addr];
			}

			at++;
		}

		// 상수(Constant) 값을 스택에 저장 
		else if (pCurCode->GetName() == "LOD_C")
		{
			sp++;
			mDStack[sp] = pCurCode->Getaddr();
			at++;
		}

		// 스택에 있는 값을 꺼내어 메모리에 저장
		else if (pCurCode->GetName() == "STO")
		{
			sp--;
			mDStack[mDStack[sp]] = mDStack[sp + 1];
			sp--;
			at++;
		}

		// 조건없이 특정 위치로 점프 
		else if (pCurCode->GetName() == "JMP")
		{
			BAddr = sp + 1;
			mainBAddr = BAddr;
			at = pCurCode->GetLabel()->GetAddr();
		}
		else if (pCurCode->GetName() == "RET")
		{
			at = mDStack[BAddr + 2];
			sp = BAddr;
			BAddr = mDStack[BAddr + 1];
		}
		else if (pCurCode->GetName() == "CAL")
		{
			mDStack[sp + 1] = 0;
			mDStack[sp + 2] = BAddr;
			mDStack[sp + 3] = at + 1;
			BAddr = sp + 1;
			sp = sp + 3;
			at = pCurCode->Getaddr();
		}

		// 사칙연산 
		else if (pCurCode->GetName() == "ADD")
		{
			sp--;
			mDStack[sp] = mDStack[sp] + mDStack[sp + 1];
			at++;
		}
		else if (pCurCode->GetName() == "SUB")
		{
			sp--;
			mDStack[sp] = mDStack[sp] - mDStack[sp + 1];
			at++;
		}
		else if (pCurCode->GetName() == "MUL")
		{
			sp--;
			mDStack[sp] = mDStack[sp] * mDStack[sp + 1];
			at++;
		}
		else if (pCurCode->GetName() == "DIV")
		{
			sp--;
			mDStack[sp] = mDStack[sp] / mDStack[sp + 1];
			at++;
		}
		else if (pCurCode->GetName() == "MOD")
		{
			sp--;
			mDStack[sp] = mDStack[sp] % mDStack[sp + 1];
			at++;
		}
		// 为了char或ASCII值的范围
		else if (pCurCode->GetName() == "MOD128")
		{
			mDStack[sp] = mDStack[sp] % 128;
			at++;
		}

		else if (pCurCode->GetName() == "MINU")
		{
			mDStack[sp] = -mDStack[sp];
			at++;
		}

		// GET是用int类型存，GETC是用ASCII类型存
		else if (pCurCode->GetName() == "GET")
		{
			sp++;
			cin >> mDStack[sp];
			at++;
		}
		else if (pCurCode->GetName() == "GETC")
		{
			sp++;
			char input;
			cin >> input;
			mDStack[sp] = static_cast<int>(input);
			at++;
		}

		// printf
		else if (pCurCode->GetName() == "PRF")
		{
			// 获取要处理的字符串
			std::string s = pCurCode->GetPrint();
			// 去除字符串中的双引号
			s.erase(std::remove(s.begin(), s.end(), '\"'), s.end());
			// 统计格式指定符的数量并调整栈指针
			int cnt = std::count(s.begin(), s.end(), '%');
			sp = sp - cnt;
			// 替换格式指定符
			for (int i = 0; i < cnt; i++) {
				size_t pos = s.find('%');
				if (pos != std::string::npos) {
					if (s[pos + 1] == 'd') {
						s.replace(pos, 2, std::to_string(mDStack[sp + i + 1]));
					}
					else if (s[pos + 1] == 'c') {
						char ch = static_cast<char>(mDStack[sp + i + 1] % 128);
						s.replace(pos, 2, std::string(1, ch));
					}
				}
			}

			at++;
			// 处理换行符
			size_t pos = 0;
			while ((pos = s.find("\\n", pos)) != std::string::npos)
			{
				s.replace(pos, 2, "\n");
				pos += 1;
			}

			mPrintResult.push_back(s);
		}

		// >, < 
		else if (pCurCode->GetName() == "BGT") {
			sp--;
			mDStack[sp] = (mDStack[sp] > mDStack[sp + 1]) ? 1 : 0;
			at++;
		}
		else if (pCurCode->GetName() == "BGE") {
			sp--;
			mDStack[sp] = (mDStack[sp] >= mDStack[sp + 1]) ? 1 : 0;
			at++;
		}
		else if (pCurCode->GetName() == "BLT") {
			sp--;
			mDStack[sp] = (mDStack[sp] < mDStack[sp + 1]) ? 1 : 0;
			at++;
		}
		else if (pCurCode->GetName() == "BLE") {
			sp--;
			mDStack[sp] = (mDStack[sp] <= mDStack[sp + 1]) ? 1 : 0;
			at++;
		}
		else if (pCurCode->GetName() == "BEQ") {
			sp--;
			mDStack[sp] = (mDStack[sp] == mDStack[sp + 1]) ? 1 : 0;
			at++;
		}
		else if (pCurCode->GetName() == "BNE") {
			sp--;
			mDStack[sp] = (mDStack[sp] != mDStack[sp + 1]) ? 1 : 0;
			at++;
		}
		else if (pCurCode->GetName() == "BZT") {
			if (mDStack[sp] == 0) {
				at = pCurCode->GetLabel()->GetAddr();
			}
			else {
				at++;
			}
			sp--;
		}

		// 无条件jump
		else if (pCurCode->GetName() == "J") {
			at = pCurCode->GetLabel()->GetAddr();
		}
		// if = 0， jump
		else if (pCurCode->GetName() == "JP0") {
			if (mDStack[sp] == 0) {
				at = pCurCode->GetLabel()->GetAddr();
			}
			else {
				at++;
			}
		}
		// if = 1 ，jump
		else if (pCurCode->GetName() == "JP1") {
			if (mDStack[sp] == 1)
			{
				at = pCurCode->GetLabel()->GetAddr();
			}
			else {
				at++;
			}
		}

		// ！符号处理
		else if (pCurCode->GetName() == "NOT") {
			if (mDStack[sp] == 0) 
			{
				mDStack[sp] = 1;
			}
			else {
				mDStack[sp] = 0;
			}
			at++;
		}
		else {
			at++;
		}
		if (sp < mDStack.size())
		{
			cout << "sp == " << sp << "  " << "sp value == " << mDStack[sp] << "  baddr == " << BAddr << "\n\n";
		}
	}
	return mPrintResult;
}

void Interpreter::WriteToFile(string_view filePath)
{
	fstream outFile(filePath.data(), ios::trunc | ios::out);
	if (outFile.good())
	{
		for (auto& code : mPrintResult)
		{
			outFile << code;
			std::cout << code;
		}
	}
	outFile.flush();
	outFile.close();
}

