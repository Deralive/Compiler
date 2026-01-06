# 实验任务：基于 C/C++ 的 SLR(1)/LR(1) 语法分析器

## 1. 任务描述
根据给定的文法，使用 C/C++ 语言编写一个 SLR(1) 或 LR(1) 语法分析器。程序需读取源代码，进行语法分析，输出最右推导过程。若检测到语法错误（如缺少分号），需报错并进行自动纠正，继续完成分析。

## 2. 实验文法
**起始符**: `program`

**文法产生式**:
1.  `program -> compoundstmt`
2.  `stmt -> ifstmt | whilestmt | assgstmt | compoundstmt`
3.  `compoundstmt -> { stmts }`
4.  `stmts -> stmt stmts | E`  *(注: E 代表空串)*
5.  `ifstmt -> if ( boolexpr ) then stmt else stmt`
6.  `whilestmt -> while ( boolexpr ) stmt`
7.  `assgstmt -> ID = arithexpr ;`
8.  `boolexpr -> arithexpr boolop arithexpr`
9.  `boolop -> < | > | <= | >= | ==`
10. `arithexpr -> multexpr arithexprprime`
11. `arithexprprime -> + multexpr arithexprprime | - multexpr arithexprprime | E`
12. `multexpr -> simpleexpr multexprprime`
13. `multexprprime -> * simpleexpr multexprprime | / simpleexpr multexprprime | E`
14. `simpleexpr -> ID | NUM | ( arithexpr )`

**Token 定义**:
- **保留字/符号**: `{`, `}`, `if`, `(`, `)`, `then`, `else`, `while`, `=`, `;`, `>`, `<`, `>=`, `<=`, `==`, `+`, `-`, `*`, `/`
- **操作数**: `ID`, `NUM`
- **空串**: `E`

## 3. 输入输出要求
- **输入**: 标准流输入。Token 之间以空格或换行分隔。
- **输出**: 
  - 对于正确代码，输出最右推导过程。
  - 推导格式：每一步推导占一行，符号间空格分隔，箭头为 `=>`。
  - **错误处理**: 遇到语法错误（如缺少 `;`）时，需输出 `语法错误,第x行,缺少";"`，然后修正错误并继续输出后续推导。

## 4. 样例参考
**输入**:

```text
{
  ID = NUM ;
}
```

```text
program => 
compoundstmt => 
{ stmts } => 
{ stmt stmts } => 
{ stmt } => 
{ assgstmt } => 
{ ID = arithexpr ; } => 
{ ID = multexpr arithexprprime ; } => 
{ ID = multexpr ; } => 
{ ID = simpleexpr multexprprime ; } => 
{ ID = simpleexpr ; } => 
{ ID = NUM ; } 
```

```text
{
ID = ID + NUM ;
}
```

输出：

```text
program => 
compoundstmt => 
{ stmts } => 
{ stmt stmts } => 
{ stmt } => 
{ assgstmt } => 
{ ID = arithexpr ; } => 
{ ID = multexpr arithexprprime ; } => 
{ ID = multexpr + multexpr arithexprprime ; } => 
{ ID = multexpr + multexpr ; } => 
{ ID = multexpr + simpleexpr multexprprime ; } => 
{ ID = multexpr + simpleexpr ; } => 
{ ID = multexpr + NUM ; } => 
{ ID = simpleexpr multexprprime + NUM ; } => 
{ ID = simpleexpr + NUM ; } => 
{ ID = ID + NUM ; } 
```

```text
{

while ( ID == NUM ) 

{ 

ID = NUM 

}

}
```

```text
语法错误，第4行，缺少";"
program => 
compoundstmt => 
{ stmts } => 
{ stmt stmts } => 
{ stmt } => 
{ whilestmt } => 
{ while ( boolexpr ) stmt } => 
{ while ( boolexpr ) compoundstmt } => 
{ while ( boolexpr ) { stmts } } => 
{ while ( boolexpr ) { stmt stmts } } => 
{ while ( boolexpr ) { stmt } } => 
{ while ( boolexpr ) { assgstmt } } => 
{ while ( boolexpr ) { ID = arithexpr ; } } => 
{ while ( boolexpr ) { ID = multexpr arithexprprime ; } } => 
{ while ( boolexpr ) { ID = multexpr ; } } => 
{ while ( boolexpr ) { ID = simpleexpr multexprprime ; } } => 
{ while ( boolexpr ) { ID = simpleexpr ; } } => 
{ while ( boolexpr ) { ID = NUM ; } } => 
{ while ( arithexpr boolop arithexpr ) { ID = NUM ; } } => 
{ while ( arithexpr boolop multexpr arithexprprime ) { ID = NUM ; } } => 
{ while ( arithexpr boolop multexpr ) { ID = NUM ; } } => 
{ while ( arithexpr boolop simpleexpr multexprprime ) { ID = NUM ; } } => 
{ while ( arithexpr boolop simpleexpr ) { ID = NUM ; } } => 
{ while ( arithexpr boolop NUM ) { ID = NUM ; } } => 
{ while ( arithexpr == NUM ) { ID = NUM ; } } => 
{ while ( multexpr arithexprprime == NUM ) { ID = NUM ; } } => 
{ while ( multexpr == NUM ) { ID = NUM ; } } => 
{ while ( simpleexpr multexprprime == NUM ) { ID = NUM ; } } => 
{ while ( simpleexpr == NUM ) { ID = NUM ; } } => 
{ while ( ID == NUM ) { ID = NUM ; } } 
```

```text
program => 
compoundstmt => 
{ stmts } => 
{ stmt stmts } => 
{ stmt } => 
{ ifstmt } => 
{ if ( boolexpr ) then stmt else stmt } => 
{ if ( boolexpr ) then stmt else assgstmt } => 
{ if ( boolexpr ) then stmt else ID = arithexpr ; } => 
{ if ( boolexpr ) then stmt else ID = multexpr arithexprprime ; } => 
{ if ( boolexpr ) then stmt else ID = multexpr ; } => 
{ if ( boolexpr ) then stmt else ID = simpleexpr multexprprime ; } => 
{ if ( boolexpr ) then stmt else ID = simpleexpr * simpleexpr multexprprime ; } => 
{ if ( boolexpr ) then stmt else ID = simpleexpr * simpleexpr ; } => 
{ if ( boolexpr ) then stmt else ID = simpleexpr * NUM ; } => 
{ if ( boolexpr ) then stmt else ID = ID * NUM ; } => 
{ if ( boolexpr ) then assgstmt else ID = ID * NUM ; } => 
{ if ( boolexpr ) then ID = arithexpr ; else ID = ID * NUM ; } => 
{ if ( boolexpr ) then ID = multexpr arithexprprime ; else ID = ID * NUM ; } => 
{ if ( boolexpr ) then ID = multexpr ; else ID = ID * NUM ; } => 
{ if ( boolexpr ) then ID = simpleexpr multexprprime ; else ID = ID * NUM ; } => 
{ if ( boolexpr ) then ID = simpleexpr ; else ID = ID * NUM ; } => 
{ if ( boolexpr ) then ID = NUM ; else ID = ID * NUM ; } => 
{ if ( arithexpr boolop arithexpr ) then ID = NUM ; else ID = ID * NUM ; } => 
{ if ( arithexpr boolop multexpr arithexprprime ) then ID = NUM ; else ID = ID * NUM ; } => 
{ if ( arithexpr boolop multexpr ) then ID = NUM ; else ID = ID * NUM ; } => 
{ if ( arithexpr boolop simpleexpr multexprprime ) then ID = NUM ; else ID = ID * NUM ; } => 
{ if ( arithexpr boolop simpleexpr ) then ID = NUM ; else ID = ID * NUM ; } => 
{ if ( arithexpr boolop ID ) then ID = NUM ; else ID = ID * NUM ; } => 
{ if ( arithexpr == ID ) then ID = NUM ; else ID = ID * NUM ; } => 
{ if ( multexpr arithexprprime == ID ) then ID = NUM ; else ID = ID * NUM ; } => 
{ if ( multexpr == ID ) then ID = NUM ; else ID = ID * NUM ; } => 
{ if ( simpleexpr multexprprime == ID ) then ID = NUM ; else ID = ID * NUM ; } => 
{ if ( simpleexpr == ID ) then ID = NUM ; else ID = ID * NUM ; } => 
{ if ( ID == ID ) then ID = NUM ; else ID = ID * NUM ; } 
```