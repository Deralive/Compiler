
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

map<string, int> keyMap;
int line = 1;

void read_prog(string &prog) {
#ifdef TEST_MODE
    char c;
    while (cin.get(c)) {
        prog += c;
    }
#else
    char c;
    while (scanf("%c", &c) != EOF) {
        prog += c;
    }
#endif
}

void output(string key, int id) {
    if (line > 1)
        cout << endl;
    cout << line++ << ": <" << key << "," << id << ">";
}

void Analysis() {
    line = 1;
    string prog = "";
    read_prog(prog);

    ifstream infile("c_keys.txt");
    string k;
    int id;
    while (infile >> k >> id)
        keyMap[k] = id;
    infile.close();
    keyMap["//"] = 79;

    int state = 0;
    string buffer = "";
    int len = prog.length();

    for (int i = 0; i < len; ++i) {
        char ch = prog[i];

        switch (state) {
            case 0:
                if (isspace(ch))
                    continue;

                if (isdigit(ch)) {
                    state = 1;
                    buffer += ch;
                } else if (isalpha(ch) || ch == '_' || ch == '#') {
                    state = 2;
                    buffer += ch;
                } else if (ch == '"') {
                    output("\"", keyMap["\""]);
                    state = 3;
                    buffer = "";
                } else if (ch == '\'') {
                    output("'", keyMap.count("'") ? keyMap["'"] : 78);
                    state = 4;
                    buffer = "";
                } else if (ch == '/') {
                    char nextChar = (i + 1 < len) ? prog[i + 1] : '\0';
                    if (nextChar == '/') {
                        buffer = "//";
                        i += 2;

                        while (i < len && prog[i] != '\n') {
                            buffer += prog[i];
                            i++;
                        }
                        output(buffer, keyMap["//"]);
                        buffer = "";
                    } else if (nextChar == '*') {
                        buffer = "/*";
                        i += 2;
                        while (i < len) {
                            buffer += prog[i];

                            if (prog[i] == '/' && buffer.length() > 2 && buffer[buffer.length() - 2] == '*') {
                                break;
                            }
                            i++;
                        }
                        output(buffer, keyMap["//"]);
                        buffer = "";
                    } else {
                        output("/", keyMap["/"]);
                    }
                } else {
                    string three = "";
                    string two = "";
                    if (i + 2 < len)
                        three = three + ch + prog[i + 1] + prog[i + 2];
                    if (i + 1 < len)
                        two = two + ch + prog[i + 1];

                    if (three.length() == 3 && keyMap.count(three)) {
                        output(three, keyMap[three]);
                        i += 2;
                    } else if (two.length() == 2 && keyMap.count(two)) {
                        output(two, keyMap[two]);
                        i += 1;
                    } else {
                        string one(1, ch);
                        if (keyMap.count(one))
                            output(one, keyMap[one]);
                    }
                }
                break;

            case 1:

                if (isdigit(ch)) {
                    buffer += ch;
                } else if (ch == '.' || ch == 'e' || ch == 'E') {
                    buffer += ch;

                    if ((ch == 'e' || ch == 'E') && i + 1 < len) {
                        char next = prog[i + 1];
                        if (next == '+' || next == '-') {
                            buffer += next;
                            i++;
                        }
                    }
                } else {
                    output(buffer, 80);
                    buffer = "";
                    state = 0;
                    i--;
                }
                break;

            case 2:
                if (isalnum(ch) || ch == '_' || ch == '#')
                    buffer += ch;
                else {

                    output(buffer, keyMap.count(buffer) ? keyMap[buffer] : 81);
                    buffer = "";
                    state = 0;
                    i--;
                }
                break;

            case 3:
                if (ch == '\\') {
                    buffer += ch;
                    if (i + 1 < len) {
                        i++;
                        buffer += prog[i];
                    }
                } else if (ch == '"') {
                    if (!buffer.empty())
                        output(buffer, 81);
                    output("\"", keyMap["\""]);
                    buffer = "";
                    state = 0;
                } else {
                    buffer += ch;
                }
                break;

            case 4:
                if (ch == '\\') {
                    buffer += ch;
                    if (i + 1 < len) {
                        i++;
                        buffer += prog[i];
                    }
                } else if (ch == '\'') {
                    if (!buffer.empty())
                        output(buffer, 80);
                    output("'", keyMap.count("'") ? keyMap["'"] : 78);
                    buffer = "";
                    state = 0;
                } else {
                    buffer += ch;
                }
                break;
        }
    }

    if (!buffer.empty()) {
        if (state == 1)
            output(buffer, 80);
        else if (state == 2)
            output(buffer, keyMap.count(buffer) ? keyMap[buffer] : 81);
    }
}
