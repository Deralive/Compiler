#ifndef TRANSLATIONSCHEMA_H
#define TRANSLATIONSCHEMA_H

#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <stack>
#include <string>
#include <vector>

using namespace std;

namespace Lab4 {
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

    inline string sanitize_input(const string &raw) {
        stringstream ss(raw);
        string line;
        string clean;
        while (getline(ss, line)) {
            bool empty = true;
            for (char c : line) {
                if (!isspace(c)) {
                    empty = false;
                    break;
                }
            }
            if (!empty) {
                clean += line + "\n";
            }
        }
        return clean;
    }

    enum VarType { T_INT, T_REAL, T_BOOL, T_VOID };

    struct Variant {
        VarType kind;
        int iVal;
        double rVal;
        bool bVal;

        Variant() : kind(T_VOID), iVal(0), rVal(0.0), bVal(false) {
        }
        Variant(VarType k, int i, double r, bool b) : kind(k), iVal(i), rVal(r), bVal(b) {
        }
    };

    struct VarData {
        VarType kind;
        int iVal;
        double rVal;
    };

    struct Token {
        string text;
        int line;
    };

    class CodeScanner {
    public:
        vector<Token> tokenize(const string &source) {
            vector<Token> tokens;
            int n = source.length();
            int i = 0;
            int currentLine = 1;

            while (i < n) {
                char c = source[i];

                if (isspace(c)) {
                    if (c == '\n')
                        currentLine++;
                    i++;
                } else if (isdigit(c)) {
                    readNumber(source, i, n, currentLine, tokens);
                } else if (isalpha(c)) {
                    readWord(source, i, n, currentLine, tokens);
                } else {
                    readOperator(source, i, n, currentLine, tokens);
                }
            }
            tokens.push_back({"$", currentLine});
            return tokens;
        }

    private:
        void readNumber(const string &src, int &pos, int len, int line, vector<Token> &out) {
            string res;
            while (pos < len && (isdigit(src[pos]) || src[pos] == '.')) {
                res += src[pos];
                pos++;
            }
            out.push_back({res, line});
        }

        void readWord(const string &src, int &pos, int len, int line, vector<Token> &out) {
            string res;
            while (pos < len && (isalnum(src[pos]) || src[pos] == '_')) {
                res += src[pos];
                pos++;
            }
            out.push_back({res, line});
        }

        void readOperator(const string &src, int &pos, int len, int line, vector<Token> &out) {
            string op;
            op += src[pos];
            if (pos + 1 < len) {
                char next = src[pos + 1];
                if ((src[pos] == '>' && next == '=') || (src[pos] == '<' && next == '=') || (src[pos] == '=' && next == '=') ||
                    (src[pos] == '!' && next == '=')) {
                    op += next;
                    pos++;
                }
            }
            out.push_back({op, line});
            pos++;
        }
    };

    class CoreParser {
    public:
        CoreParser() {
            errorFlag = false;
            flowControl.push(true);
            initGrammar();
        }

        void analyze(const vector<Token> &tokens) {
            size_t ptr = 0;
            vector<ParseStackItem> parseStack;
            parseStack.push_back({"$"});
            parseStack.push_back({"program"});

            while (!parseStack.empty()) {
                string X = parseStack.back().symbol;

                if (X.length() > 0 && X[0] == '#') {
                    parseStack.pop_back();
                    dispatchAction(X);
                    continue;
                }

                string curVal = tokens[ptr].text;
                string curType = categorize(curVal);

                ctxLine = tokens[ptr].line;

                if (terminals.count(X)) {
                    if (X == curType) {
                        ctxMatchedStr = curVal;
                        ctxLine = tokens[ptr].line;
                        parseStack.pop_back();
                        ptr++;
                    } else {
                        cout << "error: expected " << X << " but got " << curVal << endl;
                        return;
                    }
                }

                else {
                    if (grammarTable[X].count(curType)) {
                        parseStack.pop_back();
                        vector<string> rhs = grammarTable[X][curType];
                        if (rhs.size() == 1 && rhs[0] == "E")
                            continue;

                        for (auto it = rhs.rbegin(); it != rhs.rend(); ++it) {
                            parseStack.push_back({*it});
                        }
                    } else {
                        cout << "error: unexpected token " << curVal << " for " << X << endl;
                        return;
                    }
                }
            }

            if (!errorFlag) {
                for (const auto &name : outputOrder) {
                    VarData d = env[name];
                    cout << name << ": ";
                    if (d.kind == T_INT)
                        cout << d.iVal;
                    else
                        cout << d.rVal;
                    cout << endl;
                }
            }
        }

    private:
        struct ParseStackItem {
            string symbol;
        };
        map<string, map<string, vector<string>>> grammarTable;
        set<string> terminals;

        map<string, VarData> env;
        stack<Variant> operands;
        stack<bool> flowControl;
        vector<string> outputOrder;
        bool errorFlag;

        string ctxMatchedStr;
        string ctxLastId;
        string ctxLHS;
        string ctxOp;
        int ctxLine;

        void logError(const string &msg) {
            cout << "error message:line " << ctxLine << "," << msg << endl;
            errorFlag = true;
        }

        string categorize(const string &val) {
            if (terminals.count(val))
                return val;
            if (isdigit(val[0]) || (val.size() > 1 && val[0] == '-' && isdigit(val[1]))) {
                if (val.find('.') != string::npos)
                    return "REALNUM";
                return "INTNUM";
            }
            return "ID";
        }

        void dispatchAction(const string &act) {
            bool canExec = !flowControl.empty() && flowControl.top();
            if (!canExec) {
                if (act == "#ELSE_JUMP" || act == "#IF_END")
                    handleFlow(act);
                return;
            }
            if (act == "#ELSE_JUMP" || act == "#IF_END")
                handleFlow(act);
            else if (act.find("#DECL") == 0 || act == "#SAVE_ID")
                handleDecl(act);
            else if (act == "#ASSIGN" || act == "#SAVE_LHS")
                handleAssign(act);
            else if (act.find("#PUSH") == 0)
                handleStack(act);
            else if (act.find("#OP") == 0)
                handleCalc(act);
            else if (act.find("#IF") == 0 || act == "#DO_BOOL" || act == "#SAVE_OP")
                handleBool(act);
        }

        void handleDecl(const string &act) {
            if (act == "#SAVE_ID") {
                ctxLastId = ctxMatchedStr;
                return;
            }
            VarData data;
            if (act == "#DECL_INT") {
                data.kind = T_INT;
                if (ctxMatchedStr.find('.') != string::npos) {
                    logError("realnum can not be translated into int type");
                }
                data.iVal = stoi(ctxMatchedStr);
            } else {
                data.kind = T_REAL;
                data.rVal = stod(ctxMatchedStr);
            }
            env[ctxLastId] = data;
            outputOrder.push_back(ctxLastId);
        }

        void handleAssign(const string &act) {
            if (act == "#SAVE_LHS") {
                ctxLHS = ctxMatchedStr;
                return;
            }
            Variant v = operands.top();
            operands.pop();
            if (env.find(ctxLHS) == env.end()) {
                cout << "undefined variable " << ctxLHS << endl;
                errorFlag = true;
                return;
            }
            VarData &info = env[ctxLHS];
            if (info.kind == T_INT) {
                if (v.kind == T_REAL) {
                    logError("realnum can not be translated into int type");
                    info.iVal = (int)v.rVal;
                } else {
                    info.iVal = v.iVal;
                }
            } else {
                info.rVal = (v.kind == T_INT) ? (double)v.iVal : v.rVal;
            }
        }

        void handleStack(const string &act) {
            Variant v;
            if (act == "#PUSH_INT") {
                v.kind = T_INT;
                v.iVal = stoi(ctxMatchedStr);
            } else if (act == "#PUSH_REAL") {
                v.kind = T_REAL;
                v.rVal = stod(ctxMatchedStr);
            } else if (act == "#PUSH_ID_VAL") {
                if (env.find(ctxMatchedStr) == env.end()) {
                    v.kind = T_INT;
                    v.iVal = 0;
                } else {
                    VarData d = env[ctxMatchedStr];
                    if (d.kind == T_INT) {
                        v.kind = T_INT;
                        v.iVal = d.iVal;
                    } else {
                        v.kind = T_REAL;
                        v.rVal = d.rVal;
                    }
                }
            }
            operands.push(v);
        }

        void handleCalc(const string &act) {
            Variant r = operands.top();
            operands.pop();
            Variant l = operands.top();
            operands.pop();
            double dl = (l.kind == T_INT) ? l.iVal : l.rVal;
            double dr = (r.kind == T_INT) ? r.iVal : r.rVal;

            if (act == "#OP_DIV" && dr == 0.0) {
                logError("division by zero");
                operands.push(Variant(T_INT, 0, 0.0, false));
                return;
            }

            double res = 0;
            if (act == "#OP_ADD")
                res = dl + dr;
            if (act == "#OP_SUB")
                res = dl - dr;
            if (act == "#OP_MUL")
                res = dl * dr;
            if (act == "#OP_DIV")
                res = dl / dr;

            Variant v;
            if (l.kind == T_REAL || r.kind == T_REAL) {
                v.kind = T_REAL;
                v.rVal = res;
            } else {
                v.kind = T_INT;
                v.iVal = (int)res;
            }
            operands.push(v);
        }

        void handleBool(const string &act) {
            if (act == "#SAVE_OP") {
                ctxOp = ctxMatchedStr;
                return;
            }
            if (act == "#IF_CHECK") {
                Variant v = operands.top();
                operands.pop();
                bool parent = flowControl.top();
                flowControl.push(parent ? v.bVal : false);
                return;
            }
            Variant r = operands.top();
            operands.pop();
            Variant l = operands.top();
            operands.pop();
            double dl = (l.kind == T_INT) ? l.iVal : l.rVal;
            double dr = (r.kind == T_INT) ? r.iVal : r.rVal;
            Variant v;
            v.kind = T_BOOL;
            if (ctxOp == "<")
                v.bVal = dl < dr;
            else if (ctxOp == ">")
                v.bVal = dl > dr;
            else if (ctxOp == "<=")
                v.bVal = dl <= dr;
            else if (ctxOp == ">=")
                v.bVal = dl >= dr;
            else if (ctxOp == "==")
                v.bVal = dl == dr;
            else if (ctxOp == "!=")
                v.bVal = dl != dr;
            operands.push(v);
        }

        void handleFlow(const string &act) {
            if (act == "#ELSE_JUMP") {
                bool cur = flowControl.top();
                flowControl.pop();
                bool par = flowControl.empty() ? true : flowControl.top();
                flowControl.push(par ? !cur : false);
            } else if (act == "#IF_END") {
                flowControl.pop();
            }
        }

        void addProduction(const string &lhs, const vector<string> &keys, const vector<string> &rhs) {
            for (const auto &k : keys)
                grammarTable[lhs][k] = rhs;
        }

        void initGrammar() {
            terminals = {"int", "real", "if", "then", "else", "{",  "}",  "(",  ")",  ";",      "=",       "+", "-",
                         "*",   "/",    "<",  ">",    "<=",   ">=", "==", "!=", "ID", "INTNUM", "REALNUM", "$"};

            addProduction("program", {"int", "real", "{"}, {"decls", "compoundstmt"});
            addProduction("decls", {"int", "real"}, {"decl", ";", "decls"});
            addProduction("decls", {"{", "a"}, {"E"});
            addProduction("decl", {"int"}, {"int", "ID", "#SAVE_ID", "=", "const_val", "#DECL_INT"});
            addProduction("decl", {"real"}, {"real", "ID", "#SAVE_ID", "=", "REALNUM", "#DECL_REAL"});
            addProduction("const_val", {"INTNUM"}, {"INTNUM"});
            addProduction("const_val", {"REALNUM"}, {"REALNUM"});
            addProduction("compoundstmt", {"{"}, {"{", "stmts", "}"});
            addProduction("stmts", {"if", "ID", "{"}, {"stmt", "stmts"});
            addProduction("stmts", {"}"}, {"E"});
            addProduction("stmt", {"if"}, {"ifstmt"});
            addProduction("stmt", {"ID"}, {"assgstmt"});
            addProduction("stmt", {"{"}, {"compoundstmt"});
            addProduction("assgstmt", {"ID"}, {"ID", "#SAVE_LHS", "=", "arithexpr", "#ASSIGN", ";"});
            addProduction("ifstmt", {"if"}, {"if", "(", "boolexpr", ")", "#IF_CHECK", "then", "stmt", "#ELSE_JUMP", "else", "stmt", "#IF_END"});
            vector<string> exprStart = {"ID", "INTNUM", "REALNUM", "("};
            addProduction("boolexpr", exprStart, {"arithexpr", "boolop", "arithexpr", "#DO_BOOL"});
            addProduction("boolop", {"<"}, {"<", "#SAVE_OP"});
            addProduction("boolop", {">"}, {">", "#SAVE_OP"});
            addProduction("boolop", {"<="}, {"<=", "#SAVE_OP"});
            addProduction("boolop", {">="}, {">=", "#SAVE_OP"});
            addProduction("boolop", {"=="}, {"==", "#SAVE_OP"});
            addProduction("boolop", {"!="}, {"!=", "#SAVE_OP"});
            addProduction("arithexpr", exprStart, {"multexpr", "arithexprprime"});
            addProduction("arithexprprime", {"+"}, {"+", "multexpr", "#OP_ADD", "arithexprprime"});
            addProduction("arithexprprime", {"-"}, {"-", "multexpr", "#OP_SUB", "arithexprprime"});
            addProduction("arithexprprime", {";", ")", "<", ">", "<=", ">=", "==", "!="}, {"E"});
            addProduction("multexpr", exprStart, {"simpleexpr", "multexprprime"});
            addProduction("multexprprime", {"*"}, {"*", "simpleexpr", "#OP_MUL", "multexprprime"});
            addProduction("multexprprime", {"/"}, {"/", "simpleexpr", "#OP_DIV", "multexprprime"});
            addProduction("multexprprime", {"+", "-", ";", ")", "<", ">", "<=", ">=", "==", "!="}, {"E"});
            addProduction("simpleexpr", {"ID"}, {"ID", "#PUSH_ID_VAL"});
            addProduction("simpleexpr", {"INTNUM"}, {"INTNUM", "#PUSH_INT"});
            addProduction("simpleexpr", {"REALNUM"}, {"REALNUM", "#PUSH_REAL"});
            addProduction("simpleexpr", {"("}, {"(", "arithexpr", ")"});
        }
    };

    void Analysis() {
        string prog;
        read_prog(prog);

        string cleanCode = sanitize_input(prog);

        CodeScanner scanner;
        vector<Token> tokens = scanner.tokenize(cleanCode);

        CoreParser parser;
        parser.analyze(tokens);
    }

#endif
}
