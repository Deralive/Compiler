#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#define TEST_MODE
#include "TranslationSchema.h"

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

    string input1 = R"(int a = 1 ; int b = 2 ; real c = 3.0 ;
{
a = a + 1 ;
b = b * a ;
if ( a < b ) then c = c / 2 ; else c = c / 4 ;
})";
    string expect1 = "a: 2\nb: 4\nc: 1.5\n";
    run_test("Test Case 1 (Basic)", input1, expect1);

    string input2 = R"(int a = 3 ; int b = 5.73 ; real c = 3.0 ;
{
a = a + 1 ;
b = b + a ;
if ( a < b ) then c = c / 0 ; else c = c / 4 ;
})";
    string expect2 = "error message:line 1,realnum can not be translated into int type\nerror message:line 5,division by zero\n";
    run_test("Test Case 2 (Errors)", input2, expect2);

    string input3 = R"(int a = 10 ; int b = 25 ; real c = 2.1 ;
{
a = a + 1 ;
c = c * b ;
b = b * a ;
if ( a == b ) then c = c / 3 ; else c = c / 5 ;
})";
    string expect3 = "a: 11\nb: 275\nc: 10.5\n";
    run_test("Test Case 3 (Logic ==)", input3, expect3);

    string input4 = R"(int a = 16 ; int b = 21 ; real c = 3.0 ;
{
a = a - 1 ;
b = b + a ;
if ( a <= b ) then a = b / 2 ; else c = c / 4 ;
a = a * 2 - 1 ;
if ( a >= b ) then c = c / 2 ; else c = c / 4 ;
})";
    string expect4 = "a: 35\nb: 36\nc: 0.75\n";
    run_test("Test Case 4 (Complex Control Flow)", input4, expect4);

    string input5 = R"(int a = 2 ; int b = 3 ; int c = 0 ; int d = 0 ;
{
c = a + b * 4 ;
d = 10 - 4 / a ;
})";
    string expect5 = "a: 2\nb: 3\nc: 14\nd: 8\n";
    run_test("Test Case 5 (Operator Precedence)", input5, expect5);

    string input6 = R"(int a = 10 ;
{
x = a + 1 ;
})";
    string expect6 = "undefined variable x\n";
    run_test("Test Case 6 (Undefined Variable)", input6, expect6);

    string input7 = R"(int i = 5 ; real r = 2.5 ;
{
r = r + i ;
})";
    string expect7 = "i: 5\nr: 7.5\n";
    run_test("Test Case 7 (Mixed Type Arithmetic)", input7, expect7);

    string input8 = R"(int a = 5 ; int b = 10 ;
{
if ( a != 5 ) then b = 0 ; else b = 20 ;
})";
    string expect8 = "a: 5\nb: 20\n";
    run_test("Test Case 8 (Logic !=)", input8, expect8);

    string input9 = R"(int a = 10 ; int b = 10 ;
{
if ( a >= b ) then a = 1 ; else a = 0 ;
})";
    string expect9 = "a: 1\nb: 10\n";
    run_test("Test Case 9 (Logic >=)", input9, expect9);

    string input10 = R"(int a = 1 ;
{
a = 3.5 ;
})";
    string expect10 = "error message:line 3,realnum can not be translated into int type\n";
    run_test("Test Case 10 (Assignment Type Error)", input10, expect10);

    return 0;
}
