#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#define TEST_MODE
#include "LexAnalysis.h"

using namespace std;

void run_test(const string &testName, const string &input, const string &expectedOutput) {
    cout << "Running " << testName << "..." << endl;

    streambuf *oldCin = cin.rdbuf();
    streambuf *oldCout = cout.rdbuf();
    stringstream inputSS(input);
    stringstream outputSS;
    cin.rdbuf(inputSS.rdbuf());
    cout.rdbuf(outputSS.rdbuf());
    Analysis();
    cin.rdbuf(oldCin);
    cout.rdbuf(oldCout);
    string actual = outputSS.str();
    if (actual == expectedOutput) {
        cout << "[PASS]" << endl;
    } else {
        cout << "[FAIL]" << endl;
        cout << "--- Expected ---" << endl << expectedOutput << endl;
        cout << "--- Actual ---" << endl << actual << endl;
    }
    cout << "------------------------------------------------" << endl;
}

int main() {
    // Test Case 1: 基础 Hello World
    string input1 = R"(int main()
{
    printf("HelloWorld");
    return 0;
    })";
    string expect1 = "1: <int,17>\n"
                     "2: <main,81>\n"
                     "3: <(,44>\n"
                     "4: <),45>\n"
                     "5: <{,59>\n"
                     "6: <printf,81>\n"
                     "7: <(,44>\n"
                     "8: <\",78>\n"
                     "9: <HelloWorld,81>\n"
                     "10: <\",78>\n"
                     "11: <),45>\n"
                     "12: <;,53>\n"
                     "13: <return,20>\n"
                     "14: <0,80>\n"
                     "15: <;,53>\n"
                     "16: <},63>";
    run_test("Test Case 1 (Basic)", input1, expect1);

    // Test Case 2: 循环与单行注释
    string input2 = R"(int main()
{
    int i = 0;// 注释 test
    for (i = 0; i != 10; ++i)
    {
        printf("%d",i);
    }
    return 0;
})";
    string expect2 = "1: <int,17>\n"
                     "2: <main,81>\n"
                     "3: <(,44>\n"
                     "4: <),45>\n"
                     "5: <{,59>\n"
                     "6: <int,17>\n"
                     "7: <i,81>\n"
                     "8: <=,72>\n"
                     "9: <0,80>\n"
                     "10: <;,53>\n"
                     "11: <// 注释 test,79>\n"
                     "12: <for,14>\n"
                     "13: <(,44>\n"
                     "14: <i,81>\n"
                     "15: <=,72>\n"
                     "16: <0,80>\n"
                     "17: <;,53>\n"
                     "18: <i,81>\n"
                     "19: <!=,38>\n"
                     "20: <10,80>\n"
                     "21: <;,53>\n"
                     "22: <++,66>\n"
                     "23: <i,81>\n"
                     "24: <),45>\n"
                     "25: <{,59>\n"
                     "26: <printf,81>\n"
                     "27: <(,44>\n"
                     "28: <\",78>\n"
                     "29: <%d,81>\n"
                     "30: <\",78>\n"
                     "31: <,,48>\n"
                     "32: <i,81>\n"
                     "33: <),45>\n"
                     "34: <;,53>\n"
                     "35: <},63>\n"
                     "36: <return,20>\n"
                     "37: <0,80>\n"
                     "38: <;,53>\n"
                     "39: <},63>";
    run_test("Test Case 2 (Loop & Comments)", input2, expect2);

    // Test Case 3: 紧凑代码与块注释
    string input3 = R"(int main(){char c = "h";/* 注释 12313test */if (c=="h")printf("%c",c);else print("k");return 0;})";
    string expect3 = "1: <int,17>\n"
                     "2: <main,81>\n"
                     "3: <(,44>\n"
                     "4: <),45>\n"
                     "5: <{,59>\n"
                     "6: <char,4>\n"
                     "7: <c,81>\n"
                     "8: <=,72>\n"
                     "9: <\",78>\n"
                     "10: <h,81>\n"
                     "11: <\",78>\n"
                     "12: <;,53>\n"
                     "13: </* 注释 12313test */,79>\n"
                     "14: <if,16>\n"
                     "15: <(,44>\n"
                     "16: <c,81>\n"
                     "17: <==,73>\n"
                     "18: <\",78>\n"
                     "19: <h,81>\n"
                     "20: <\",78>\n"
                     "21: <),45>\n"
                     "22: <printf,81>\n"
                     "23: <(,44>\n"
                     "24: <\",78>\n"
                     "25: <%c,81>\n"
                     "26: <\",78>\n"
                     "27: <,,48>\n"
                     "28: <c,81>\n"
                     "29: <),45>\n"
                     "30: <;,53>\n"
                     "31: <else,10>\n"
                     "32: <print,81>\n"
                     "33: <(,44>\n"
                     "34: <\",78>\n"
                     "35: <k,81>\n"
                     "36: <\",78>\n"
                     "37: <),45>\n"
                     "38: <;,53>\n"
                     "39: <return,20>\n"
                     "40: <0,80>\n"
                     "41: <;,53>\n"
                     "42: <},63>";
    run_test("Test Case 3 (Compact & Block Comment)", input3, expect3);

    // Test Case 4: C++ 关键字与符号
    string input4 = R"(bool gofor(char& ch, string& pos, const string& prog)
{
    ++pos;
    if (pos >= prog.size())
    {
        return false;
    }
    else
    {
        ch = prog[pos];
        return true;
    }
})";
    string expect4 = "1: <bool,81>\n"
                     "2: <gofor,81>\n"
                     "3: <(,44>\n"
                     "4: <char,4>\n"
                     "5: <&,41>\n"
                     "6: <ch,81>\n"
                     "7: <,,48>\n"
                     "8: <string,81>\n"
                     "9: <&,41>\n"
                     "10: <pos,81>\n"
                     "11: <,,48>\n"
                     "12: <const,5>\n"
                     "13: <string,81>\n"
                     "14: <&,41>\n"
                     "15: <prog,81>\n"
                     "16: <),45>\n"
                     "17: <{,59>\n"
                     "18: <++,66>\n"
                     "19: <pos,81>\n"
                     "20: <;,53>\n"
                     "21: <if,16>\n"
                     "22: <(,44>\n"
                     "23: <pos,81>\n"
                     "24: <>=,75>\n"
                     "25: <prog,81>\n"
                     "26: <.,49>\n"
                     "27: <size,81>\n"
                     "28: <(,44>\n"
                     "29: <),45>\n"
                     "30: <),45>\n"
                     "31: <{,59>\n"
                     "32: <return,20>\n"
                     "33: <false,81>\n"
                     "34: <;,53>\n"
                     "35: <},63>\n"
                     "36: <else,10>\n"
                     "37: <{,59>\n"
                     "38: <ch,81>\n"
                     "39: <=,72>\n"
                     "40: <prog,81>\n"
                     "41: <[,55>\n"
                     "42: <pos,81>\n"
                     "43: <],56>\n"
                     "44: <;,53>\n"
                     "45: <return,20>\n"
                     "46: <true,81>\n"
                     "47: <;,53>\n"
                     "48: <},63>\n"
                     "49: <},63>";
    run_test("Test Case 4 (C++ Syntax check)", input4, expect4);

    // Test Case 5: 复杂运算符贪婪匹配 (+=, >>=, --)
    string input5 = R"(void testOp() {
    a += b;
    c--;
    d >>= 2;
})";
    string expect5 = "1: <void,30>\n"
                     "2: <testOp,81>\n"
                     "3: <(,44>\n"
                     "4: <),45>\n"
                     "5: <{,59>\n"
                     "6: <a,81>\n"
                     "7: <+=,67>\n"
                     "8: <b,81>\n"
                     "9: <;,53>\n"
                     "10: <c,81>\n"
                     "11: <--,34>\n"
                     "12: <;,53>\n"
                     "13: <d,81>\n"
                     "14: <>>=,77>\n"
                     "15: <2,80>\n"
                     "16: <;,53>\n"
                     "17: <},63>";
    run_test("Test Case 5 (Greedy Operators)", input5, expect5);

    // Test Case 6: 浮点数与科学计数法
    string input6 = R"(float f = 1.23;
double d = 1e-10;
double e = 1.5E+2;)";
    string expect6 = "1: <float,13>\n"
                     "2: <f,81>\n"
                     "3: <=,72>\n"
                     "4: <1.23,80>\n"
                     "5: <;,53>\n"
                     "6: <double,9>\n"
                     "7: <d,81>\n"
                     "8: <=,72>\n"
                     "9: <1e-10,80>\n"
                     "10: <;,53>\n"
                     "11: <double,9>\n"
                     "12: <e,81>\n"
                     "13: <=,72>\n"
                     "14: <1.5E+2,80>\n"
                     "15: <;,53>";
    run_test("Test Case 6 (Scientific Numbers)", input6, expect6);

    // Test Case 7: 空字符串与转义字符
    string input7 = R"(char* s = "";
char c = '\n';
char b = '\\';)";
    string expect7 = "1: <char,4>\n"
                     "2: <*,46>\n"
                     "3: <s,81>\n"
                     "4: <=,72>\n"
                     "5: <\",78>\n"
                     "6: <\",78>\n"
                     "7: <;,53>\n"
                     "8: <char,4>\n"
                     "9: <c,81>\n"
                     "10: <=,72>\n"
                     "11: <',78>\n"
                     "12: <\\n,80>\n"
                     "13: <',78>\n"
                     "14: <;,53>\n"
                     "15: <char,4>\n"
                     "16: <b,81>\n"
                     "17: <=,72>\n"
                     "18: <',78>\n"
                     "19: <\\\\,80>\n"
                     "20: <',78>\n"
                     "21: <;,53>";
    run_test("Test Case 7 (String Escapes)", input7, expect7);

    // Test Case 8: 紧凑无空格代码 (Stress Test)
    string input8 = "x=a+b*c;";
    string expect8 = "1: <x,81>\n"
                     "2: <=,72>\n"
                     "3: <a,81>\n"
                     "4: <+,65>\n"
                     "5: <b,81>\n"
                     "6: <*,46>\n"
                     "7: <c,81>\n"
                     "8: <;,53>";
    run_test("Test Case 8 (No Whitespace)", input8, expect8);

    // Test Case 9: 预处理指令与下划线标识符
    string input9 = R"(#include <stdio.h>
int _val = 1;)";
    string expect9 = "1: <#include,81>\n"
                     "2: <<,68>\n"
                     "3: <stdio,81>\n"
                     "4: <.,49>\n"
                     "5: <h,81>\n"
                     "6: <>,74>\n"
                     "7: <int,17>\n"
                     "8: <_val,81>\n"
                     "9: <=,72>\n"
                     "10: <1,80>\n"
                     "11: <;,53>";
    run_test("Test Case 9 (Preprocessor & ID)", input9, expect9);

    // Test Case 10: 综合逻辑（指针、数组、结构体）
    string input10 = R"(struct Node {
    int data[10];
    struct Node* next;
};)";
    string expect10 = "1: <struct,25>\n"
                      "2: <Node,81>\n"
                      "3: <{,59>\n"
                      "4: <int,17>\n"
                      "5: <data,81>\n"
                      "6: <[,55>\n"
                      "7: <10,80>\n"
                      "8: <],56>\n"
                      "9: <;,53>\n"
                      "10: <struct,25>\n"
                      "11: <Node,81>\n"
                      "12: <*,46>\n"
                      "13: <next,81>\n"
                      "14: <;,53>\n"
                      "15: <},63>\n"
                      "16: <;,53>";
    run_test("Test Case 10 (Struct & Arrays)", input10, expect10);

    return 0;
}
