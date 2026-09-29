# C-Minus Compiler

A C-Minus compiler built on the Flex/Bison skeleton from Kenneth C. Louden's
*Compiler Construction: Principles and Practice*, extended from the book's
TINY example to the full C-Minus language.

[English](#english) | [한국어](#한국어)

---

## English

### Status

| Phase | Status |
|---|---|
| Lexical analysis (scanner) | ✅ Implemented |
| Syntax analysis (parser) | ✅ Implemented |
| Semantic analysis (symbol table, type checking) | 🚧 In progress |
| Code generation | 🚧 In progress |

The scanner and parser build a syntax tree for the full C-Minus grammar
(declarations, functions, arrays, control flow, expressions). A scope-aware
symbol table (`symtab.c`/`symtab.h`) is in place; the semantic analysis pass
that uses it to build scopes and type-check the tree is being implemented
next, followed by code generation.

### Requirements

- `flex`
- `bison`
- `gcc` (or any C compiler)

### Build

```bash
bison -d -y cminus.y      # generates y.tab.c / y.tab.h
gcc -o cminus main.c util.c scan.c y.tab.c
```

### Run

```bash
./cminus <source-file>.cm
```

This prints the token stream (if tracing is enabled) and the parsed syntax
tree for the given C-Minus source file.

### Project layout

| File | Purpose |
|---|---|
| `cminus.l` | Flex specification for the scanner (alternate scanner pipeline) |
| `scan.c` / `scan.h` | Hand-written DFA-based scanner |
| `cminus.y` | Bison grammar for the C-Minus parser |
| `globals.h` | Shared types (`TreeNode`, token/node kinds, tracing flags) |
| `util.c` / `util.h` | Syntax tree construction and printing helpers |
| `symtab.c` / `symtab.h` | Scope-aware symbol table (hash table per scope) |
| `symtab_test.c` | Standalone test/demo for the symbol table module |
| `main.c` | Driver: scans, parses, (soon) analyzes and generates code |

> Note: `scan.c` and `cminus.l` are two alternative scanner implementations
> and both define `getToken()` — only one should be linked into a given
> build, not both.

---

## 한국어

### 진행 상황

| 단계 | 상태 |
|---|---|
| 어휘 분석 (스캐너) | ✅ 구현 완료 |
| 구문 분석 (파서) | ✅ 구현 완료 |
| 의미 분석 (심볼 테이블, 타입 체크) | 🚧 구현 중 |
| 코드 생성 | 🚧 구현 중 |

스캐너와 파서는 C-Minus 전체 문법(선언, 함수, 배열, 제어문, 수식)에 대한
구문 트리를 생성합니다. 스코프를 인식하는 심볼 테이블(`symtab.c`/`symtab.h`)이
준비되어 있으며, 이를 활용해 스코프를 구성하고 타입을 검사하는 의미 분석
단계를 다음으로 구현할 예정이고, 이어서 코드 생성을 진행할 계획입니다.

### 요구 사항

- `flex`
- `bison`
- `gcc` (또는 다른 C 컴파일러)

### 빌드

```bash
bison -d -y cminus.y      # y.tab.c / y.tab.h 생성
gcc -o cminus main.c util.c scan.c y.tab.c
```

### 실행

```bash
./cminus <소스파일>.cm
```

주어진 C-Minus 소스 파일에 대해 (트레이싱이 켜져 있으면) 토큰 스트림과
파싱된 구문 트리를 출력합니다.

### 프로젝트 구성

| 파일 | 설명 |
|---|---|
| `cminus.l` | 스캐너용 Flex 명세 (대체 스캐너 파이프라인) |
| `scan.c` / `scan.h` | 손으로 작성한 DFA 기반 스캐너 |
| `cminus.y` | C-Minus 파서를 위한 Bison 문법 |
| `globals.h` | 공용 타입 정의 (`TreeNode`, 토큰/노드 종류, 트레이싱 플래그) |
| `util.c` / `util.h` | 구문 트리 생성 및 출력 헬퍼 함수 |
| `symtab.c` / `symtab.h` | 스코프 기반 심볼 테이블 (스코프별 해시 테이블) |
| `symtab_test.c` | 심볼 테이블 모듈 단독 테스트/데모 |
| `main.c` | 드라이버: 스캔 → 파싱 → (예정) 의미 분석 → 코드 생성 |

> 참고: `scan.c`와 `cminus.l`은 서로 다른 두 스캐너 구현체이며 둘 다
> `getToken()`을 정의하므로, 빌드 시 둘 중 하나만 링크해야 합니다.
