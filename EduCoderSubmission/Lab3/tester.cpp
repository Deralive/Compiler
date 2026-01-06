#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#define TEST_MODE
#include "LRParser.h"

using namespace std;

void run_test(const string &testName, const string &input, const string &expectedOutput) {
    cout << "Running " << testName << "..." << endl;

    streambuf *oldCin = cin.rdbuf();
    streambuf *oldCout = cout.rdbuf();

    stringstream inputSS(input);
    stringstream outputSS;

    cin.rdbuf(inputSS.rdbuf());
    cout.rdbuf(outputSS.rdbuf());

    try {
        Analysis();
    } catch (...) {
        cout << "Exception occurred!" << endl;
    }

    cin.rdbuf(oldCin);
    cout.rdbuf(oldCout);

    string actual = outputSS.str();

    while (!actual.empty() && (actual.back() == '\n' || actual.back() == '\r' || actual.back() == ' '))
        actual.pop_back();

    string expectedClean = expectedOutput;
    while (!expectedClean.empty() && (expectedClean.back() == '\n' || expectedClean.back() == '\r' || expectedClean.back() == ' '))
        expectedClean.pop_back();

    if (expectedClean.empty()) {
        cout << "[NEW CASE GENERATED]" << endl;
        cout << "Please copy the following output into your expect string:" << endl;
        cout << "R\"(" << actual << ")\"" << endl;
    } else if (actual == expectedClean) {
        cout << "[PASS]" << endl;
    } else {
        cout << "[FAIL]" << endl;
        cout << "--- Expected ---" << endl << expectedClean << endl;
        cout << "--- Actual ---" << endl << actual << endl;
    }
    cout << "------------------------------------------------" << endl;
}

int main() {

    string input1 = R"({
  ID = NUM ;
})";
    string expect1 = R"(program => 
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
{ ID = NUM ; } )";
    run_test("Test Case 1 (Simple Assignment)", input1, expect1);

    string input2 = R"({
ID = ID + NUM ;
})";
    string expect2 = R"(program => 
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
{ ID = ID + NUM ; } )";
    run_test("Test Case 2 (Arithmetic)", input2, expect2);

    string input3 = R"({
while ( ID == NUM ) 
{ 
ID = NUM 
}
})";
    string expect3 = R"(语法错误，第4行，缺少";"
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
{ while ( ID == NUM ) { ID = NUM ; } } )";
    run_test("Test Case 3 (While & Error)", input3, expect3);

    string input4 = R"({
if ( ID == ID )
then
ID = NUM ;
else
ID = ID * NUM ;
})";
    string expect4 = R"(program => 
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
{ if ( ID == ID ) then ID = NUM ; else ID = ID * NUM ; } )";
    run_test("Test Case 4 (If-Else)", input4, expect4);

    string input5 = R"({
})";

    string expect5 = R"(program => 
compoundstmt => 
{ stmts } => 
{ } )";
    run_test("Test Case 5 (Empty Block)", input5, expect5);

    string input6 = R"({
ID = NUM ;
ID = ID ;
})";
    string expect6 = R"(program => 
compoundstmt => 
{ stmts } => 
{ stmt stmts } => 
{ stmt stmt stmts } => 
{ stmt stmt } => 
{ stmt assgstmt } => 
{ stmt ID = arithexpr ; } => 
{ stmt ID = multexpr arithexprprime ; } => 
{ stmt ID = multexpr ; } => 
{ stmt ID = simpleexpr multexprprime ; } => 
{ stmt ID = simpleexpr ; } => 
{ stmt ID = ID ; } => 
{ assgstmt ID = ID ; } => 
{ ID = arithexpr ; ID = ID ; } => 
{ ID = multexpr arithexprprime ; ID = ID ; } => 
{ ID = multexpr ; ID = ID ; } => 
{ ID = simpleexpr multexprprime ; ID = ID ; } => 
{ ID = simpleexpr ; ID = ID ; } => 
{ ID = NUM ; ID = ID ; } )";
    run_test("Test Case 6 (Sequence)", input6, expect6);

    string input7 = R"({
{
  ID = NUM ;
}
})";
    string expect7 = R"(program => 
compoundstmt => 
{ stmts } => 
{ stmt stmts } => 
{ stmt } => 
{ compoundstmt } => 
{ { stmts } } => 
{ { stmt stmts } } => 
{ { stmt } } => 
{ { assgstmt } } => 
{ { ID = arithexpr ; } } => 
{ { ID = multexpr arithexprprime ; } } => 
{ { ID = multexpr ; } } => 
{ { ID = simpleexpr multexprprime ; } } => 
{ { ID = simpleexpr ; } } => 
{ { ID = NUM ; } } )";
    run_test("Test Case 7 (Nested Blocks)", input7, expect7);

    string input8 = R"({
ID = ID + ID * NUM ;
})";
    string expect8 = R"(program => 
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
{ ID = multexpr + simpleexpr * simpleexpr multexprprime ; } => 
{ ID = multexpr + simpleexpr * simpleexpr ; } => 
{ ID = multexpr + simpleexpr * NUM ; } => 
{ ID = multexpr + ID * NUM ; } => 
{ ID = simpleexpr multexprprime + ID * NUM ; } => 
{ ID = simpleexpr + ID * NUM ; } => 
{ ID = ID + ID * NUM ; } )";
    run_test("Test Case 8 (Precedence)", input8, expect8);

    string input9 = R"({
ID = ( ID + NUM ) * NUM ;
})";
    string expect9 = R"(program => 
compoundstmt => 
{ stmts } => 
{ stmt stmts } => 
{ stmt } => 
{ assgstmt } => 
{ ID = arithexpr ; } => 
{ ID = multexpr arithexprprime ; } => 
{ ID = multexpr ; } => 
{ ID = simpleexpr multexprprime ; } => 
{ ID = simpleexpr * simpleexpr multexprprime ; } => 
{ ID = simpleexpr * simpleexpr ; } => 
{ ID = simpleexpr * NUM ; } => 
{ ID = ( arithexpr ) * NUM ; } => 
{ ID = ( multexpr arithexprprime ) * NUM ; } => 
{ ID = ( multexpr + multexpr arithexprprime ) * NUM ; } => 
{ ID = ( multexpr + multexpr ) * NUM ; } => 
{ ID = ( multexpr + simpleexpr multexprprime ) * NUM ; } => 
{ ID = ( multexpr + simpleexpr ) * NUM ; } => 
{ ID = ( multexpr + NUM ) * NUM ; } => 
{ ID = ( simpleexpr multexprprime + NUM ) * NUM ; } => 
{ ID = ( simpleexpr + NUM ) * NUM ; } => 
{ ID = ( ID + NUM ) * NUM ; } )";
    run_test("Test Case 9 (Parentheses)", input9, expect9);

    string input10 = R"({
if ( ID >= NUM ) then
  while ( ID < NUM ) ID = NUM ;
else
  ID = NUM ;
})";
    string expect10 = R"(program => 
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
{ if ( boolexpr ) then stmt else ID = simpleexpr ; } => 
{ if ( boolexpr ) then stmt else ID = NUM ; } => 
{ if ( boolexpr ) then whilestmt else ID = NUM ; } => 
{ if ( boolexpr ) then while ( boolexpr ) stmt else ID = NUM ; } => 
{ if ( boolexpr ) then while ( boolexpr ) assgstmt else ID = NUM ; } => 
{ if ( boolexpr ) then while ( boolexpr ) ID = arithexpr ; else ID = NUM ; } => 
{ if ( boolexpr ) then while ( boolexpr ) ID = multexpr arithexprprime ; else ID = NUM ; } => 
{ if ( boolexpr ) then while ( boolexpr ) ID = multexpr ; else ID = NUM ; } => 
{ if ( boolexpr ) then while ( boolexpr ) ID = simpleexpr multexprprime ; else ID = NUM ; } => 
{ if ( boolexpr ) then while ( boolexpr ) ID = simpleexpr ; else ID = NUM ; } => 
{ if ( boolexpr ) then while ( boolexpr ) ID = NUM ; else ID = NUM ; } => 
{ if ( boolexpr ) then while ( arithexpr boolop arithexpr ) ID = NUM ; else ID = NUM ; } => 
{ if ( boolexpr ) then while ( arithexpr boolop multexpr arithexprprime ) ID = NUM ; else ID = NUM ; } => 
{ if ( boolexpr ) then while ( arithexpr boolop multexpr ) ID = NUM ; else ID = NUM ; } => 
{ if ( boolexpr ) then while ( arithexpr boolop simpleexpr multexprprime ) ID = NUM ; else ID = NUM ; } => 
{ if ( boolexpr ) then while ( arithexpr boolop simpleexpr ) ID = NUM ; else ID = NUM ; } => 
{ if ( boolexpr ) then while ( arithexpr boolop NUM ) ID = NUM ; else ID = NUM ; } => 
{ if ( boolexpr ) then while ( arithexpr < NUM ) ID = NUM ; else ID = NUM ; } => 
{ if ( boolexpr ) then while ( multexpr arithexprprime < NUM ) ID = NUM ; else ID = NUM ; } => 
{ if ( boolexpr ) then while ( multexpr < NUM ) ID = NUM ; else ID = NUM ; } => 
{ if ( boolexpr ) then while ( simpleexpr multexprprime < NUM ) ID = NUM ; else ID = NUM ; } => 
{ if ( boolexpr ) then while ( simpleexpr < NUM ) ID = NUM ; else ID = NUM ; } => 
{ if ( boolexpr ) then while ( ID < NUM ) ID = NUM ; else ID = NUM ; } => 
{ if ( arithexpr boolop arithexpr ) then while ( ID < NUM ) ID = NUM ; else ID = NUM ; } => 
{ if ( arithexpr boolop multexpr arithexprprime ) then while ( ID < NUM ) ID = NUM ; else ID = NUM ; } => 
{ if ( arithexpr boolop multexpr ) then while ( ID < NUM ) ID = NUM ; else ID = NUM ; } => 
{ if ( arithexpr boolop simpleexpr multexprprime ) then while ( ID < NUM ) ID = NUM ; else ID = NUM ; } => 
{ if ( arithexpr boolop simpleexpr ) then while ( ID < NUM ) ID = NUM ; else ID = NUM ; } => 
{ if ( arithexpr boolop NUM ) then while ( ID < NUM ) ID = NUM ; else ID = NUM ; } => 
{ if ( arithexpr >= NUM ) then while ( ID < NUM ) ID = NUM ; else ID = NUM ; } => 
{ if ( multexpr arithexprprime >= NUM ) then while ( ID < NUM ) ID = NUM ; else ID = NUM ; } => 
{ if ( multexpr >= NUM ) then while ( ID < NUM ) ID = NUM ; else ID = NUM ; } => 
{ if ( simpleexpr multexprprime >= NUM ) then while ( ID < NUM ) ID = NUM ; else ID = NUM ; } => 
{ if ( simpleexpr >= NUM ) then while ( ID < NUM ) ID = NUM ; else ID = NUM ; } => 
{ if ( ID >= NUM ) then while ( ID < NUM ) ID = NUM ; else ID = NUM ; } )";
    run_test("Test Case 10 (Nested Control)", input10, expect10);

    return 0;
}
