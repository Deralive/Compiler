# 任务描述：LL(1) 解析器设计

## 1. 任务目标
本关任务：用 C/C++ 编写一个 LL(1) 解析器程序。

## 2. 相关知识
为了完成本关任务，你需要掌握：
1.  **LL 文法**：理解 First 集、Follow 集以及 LL(1) 分析表的构造。
2.  **C/C++ 编程语言基础**。
3.  **LL(1) 解析器原理**：使用分析栈和预测分析表对输入串进行推导。

---

## 3. 实验要求与文法定义

### 3.1 实验文法
在创建解析器之前，请根据以下文法构造 LL(1) 分析表。

**起始符**：`program`
**空符号**：`E` (代表 $\epsilon$, Epsilon)

```text
program -> compoundstmt
stmt -> ifstmt | whilestmt | assgstmt | compoundstmt
compoundstmt -> { stmts }
stmts -> stmt stmts | E
ifstmt -> if ( boolexpr ) then stmt else stmt
whilestmt -> while ( boolexpr ) stmt
assgstmt -> ID = arithexpr ;
boolexpr -> arithexpr boolop arithexpr
boolop -> < | > | <= | >= | ==
arithexpr -> multexpr arithexprprime
arithexprprime -> + multexpr arithexprprime | - multexpr arithexprprime | E
multexpr -> simpleexpr multexprprime
multexprprime -> * simpleexpr multexprprime | / simpleexpr multexprprime | E
simpleexpr -> ID | NUM | ( arithexpr )
```

### 3.2 终结符与保留字
* **界符**：`{` `}` `(` `)` `;`
* **关键字**：`if` `then` `else` `while`
* **运算符**：`=` `>` `<` `>=` `<=` `==` `+` `-` `*` `/`
* **标识符/常数**：`ID` `NUM`
* **空串**：`E`

### 3.3 输入格式要求
* **分隔方式**：同一行的输入字符用一个**空格字符**分隔。
    * 例如：`ID = NUM ;` (红色标记为空格)
    * 输入无其余无关符号。

### 3.4 错误处理
本实验需要考虑错误处理。如果程序不正确（包含语法错误），程序应该：
1.  打印语法错误消息（包含行号）。
2.  修正错误（通常采用恐慌模式或插入/跳过策略），并继续解析，直到生成尽可能完整的语法树。

**错误提示格式示例：**
`语法错误,第4行,缺少";"`

### 3.5 输出格式要求
输出为语法树。
* 在语法树同一层的叶子节点，在输出格式中应有相同的缩进。
* **使用 Tab (`\t`) 来控制缩进**。

---

## 4. 代码模板

### 4.1 头文件 `LLparser.h`
```cpp
// LLparser.h
#include <cstdio>
#include <cstring>
#include <iostream>
#include <map>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
using namespace std;

/* 不要修改这个标准输入函数 */
void read_prog(string& prog)
{
    char c;
    while(scanf("%c",&c)!=EOF){
        prog += c;
    }
}

/* 你可以添加其他函数 */
// 建议在此处定义 Stack, 分析表, 以及核心 parsing 逻辑

void Analysis()
{
    string prog;
    read_prog(prog);
    /* 骚年们 请开始你们的表演 */
    /********* Begin *********/
    
    
    /********* End *********/
}
```

### 4.2 主程序 `main.cpp`
```cpp
#include "LLparser.h"
int main()
{
    Analysis(); 
    return 0;
}
```

---

## 5. 测试样例

### 样例 1：简单赋值

**输入：**
```text
{
 ID = NUM ;
}
```

**预期输出：**
```text
program
	compoundstmt
		{
		stmts
			stmt
				assgstmt
					ID
					=
					arithexpr
						multexpr
							simpleexpr
								NUM
							multexprprime
								E
						arithexprprime
							E
					;
			stmts
				E
		}
```

---

### 样例 2：算术运算

**输入：**
```text
{
ID = ID + NUM ;
}
```

**预期输出：**
```text
program
	compoundstmt
		{
		stmts
			stmt
				assgstmt
					ID
					=
					arithexpr
						multexpr
							simpleexpr
								ID
							multexprprime
								E
						arithexprprime
							+
							multexpr
								simpleexpr
									NUM
								multexprprime
									E
							arithexprprime
								E
					;
			stmts
				E
		}
```

---

### 样例 3：While 循环与错误恢复

**输入：**
```text
{
while ( ID == NUM ) 
{ 
ID = NUM 
}
}
```
*(注意：第 4 行 `ID = NUM` 后缺少分号)*

**预期输出：**
```text
语法错误,第4行,缺少";"
program
	compoundstmt
		{
		stmts
			stmt
				whilestmt
					while
					(
					boolexpr
						arithexpr
							multexpr
								simpleexpr
									ID
								multexprprime
									E
							arithexprprime
								E
						boolop
							==
						arithexpr
							multexpr
								simpleexpr
									NUM
								multexprprime
									E
							arithexprprime
								E
					)
					stmt
						compoundstmt
							{
							stmts
								stmt
									assgstmt
										ID
										=
										arithexpr
											multexpr
												simpleexpr
													NUM
												multexprprime
													E
											arithexprprime
												E
										;
								stmts
									E
							}
			stmts
				E
		}
```

---

### 样例 4：If-Then-Else

**输入：**
```text
{
if ( ID == ID )
then
ID = NUM ;
else
ID = ID * NUM ;
}
```

**预期输出：**
```text
program
	compoundstmt
		{
		stmts
			stmt
				ifstmt
					if
					(
					boolexpr
						arithexpr
							multexpr
								simpleexpr
									ID
								multexprprime
									E
							arithexprprime
								E
						boolop
							==
						arithexpr
							multexpr
								simpleexpr
									ID
								multexprprime
									E
							arithexprprime
								E
					)
					then
					stmt
						assgstmt
							ID
							=
							arithexpr
								multexpr
									simpleexpr
										NUM
									multexprprime
										E
								arithexprprime
									E
							;
					else
					stmt
						assgstmt
							ID
							=
							arithexpr
								multexpr
									simpleexpr
										ID
									multexprprime
										*
										simpleexpr
											NUM
										multexprprime
											E
								arithexprprime
									E
							;
			stmts
				E
		}
```

# LL(1) 分析表完整构建过程

实验前需要构造 LL(1) 分析表，文法的 FIRST 集、FOLLOW 集以及 LL(1) 预测分析表的完整计算过程如下。

## 终结符与非终结符列表

为了表格清晰，我们定义 **E** 代表空串 ($\epsilon$)，**$** 代表输入结束符。

非终结符:
`program`, `stmt`, `compoundstmt`, `stmts`, `ifstmt`, `whilestmt`, `assgstmt`, `boolexpr`, `boolop`, `arithexpr`, `arithexprprime`, `multexpr`, `multexprprime`, `simpleexpr`

终结符:
`{`, `}`, `if`, `while`, `then`, `else`, `ID`, `NUM`, `=`, `;`, `(`, `)`, `+`, `-`, `*`, `/`, `<`, `>`, `<=`, `>=`, `==`

## FIRST 集与 FOLLOW 集计算

| 非终结符       | FIRST 集              | FOLLOW 集                            |
| :------------- | :-------------------- | :----------------------------------- |
| program        | `{`                   | `$`                                  |
| stmt           | `if, while, ID, {`    | `if, while, ID, {, }, else`          |
| compoundstmt   | `{`                   | `$, if, while, ID, {, }, else`       |
| stmts          | `if, while, ID, {, E` | `}`                                  |
| ifstmt         | `if`                  | `if, while, ID, {, }, else`          |
| whilestmt      | `while`               | `if, while, ID, {, }, else`          |
| assgstmt       | `ID`                  | `if, while, ID, {, }, else`          |
| boolexpr       | `ID, NUM, (`          | `)`                                  |
| boolop         | `<, >, <=, >=, ==`    | `ID, NUM, (`                         |
| arithexpr      | `ID, NUM, (`          | `;, ), <, >, <=, >=, ==`             |
| arithexprprime | `+, -, E`             | `;, ), <, >, <=, >=, ==`             |
| multexpr       | `ID, NUM, (`          | `+, -, ;, ), <, >, <=, >=, ==`       |
| multexprprime  | `*, /, E`             | `+, -, ;, ), <, >, <=, >=, ==`       |
| simpleexpr     | `ID, NUM, (`          | `*, /, +, -, ;, ), <, >, <=, >=, ==` |

## SELECT 集

| 产生式                                 | SELECT 集                                      |
| :------------------------------------- | :--------------------------------------------- |
| `program -> compoundstmt`              | `{`                                            |
| `stmt -> ifstmt`                       | `if`                                           |
| `stmt -> whilestmt`                    | `while`                                        |
| `stmt -> assgstmt`                     | `ID`                                           |
| `stmt -> compoundstmt`                 | `{`                                            |
| `compoundstmt -> { stmts }`            | `{`                                            |
| `stmts -> stmt stmts`                  | `if`, `while`, `ID`, `{`                       |
| `stmts -> E`                           | `}`                                            |
| `ifstmt -> if ...`                     | `if`                                           |
| `whilestmt -> while ...`               | `while`                                        |
| `assgstmt -> ID = ...`                 | `ID`                                           |
| `boolexpr -> arithexpr ...`            | `ID`, `NUM`, `(`                               |
| `boolop -> <`                          | `<`                                            |
| `boolop -> >`                          | `>`                                            |
| `boolop -> <=`                         | `<=`                                           |
| `boolop -> >=`                         | `>=`                                           |
| `boolop -> ==`                         | `==`                                           |
| `arithexpr -> multexpr arithexprprime` | `ID`, `NUM`, `(`                               |
| `arithexprprime -> + ...`              | `+`                                            |
| `arithexprprime -> - ...`              | `-`                                            |
| `arithexprprime -> E`                  | `;`, `)`, `<`, `>`, `<=`, `>=`, `==`           |
| `multexpr -> simpleexpr multexprprime` | `ID`, `NUM`, `(`                               |
| `multexprprime -> * ...`               | `*`                                            |
| `multexprprime -> / ...`               | `/`                                            |
| `multexprprime -> E`                   | `+`, `-`, `;`, `)`, `<`, `>`, `<=`, `>=`, `==` |
| `simpleexpr -> ID`                     | `ID`                                           |
| `simpleexpr -> NUM`                    | `NUM`                                          |
| `simpleexpr -> ( ... )`                | `(`                                            |

## LL(1) 分析表编码压缩版

| 编号 | 产生式                                        |
| :--- | :-------------------------------------------- |
| (1)  | program -> compoundstmt                       |
| (2)  | stmt -> ifstmt                                |
| (3)  | stmt -> whilestmt                             |
| (4)  | stmt -> assgstmt                              |
| (5)  | stmt -> compoundstmt                          |
| (6)  | compoundstmt -> { stmts }                     |
| (7)  | stmts -> stmt stmts                           |
| (8)  | stmts -> E                                    |
| (9)  | ifstmt -> if ( boolexpr ) then stmt else stmt |
| (10) | whilestmt -> while ( boolexpr ) stmt          |
| (11) | assgstmt -> ID = arithexpr ;                  |
| (12) | boolexpr -> arithexpr boolop arithexpr        |
| (13) | boolop -> <                                   |
| (14) | boolop -> >                                   |
| (15) | boolop -> <=                                  |
| (16) | boolop -> >=                                  |
| (17) | boolop -> ==                                  |
| (18) | arithexpr -> multexpr A'                      |
| (19) | A' -> + multexpr A'                           |
| (20) | A' -> - multexpr A'                           |
| (21) | A' -> E                                       |
| (22) | multexpr -> simpleexpr M'                     |
| (23) | M' -> * simpleexpr M'                         |
| (24) | M' -> / simpleexpr M'                         |
| (25) | M' -> E                                       |
| (26) | simpleexpr -> ID                              |
| (27) | simpleexpr -> NUM                             |
| (28) | simpleexpr -> ( arithexpr )                   |

注：A' 代表 arithexprprime，M' 代表 multexprprime，E 代表空串。

---

## LL(1) 预测分析编码表


| 非终结符  |   {   |   }   |  if   | while |  ID   |  NUM  |   (   |   )   |   ;   |   =   | then  | else  |   +   |   -   |   *   |   /   |   <   |   >   |  <=   |  >=   |  ==   |
| :-------: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: |
|  program  |  (1)  |       |       |       |       |       |       |       |       |       |       |       |       |       |       |       |       |       |       |       |       |
| compound  |  (6)  |       |       |       |       |       |       |       |       |       |       |       |       |       |       |       |       |       |       |       |       |
|   stmts   |  (7)  |  (8)  |  (7)  |  (7)  |  (7)  |       |       |       |       |       |       |       |       |       |       |       |       |       |       |       |       |
|   stmt    |  (5)  |       |  (2)  |  (3)  |  (4)  |       |       |       |       |       |       |       |       |       |       |       |       |       |       |       |       |
|  ifstmt   |       |       |  (9)  |       |       |       |       |       |       |       |       |       |       |       |       |       |       |       |       |       |       |
| whilestmt |       |       |       | (10)  |       |       |       |       |       |       |       |       |       |       |       |       |       |       |       |       |       |
| assgstmt  |       |       |       |       | (11)  |       |       |       |       |       |       |       |       |       |       |       |       |       |       |       |       |
| boolexpr  |       |       |       |       | (12)  | (12)  | (12)  |       |       |       |       |       |       |       |       |       |       |       |       |       |       |
|  boolop   |       |       |       |       |       |       |       |       |       |       |       |       |       |       |       |       | (13)  | (14)  | (15)  | (16)  | (17)  |
| arithexpr |       |       |       |       | (18)  | (18)  | (18)  |       |       |       |       |       |       |       |       |       |       |       |       |       |       |
|    A'     |       |       |       |       |       |       |       | (21)  | (21)  |       |       |       | (19)  | (20)  |       |       | (21)  | (21)  | (21)  | (21)  | (21)  |
| multexpr  |       |       |       |       | (22)  | (22)  | (22)  |       |       |       |       |       |       |       |       |       |       |       |       |       |       |
|    M'     |       |       |       |       |       |       |       | (25)  | (25)  |       |       |       | (25)  | (25)  | (23)  | (24)  | (25)  | (25)  | (25)  | (25)  | (25)  |
|  simple   |       |       |       |       | (26)  | (27)  | (28)  |       |       |       |       |       |       |       |       |       |       |       |       |       |       |