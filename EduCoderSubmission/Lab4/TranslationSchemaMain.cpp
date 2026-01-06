// C语言词法分析器
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

void read_prog(string &prog) {
    char c;
    while (scanf("%c", &c) != EOF) {
        prog += c;
    }
}

void Analysis() {
    string prog;
    read_prog(prog);
}
