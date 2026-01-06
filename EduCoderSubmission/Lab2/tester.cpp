#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#define TEST_MODE
#include "LL1Parser.h"

using namespace std;

void run_test(const string &testName, const string &input, const string &expectedOutput) {
    cout << "Running " << testName << "..." << endl;

    // 备份流
    streambuf *oldCin = cin.rdbuf();
    streambuf *oldCout = cout.rdbuf();

    // 准备输入输出流
    stringstream inputSS(input);
    stringstream outputSS;

    cin.rdbuf(inputSS.rdbuf());
    cout.rdbuf(outputSS.rdbuf());

    // 执行分析
    try {
        string prog;
        read_prog(prog); // 读取输入

        LL1Parser parser;
        parser.scan(prog);
        // 【修复】传入参数 MODE_TREE
        parser.parse(LL1Parser::MODE_TREE);
    } catch (...) {
        cout << "Exception occurred!" << endl;
    }

    // 恢复流
    cin.rdbuf(oldCin);
    cout.rdbuf(oldCout);

    string actual = outputSS.str();

    // 清理尾部空白
    while (!actual.empty() && (actual.back() == '\n' || actual.back() == '\r'))
        actual.pop_back();
    string expectedClean = expectedOutput;
    while (!expectedClean.empty() && (expectedClean.back() == '\n' || expectedClean.back() == '\r'))
        expectedClean.pop_back();

    if (actual == expectedClean) {
        cout << "[PASS]" << endl;
    } else {
        cout << "[FAIL]" << endl;
        // 如果输出太长，可以只打印前几行，或者保存到文件查看
        cout << "--- Expected ---" << endl << expectedClean << endl;
        cout << "--- Actual ---" << endl << actual << endl;
    }
    cout << "------------------------------------------------" << endl;
}

int main() {

    string input1 = R"({
 ID = NUM ;
})";

    string expect1 = R"(program
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
		})";
    run_test("Test Case 1 (Simple Assignment)", input1, expect1);

    string input2 = R"({
ID = ID + NUM ;
})";
    string expect2 = R"(program
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
		})";
    run_test("Test Case 2 (Arithmetic)", input2, expect2);

    string input3 = R"({
while ( ID == NUM ) 
{ 
ID = NUM 
}
})";

    string expect3 = R"(语法错误,第4行,缺少";"
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
		})";
    run_test("Test Case 3 (While & Error)", input3, expect3);

    string input4 = R"({
if ( ID == ID )
then
ID = NUM ;
else
ID = ID * NUM ;
})";
    string expect4 = R"(program
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
		})";
    run_test("Test Case 4 (If-Then-Else)", input4, expect4);

    return 0;
}
