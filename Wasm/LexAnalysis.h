#ifndef LEX_ANALYSIS_H
#define LEX_ANALYSIS_H

#include <cstdio>
#include <cstring>
#include <iostream>
#include <map>
#include <string>
#include <vector>

using namespace std;

namespace Lab1 {

    class Scanner {
    private:
        map<string, int> keyMap;
        int line;

        // 初始化关键字映射
        void initKeyMap() {
            keyMap.clear();
            keyMap["auto"] = 1;
            keyMap["break"] = 2;
            keyMap["case"] = 3;
            keyMap["char"] = 4;
            keyMap["const"] = 5;
            keyMap["continue"] = 6;
            keyMap["default"] = 7;
            keyMap["do"] = 8;
            keyMap["double"] = 9;
            keyMap["else"] = 10;
            keyMap["enum"] = 11;
            keyMap["extern"] = 12;
            keyMap["float"] = 13;
            keyMap["for"] = 14;
            keyMap["goto"] = 15;
            keyMap["if"] = 16;
            keyMap["int"] = 17;
            keyMap["long"] = 18;
            keyMap["register"] = 19;
            keyMap["return"] = 20;
            keyMap["short"] = 21;
            keyMap["signed"] = 22;
            keyMap["sizeof"] = 23;
            keyMap["static"] = 24;
            keyMap["struct"] = 25;
            keyMap["switch"] = 26;
            keyMap["typedef"] = 27;
            keyMap["union"] = 28;
            keyMap["unsigned"] = 29;
            keyMap["void"] = 30;
            keyMap["volatile"] = 31;
            keyMap["while"] = 32;

            keyMap["-"] = 33;
            keyMap["--"] = 34;
            keyMap["-="] = 35;
            keyMap["->"] = 36;
            keyMap["!"] = 37;
            keyMap["!="] = 38;
            keyMap["%"] = 39;
            keyMap["%="] = 40;
            keyMap["&"] = 41;
            keyMap["&&"] = 42;
            keyMap["&="] = 43;
            keyMap["("] = 44;
            keyMap[")"] = 45;
            keyMap["*"] = 46;
            keyMap["*="] = 47;
            keyMap[","] = 48;
            keyMap["."] = 49;
            keyMap["/"] = 50;
            keyMap["/="] = 51;
            keyMap[":"] = 52;
            keyMap[";"] = 53;
            keyMap["?"] = 54;
            keyMap["["] = 55;
            keyMap["]"] = 56;
            keyMap["^"] = 57;
            keyMap["^="] = 58;
            keyMap["{"] = 59;
            keyMap["|"] = 60;
            keyMap["||"] = 61;
            keyMap["|="] = 62;
            keyMap["}"] = 63;
            keyMap["~"] = 64;
            keyMap["+"] = 65;
            keyMap["++"] = 66;
            keyMap["+="] = 67;
            keyMap["<"] = 68;
            keyMap["<<"] = 69;
            keyMap["<<="] = 70;
            keyMap["<="] = 71;
            keyMap["="] = 72;
            keyMap["=="] = 73;
            keyMap[">"] = 74;
            keyMap[">="] = 75;
            keyMap[">>"] = 76;
            keyMap[">>="] = 77;
            keyMap["\""] = 78;
            keyMap["//"] = 79;
        }

        void read_prog(string &prog) {
            char c;
            while (scanf("%c", &c) != EOF) {
                prog += c;
            }
        }

        void output(string key, int id) {
            if (line > 1)
                cout << endl;
            cout << line++ << ": <" << key << "," << id << ">";
        }

    public:
        Scanner() {
            line = 1;
        }

        void Run() {
            line = 1;
            string prog = "";
            initKeyMap();
            read_prog(prog);

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
                            output("\"", keyMap.count("\"") ? keyMap["\""] : 78);
                            state = 3;
                            buffer = "";
                        } else if (ch == '\'') {
                            output("'", 78);
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
                                output(buffer, 79);
                                buffer = "";
                            } else if (nextChar == '*') {
                                buffer = "/*";
                                i += 2;
                                while (i < len) {
                                    buffer += prog[i];
                                    if (prog[i] == '/' && buffer.length() > 2 && buffer[buffer.length() - 2] == '*')
                                        break;
                                    i++;
                                }
                                output(buffer, 79);
                                buffer = "";
                            } else {
                                string tempStr(1, ch);
                                if (i + 1 < len && prog[i + 1] == '=') {
                                    tempStr += '=';
                                    i++;
                                }
                                output(tempStr, keyMap.count(tempStr) ? keyMap[tempStr] : 50);
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
                            output("\"", keyMap.count("\"") ? keyMap["\""] : 78);
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
                            output("'", 78);
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
    };

    void Analysis() {
        Scanner scanner;
        scanner.Run();
    }

} // namespace Lab1

#endif
