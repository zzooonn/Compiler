# SysY 컴파일러

**C++17로 작성한 SysY 계열 교육용 언어의 컴파일러와 P-code 인터프리터입니다.**

- 소스 코드를 토큰으로 분리하고, 문법·의미를 분석한 뒤 P-code를 생성합니다.
- 생성한 명령어는 스택 기반 인터프리터에서 실행합니다.
- 단계별 실험 코드, 수정 버전, 과제와 개발 과정 정리 문서를 함께 보관합니다.

## 빠른 탐색

- [실행 대상으로 안내하는 수정 버전](실험/수정/)
- [단계별 구현 변화와 문제 해결 과정](실험/70216005_컴파일러_변화_분석_최종.md)
- [컴파일러 설계 실험 보고서](실험/70216005-曹贤准-编译器设计实验感想.pdf)
- [이론 과제](作业/)

## 처리 흐름

```text
testfile.txt
    → Lexer: 어휘 분석
    → Parser + SymbolTableManager + ErrorHandler: 문법·의미 분석
    → P-code 생성
    → Interpreter: 명령어 실행
    → pcoderesult.txt
```

## 주요 구현

| 영역 | 내용 | 확인할 파일 |
| --- | --- | --- |
| 어휘 분석 | 식별자, 예약어, 상수, 연산자, 주석 처리 | [Lexer.cpp](실험/수정/Lexer.cpp) |
| 구문 분석 | 재귀 하강 방식의 선언·함수·문장·표현식 분석 | [Parser.cpp](실험/수정/Parser.cpp) |
| 의미 분석 | 스코프와 심볼 관리, 정의·참조 및 함수 인자 검사 | [SymbolTableManager.cpp](실험/수정/SymbolTableManager.cpp) |
| 오류 처리 | 과제 규칙에 따른 오류 코드와 줄 번호 기록 | [ErrorHandler.cpp](실험/수정/ErrorHandler.cpp) |
| 코드 생성 | 분기, 반복, 함수 호출을 위한 P-code와 라벨 구성 | [Parser.cpp](실험/수정/Parser.cpp), [DataDef.h](실험/수정/DataDef.h) |
| 실행 | 스택과 호출 프레임 기반 P-code 해석 | [Interpreter.cpp](실험/수정/Interpreter.cpp) |

## 저장소 구성

```text
Compiler/
├── 실험/
│   ├── 70216005-1/        # 초기 테스트 입력·출력
│   ├── 70216005-2/        # 어휘 분석 단계
│   ├── 70216005-3/        # 구문 분석 단계
│   ├── 70216005-4/        # 의미 분석 단계
│   ├── 70216005-5/        # P-code 생성·실행 단계
│   ├── CMakeLists/        # 모듈화 버전 및 관련 문서
│   ├── cmake/             # 중간 수정 버전
│   ├── 수정/              # 아래 실행 안내의 대상 버전
│   └── ...                # 연습 파일·실험 보고서·변화 분석 문서
├── 作业/                  # 이론 과제
└── examples/smoke/        # 최소 실행 예제
```

- 원본의 폴더 구조와 단계별 제출 압축파일을 유지했습니다.
- 여러 버전의 소스를 한 실행 파일에 섞지 않고, 아래 예제는 `실험/수정/`만 사용합니다.
- 교재·강의 슬라이드·외부 참고 코드와 IDE 캐시·빌드 산출물은 업로드 범위에서 제외했습니다.

## 빌드

- 필요 환경: C++17을 지원하는 GCC 또는 Clang.
- 선택 도구: CMake 3.10 이상. 기존 CMake 설정은 Clang을 지정합니다.
- 아래 명령은 저장소 루트에서 실행합니다.

### GCC 직접 빌드

```sh
mkdir -p build
g++ -std=c++17 실험/수정/ComplierSysY.cpp 실험/수정/ErrorHandler.cpp 실험/수정/Interpreter.cpp 실험/수정/Lexer.cpp 실험/수정/Parser.cpp 실험/수정/SymbolTableManager.cpp -o build/Compiler
```

- Windows PowerShell에서는 `mkdir -p build` 대신 `New-Item -ItemType Directory -Force build`를 사용하고 출력 경로를 `build/Compiler.exe`로 지정합니다.

### CMake 빌드

```sh
cmake -S 실험/수정 -B build -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++
cmake --build build
```

- CMake의 실행 파일 위치는 선택한 생성기에 따라 달라집니다. 아래 실행 예제는 직접 빌드한 파일의 경로를 사용합니다.

## 실행

- 프로그램은 **현재 작업 폴더의 `testfile.txt`**를 읽습니다. 입력 파일 경로를 명령행 인자로 받는 구조는 아닙니다.
- [최소 예제](examples/smoke/testfile.txt)는 `2 + 3`을 계산하여 출력합니다.

```sh
cd examples/smoke
../../build/Compiler
cat pcoderesult.txt
```

```powershell
# Windows PowerShell: 저장소 루트에서 실행
Set-Location examples/smoke
& ../../build/Compiler.exe
Get-Content pcoderesult.txt
```

예상 결과:

```text
5
```

| 파일 | 내용 |
| --- | --- |
| `lexer.txt` | 어휘 분석 결과 |
| `pcode.txt` | 생성한 P-code |
| `error.txt` | 구문·의미 오류가 있을 때 기록한 오류 목록 |
| `pcoderesult.txt` | 인터프리터의 프로그램 출력 |

- `getint`·`getchar`를 사용하는 입력은 실행 중 표준 입력을 받습니다.
- 실행 중에는 분석·명령어 관련 디버그 메시지도 표준 출력에 표시될 수 있습니다.
- `error.txt`는 오류가 있을 때만 작성하므로, 이전 실행의 파일이 남아 있다면 실행 시점을 확인해야 합니다.

## 확인 범위와 현재 제약

- Windows의 MinGW GCC에서 수정 버전의 빌드와 위 최소 예제를 확인했습니다.
- CMake 경로와 전체 과제 테스트의 통과 여부는 별도로 검증하지 않았습니다.
- 입력 파일명이 고정되어 있으며, 오류를 발견한 뒤에도 현재 진입점은 인터프리터 실행을 이어갑니다.
- 전체 C/C++ 언어를 대상으로 하지 않는 수업용 언어 구현이며, 기계어 대신 P-code를 생성합니다.
- 단계별 결함과 개선 과정은 [변화 분석 문서](실험/70216005_컴파일러_변화_분석_최종.md)에 정리되어 있습니다.
