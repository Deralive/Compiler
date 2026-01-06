/**
 * 一、终结符与非终结符定义
 * ----------------------------------------------------------------------------------------------
 * 非终结符: program, stmt, compoundstmt, stmts, ifstmt, whilestmt, assgstmt,
 * boolexpr, boolop, arithexpr, arithexprprime (A'), multexpr,
 * multexprprime (M'), simpleexpr
 *
 * 终结符:   {, }, if, while, then, else, ID, NUM, =, ;, (, ),
 * +, -, *, /, <, >, <=, >=, ==, $ (EOF), E (Epsilon)
 *
 * 二、FIRST 集与 FOLLOW 集计算
 * ----------------------------------------------------------------------------------------------
 * | 非终结符        | FIRST 集               | FOLLOW 集                            |
 * | -------------- | --------------------- | ------------------------------------ |
 * | program        | {                     | $                                    |
 * | stmt           | if, while, ID, {      | if, while, ID, {, }, else            |
 * | compoundstmt   | {                     | $, if, while, ID, {, }, else         |
 * | stmts          | if, while, ID, {, E   | }                                    |
 * | ifstmt         | if                    | if, while, ID, {, }, else            |
 * | whilestmt      | while                 | if, while, ID, {, }, else            |
 * | assgstmt       | ID                    | if, while, ID, {, }, else            |
 * | boolexpr       | ID, NUM, (            | )                                    |
 * | boolop         | <, >, <=, >=, ==      | ID, NUM, (                           |
 * | arithexpr      | ID, NUM, (            | ;, ), <, >, <=, >=, ==               |
 * | A' (Arith')    | +, -, E               | ;, ), <, >, <=, >=, ==               |
 * | multexpr       | ID, NUM, (            | +, -, ;, ), <, >, <=, >=, ==         |
 * | M' (Mult')     | *, /, E               | +, -, ;, ), <, >, <=, >=, ==         |
 * | simpleexpr     | ID, NUM, (            | *, /, +, -, ;, ), <, >, <=, >=, ==   |
 *
 * 三、产生式编号与 SELECT 集
 * ---------------------------------------------------------------------------------------
 * | 编号  | 产生式                                         | SELECT 集                    |
 * | ---- | --------------------------------------------- | -----------------------------|
 * | (1)  | program -> compoundstmt                       | {                            |
 * | (2)  | stmt -> ifstmt                                | if                           |
 * | (3)  | stmt -> whilestmt                             | while                        |
 * | (4)  | stmt -> assgstmt                              | ID                           |
 * | (5)  | stmt -> compoundstmt                          | {                            |
 * | (6)  | compoundstmt -> { stmts }                     | {                            |
 * | (7)  | stmts -> stmt stmts                           | if, while, ID, {             |
 * | (8)  | stmts -> E                                    | }                            |
 * | (9)  | ifstmt -> if ( boolexpr ) then stmt else stmt | if                           |
 * | (10) | whilestmt -> while ( boolexpr ) stmt          | while                        |
 * | (11) | assgstmt -> ID = arithexpr ;                  | ID                           |
 * | (12) | boolexpr -> arithexpr boolop arithexpr        | ID, NUM, (                   |
 * | (13) | boolop -> <                                   | <                            |
 * | (14) | boolop -> >                                   | >                            |
 * | (15) | boolop -> <=                                  | <=                           |
 * | (16) | boolop -> >=                                  | >=                           |
 * | (17) | boolop -> ==                                  | ==                           |
 * | (18) | arithexpr -> multexpr A'                      | ID, NUM, (                   |
 * | (19) | A' -> + multexpr A'                           | +                            |
 * | (20) | A' -> - multexpr A'                           | -                            |
 * | (21) | A' -> E                                       | ;, ), <, >, <=, >=, ==       |
 * | (22) | multexpr -> simpleexpr M'                     | ID, NUM, (                   |
 * | (23) | M' -> * simpleexpr M'                         | *                            |
 * | (24) | M' -> / simpleexpr M'                         | /                            |
 * | (25) | M' -> E                                       | +, -, ;, ), <, >, <=, >=, == |
 * | (26) | simpleexpr -> ID                              | ID                           |
 * | (27) | simpleexpr -> NUM                             | NUM                          |
 * | (28) | simpleexpr -> ( arithexpr )                   | (                            |
 *
 * 四、LL(1) 预测分析表
 *
 * [Part 1] 基础控制流与赋值符号
 * | Non-Term |  {  |  }  | if  | while | ID  | NUM |  (  |  )  |  ;  |  =  | then | else |
 * | :------- | :-: | :-: | :-: | :---: | :-: | :-: | :-: | :-: | :-: | :-: | :--: | :--: |
 * | program  | (1) |     |     |       |     |     |     |     |     |     |      |      |
 * | compound | (6) |     |     |       |     |     |     |     |     |     |      |      |
 * | stmts    | (7) | (8) | (7) |  (7)  | (7) |     |     |     |     |     |      |      |
 * | stmt     | (5) |     | (2) |  (3)  | (4) |     |     |     |     |     |      |      |
 * | ifstmt   |     |     | (9) |       |     |     |     |     |     |     |      |      |
 * | whilestmt|     |     |     |  (10) |     |     |     |     |     |     |      |      |
 * | assgstmt |     |     |     |       | (11)|     |     |     |     |     |      |      |
 * | boolexpr |     |     |     |       | (12)| (12)| (12)|     |     |     |      |      |
 * | boolop   |     |     |     |       |     |     |     |     |     |     |      |      |
 * | arithexpr|     |     |     |       | (18)| (18)| (18)|     |     |     |      |      |
 * | A'       |     |     |     |       |     |     |     | (21)| (21)|     |      |      |
 * | multexpr |     |     |     |       | (22)| (22)| (22)|     |     |     |      |      |
 * | M'       |     |     |     |       |     |     |     | (25)| (25)|     |      |      |
 * | simple   |     |     |     |       | (26)| (27)| (28)|     |     |     |      |      |
 *
 * [Part 2] 运算符与比较符
 * | Non-Term |  +  |  -  |  * |  /  |  <  |  >  | <=  | >=  | ==  |
 * | :------- | :-: | :-: | :-: | :-: | :-: | :-: | :-: | :-: | :-: |
 * | program  |     |     |     |     |     |     |     |     |     |
 * | compound |     |     |     |     |     |     |     |     |     |
 * | stmts    |     |     |     |     |     |     |     |     |     |
 * | stmt     |     |     |     |     |     |     |     |     |     |
 * | ifstmt   |     |     |     |     |     |     |     |     |     |
 * | whilestmt|     |     |     |     |     |     |     |     |     |
 * | assgstmt |     |     |     |     |     |     |     |     |     |
 * | boolexpr |     |     |     |     |     |     |     |     |     |
 * | boolop   |     |     |     |     | (13)| (14)| (15)| (16)| (17)|
 * | arithexpr|     |     |     |     |     |     |     |     |     |
 * | A'       | (19)| (20)|     |     | (21)| (21)| (21)| (21)| (21)|
 * | multexpr |     |     |     |     |     |     |     |     |     |
 * | M'       | (25)| (25)| (23)| (24)| (25)| (25)| (25)| (25)| (25)|
 * | simple   |     |     |     |     |     |     |     |     |     |
 *
 */

#include <algorithm>
#include <cstdio>
#include <iomanip>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

const int WIDTH_STACK = 50;
const int WIDTH_INPUT = 30;
const int WIDTH_ACTION = 35;

namespace Lab2 {
    class LL1Parser {
    public:
        enum Mode {
            MODE_TABLE, // 表格模式：实时打印推导过程，用于调试。
            MODE_TREE // 树模式：解析结束后打印错误日志和缩进格式的语法树。
        };

    private:
        struct LexUnit {
            string val;
            int lineNum;
        };

        struct StackNode {
            string name; // 文法符号名称
            int depth; // 在语法树中的深度
        };

        vector<LexUnit> _tokenStream;
        map<string, map<string, vector<string>>> _predictTable;
        set<string> _terminals;

        vector<string> _treeBuffer;
        vector<string> _errorBuffer;

        Mode _currentMode;
        size_t _lookahead;
        vector<StackNode> _stack;

        // 为了防止输出过长，仅显示栈顶的 3 个元素。
        string getStackString() const {
            string s = "";
            size_t start = 0;
            if (_stack.size() > 3) {
                s += ". ";
                start = _stack.size() - 3;
            }
            for (size_t i = start; i < _stack.size(); ++i) {
                s += _stack[i].name + " ";
            }
            return s;
        }

        string getInputString() const {
            string s = "";
            for (size_t i = _lookahead; i < _tokenStream.size(); ++i) {
                s += _tokenStream[i].val;
            }
            return s;
        }

        string getTokenCategory(const string &val) const {
            if (_terminals.count(val) && val != "ID" && val != "NUM") {
                return val;
            }
            if (val == "ID")
                return "ID";
            if (val == "NUM")
                return "NUM";
            if (isdigit(val[0]))
                return "NUM";
            if (isalpha(val[0]) || val[0] == '_')
                return "ID";
            return val;
        }

        int getCurrentLine() const {
            if (_lookahead >= _tokenStream.size())
                return -1;
            return _tokenStream[_lookahead].lineNum;
        }

        int getPrevLine() const {
            if (_lookahead > 0)
                return _tokenStream[_lookahead - 1].lineNum;
            return getCurrentLine();
        }

        // 日志与记录函数
        void logTableAction(const string &action) {
            if (_currentMode == MODE_TABLE) {
                cout << left << setw(WIDTH_STACK) << getStackString() << setw(WIDTH_INPUT) << getInputString() << setw(WIDTH_ACTION) << action
                     << endl;
            }
        }

        void logTreeNode(const string &name, int depth) {
            if (_currentMode == MODE_TREE) {
                string line = "";
                for (int i = 0; i < depth; ++i)
                    line += "\t";
                line += name;
                _treeBuffer.push_back(line);
            }
        }

        void logError(const string &msg, int lineNum, bool isTableError) {
            if (_currentMode == MODE_TABLE && isTableError) {
                logTableAction("Error: " + msg);
            }
            if (_currentMode == MODE_TREE && !isTableError) {
                stringstream ss;
                ss << "语法错误,第" << lineNum << "行," << msg;
                _errorBuffer.push_back(ss.str());
            }
        }

        // 处理栈顶为文件结束符的情况。
        bool handleEndMarker(const string &curVal) {
            if (curVal == "$") {
                logTableAction("Accept");
                return false;
            } else {
                logTableAction("Stack empty");
                logError("分析结束但输入未耗尽", getPrevLine(), false);
                return false;
            }
        }

        // 处理栈顶为终结符的情况。
        void handleTerminal(const StackNode &top, const string &curVal, const string &curCat) {
            if (top.name == curCat) {
                logTableAction("Match " + curVal);
                _lookahead++;
            } else {
                // 匹配失败，假装已插入缺失符号，不消耗输入
                logTableAction("Missing " + top.name + " (Inserted)");
                logError("缺少\"" + top.name + "\"", getPrevLine(), false);
            }
        }

        /**
         * @brief 处理栈顶为非终结符的情况。
         * @details 包含查表展开、自动空串推导和恐慌模式错误恢复逻辑。
         */
        bool handleNonTerminal(const StackNode &top, const string &curVal, const string &curCat) {
            string X = top.name;

            // 查预测分析表，正常展开
            if (_predictTable[X].count(curCat)) {
                const vector<string> &rhs = _predictTable[X][curCat];

                string production = X + " ->";
                for (const auto &s : rhs)
                    production += " " + s;
                logTableAction(production);

                // 倒序入栈
                for (auto it = rhs.rbegin(); it != rhs.rend(); ++it) {
                    if (*it != "E") {
                        _stack.push_back({*it, top.depth + 1});
                    } else {
                        // 空产生式不入栈，但需要在语法树中记录节点
                        logTreeNode("E", top.depth + 1);
                    }
                }
                return true;
            }

            // 如果当前非终结符可以推导为空，则认为它已经结束，不报错
            if (canDeriveEpsilon(X)) {
                logTableAction(X + " -> E");
                logTreeNode("E", top.depth + 1);
                // 不入栈也不消耗输入，相当于 X 自动匹配为空并消失
                return true;
            }

            // 如果遇到了块结束符，说明当前的非终结符 X 缺失了，放弃 X 以便外层恢复
            if (curVal == "}" || curVal == "$" || curVal == ";") {
                logTableAction("Discard " + X);
                logError("在需要 \"" + X + "\" 的地方遇到了 \"" + curVal + "\" (已忽略该结构)", getCurrentLine(), false);
                return true;
            }

            // 恐慌模式：遇到意外字符
            // 真正的垃圾输入，跳过当前 Token，保留 X 重试
            logTableAction("Unexpected " + curVal);
            logError("未预期的输入 \"" + curVal + "\"", getCurrentLine(), false);

            if (curVal == "$")
                return false;

            _lookahead++;
            _stack.push_back(top);
            return true;
        }

        // 向预测分析表添加规则
        void addRule(const string &lhs, const vector<string> &inputs, const vector<string> &rhs) {
            for (const auto &in : inputs) {
                _predictTable[lhs][in] = rhs;
            }
        }

        // 检查非终结符是否存在推导为空的产生式
        bool canDeriveEpsilon(const string &nonTerminal) {
            if (_predictTable.find(nonTerminal) == _predictTable.end())
                return false;
            for (const auto &pair : _predictTable[nonTerminal]) {
                const vector<string> &rhs = pair.second;
                if (rhs.size() == 1 && rhs[0] == "E")
                    return true;
            }
            return false;
        }

        // 初始化文法规则
        void buildGrammar() {
            _terminals = {"{", "}", "if", "then", "else", "while", "ID", "NUM", "=",  ";", "(", ")",
                          "+", "-", "*",  "/",    "<",    ">",     "<=", ">=",  "==", "E", "$"};

            addRule("program", {"{"}, {"compoundstmt"});
            addRule("compoundstmt", {"{"}, {"{", "stmts", "}"});
            addRule("stmts", {"if", "while", "ID", "{"}, {"stmt", "stmts"});
            addRule("stmts", {"}"}, {"E"});
            addRule("stmt", {"if"}, {"ifstmt"});
            addRule("stmt", {"while"}, {"whilestmt"});
            addRule("stmt", {"ID"}, {"assgstmt"});
            addRule("stmt", {"{"}, {"compoundstmt"});
            addRule("ifstmt", {"if"}, {"if", "(", "boolexpr", ")", "then", "stmt", "else", "stmt"});
            addRule("whilestmt", {"while"}, {"while", "(", "boolexpr", ")", "stmt"});
            addRule("assgstmt", {"ID"}, {"ID", "=", "arithexpr", ";"});
            addRule("boolexpr", {"ID", "NUM", "("}, {"arithexpr", "boolop", "arithexpr"});
            addRule("boolop", {"<"}, {"<"});
            addRule("boolop", {">"}, {">"});
            addRule("boolop", {"<="}, {"<="});
            addRule("boolop", {">="}, {">="});
            addRule("boolop", {"=="}, {"=="});
            addRule("arithexpr", {"ID", "NUM", "("}, {"multexpr", "arithexprprime"});
            addRule("arithexprprime", {"+"}, {"+", "multexpr", "arithexprprime"});
            addRule("arithexprprime", {"-"}, {"-", "multexpr", "arithexprprime"});
            addRule("arithexprprime", {")", ";", "<", ">", "<=", ">=", "=="}, {"E"});
            addRule("multexpr", {"ID", "NUM", "("}, {"simpleexpr", "multexprprime"});
            addRule("multexprprime", {"*"}, {"*", "simpleexpr", "multexprprime"});
            addRule("multexprprime", {"/"}, {"/", "simpleexpr", "multexprprime"});
            addRule("multexprprime", {"+", "-", ")", ";", "<", ">", "<=", ">=", "=="}, {"E"});
            addRule("simpleexpr", {"ID"}, {"ID"});
            addRule("simpleexpr", {"NUM"}, {"NUM"});
            addRule("simpleexpr", {"("}, {"(", "arithexpr", ")"});
        }

    public:
        LL1Parser() {
            buildGrammar();
        }

        /**
         * @brief 词法分析函数。
         * @details 读取源代码，切分 Token 并记录行号。会自动跳过空行。
         */
        void scan(const string &sourceCode) {
            _tokenStream.clear();
            stringstream ss(sourceCode);
            string lineStr;
            int currentLine = 1;
            while (getline(ss, lineStr)) {
                stringstream lineStream(lineStr);
                string word;
                bool hasTokens = false;
                while (lineStream >> word) {
                    _tokenStream.push_back({word, currentLine});
                    hasTokens = true;
                }
                // 仅当行内有有效 Token 时增加行号，跳过空行
                if (hasTokens)
                    currentLine++;
            }
            _tokenStream.push_back({"$", currentLine});
        }

        /**
         * @brief 语法分析主函数。
         * @param mode 输出模式。
         */
        void parse(Mode mode) {
            // 初始化状态
            _currentMode = mode;
            _lookahead = 0;
            _stack.clear();
            _treeBuffer.clear();
            _errorBuffer.clear();

            _stack.push_back({"$", 0});
            _stack.push_back({"program", 0});

            if (mode == MODE_TABLE) {
                cout << left << setw(WIDTH_STACK) << "Stack" << setw(WIDTH_INPUT) << "Input" << setw(WIDTH_ACTION) << "Action" << endl;
                cout << string(WIDTH_STACK + WIDTH_INPUT + WIDTH_ACTION, '-') << endl;
            }

            // 主解析循环
            bool continueParsing = true;
            while (continueParsing && !_stack.empty()) {
                if (_lookahead >= _tokenStream.size())
                    break;

                StackNode top = _stack.back();
                string X = top.name;
                int depth = top.depth;

                string curVal = _tokenStream[_lookahead].val;
                string curCategory = getTokenCategory(curVal);

                if (X == "$") {
                    continueParsing = handleEndMarker(curVal);
                } else {
                    _stack.pop_back();

                    // 树模式下记录节点
                    if (_currentMode == MODE_TREE && X != "E") {
                        logTreeNode(X, depth);
                    }

                    if (X == "E") {
                        // 空产生式，不做处理
                    } else if (_terminals.count(X)) {
                        handleTerminal(top, curVal, curCategory);
                    } else {
                        continueParsing = handleNonTerminal(top, curVal, curCategory);
                    }
                }
            }

            if (mode == MODE_TREE) {
                for (const auto &err : _errorBuffer)
                    cout << err << endl;
                for (const auto &line : _treeBuffer)
                    cout << line << endl;
            }
        }
    };

    void read_prog(string &prog) {
        char c;
        while (scanf("%c", &c) != EOF) {
            prog += c;
        }
    }

    void Analysis() {
        string prog;
        read_prog(prog);
        LL1Parser parser;
        parser.scan(prog);
        parser.parse(LL1Parser::MODE_TREE);
    }
} // namespace Lab2
