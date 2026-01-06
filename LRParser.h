#ifndef LR_PARSER_H
#define LR_PARSER_H

#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <stack>
#include <string>
#include <vector>

using namespace std;

namespace Lab3 {
    // 内部使用的空串符号
    const string EPSILON = "@";

    enum TokType { TOK_EOF, TOK_ID, TOK_NUM, TOK_KW, TOK_SYM };

    struct Tok {
        TokType ty;
        string val;
        int ln;
    };

    struct Prod {
        int id;
        string lhs;
        vector<string> rhs;
    };

    enum ActType { ACT_S, ACT_R, ACT_ACC, ACT_ERR };

    struct Act {
        ActType ty;
        int val;
    };

    class Grammar {
    public:
        Grammar();

        // 核心亮点功能：重置并加载用户自定义文法
        void resetAndLoad(const string &input);

        // 核心亮点功能：导出表格数据给前端可视化
        string toJSON() const;
        string getSetsJSON() const {
            stringstream ss;
            ss << "{";

            // 1. 导出 FIRST 集
            ss << "\"first\": {";
            bool firstNT = true;
            for (const auto &kv : fst) {
                // 只输出非终结符的 First 集 (通常用户只关心这个，虽然算法也会算终结符的)
                if (!isNonTerm(kv.first))
                    continue;

                if (!firstNT)
                    ss << ",";
                ss << "\"" << kv.first << "\": [";
                bool firstItem = true;
                for (const auto &val : kv.second) {
                    if (!firstItem)
                        ss << ",";
                    ss << "\"" << val << "\"";
                    firstItem = false;
                }
                ss << "]";
                firstNT = false;
            }
            ss << "},";

            // 2. 导出 FOLLOW 集
            ss << "\"follow\": {";
            firstNT = true;
            for (const auto &kv : flw) {
                if (!firstNT)
                    ss << ",";
                ss << "\"" << kv.first << "\": [";
                bool firstItem = true;
                for (const auto &val : kv.second) {
                    if (!firstItem)
                        ss << ",";
                    ss << "\"" << val << "\"";
                    firstItem = false;
                }
                ss << "]";
                firstNT = false;
            }
            ss << "}";

            ss << "}";
            return ss.str();
        }

        void genTable();
        int cntProds() const;
        const Prod &getProd(int id) const;
        Act getAct(int st, const string &sym) const;
        int getGOTO(int st, const string &nt) const;

        bool isTerm(const string &s) const;
        bool isNonTerm(const string &s) const;

    private:
        vector<Prod> prods;
        set<string> terms;
        set<string> nterms;
        map<string, set<string>> fst;
        map<string, set<string>> flw;

        struct Itm {
            int pid;
            int dot;
            bool operator<(const Itm &o) const {
                return pid != o.pid ? pid < o.pid : dot < o.dot;
            }
            bool operator==(const Itm &o) const {
                return pid == o.pid && dot == o.dot;
            }
        };

        vector<set<Itm>> cc;

        map<pair<int, string>, Act> actTbl;
        map<pair<int, string>, int> gotoTbl;

        void loadRules();
        void calcFst();
        void calcFlw();
        bool updFst(const Prod &p);

        set<Itm> getCls(const set<Itm> &i);
        set<Itm> getGoto(const set<Itm> &i, const string &sym);
        void buildCC();
    };

    Grammar::Grammar() {
        loadRules();
    }

    // === 解析用户输入的文法字符串 ===
    void Grammar::resetAndLoad(const string &input) {
        prods.clear();
        terms.clear();
        nterms.clear();
        fst.clear();
        flw.clear();
        cc.clear();
        actTbl.clear();
        gotoTbl.clear();

        stringstream ss(input);
        string line;
        int pid = 0;

        while (getline(ss, line)) {
            if (line.empty())
                continue;
            size_t arrow = line.find("->");
            if (arrow == string::npos)
                continue;
            string lhs = line.substr(0, arrow);
            lhs.erase(0, lhs.find_first_not_of(" \t\r\n"));
            lhs.erase(lhs.find_last_not_of(" \t\r\n") + 1);
            nterms.insert(lhs);
        }

        if (!nterms.empty()) {
            ss.clear();
            ss.seekg(0);
            string firstLine, startSym;
            while (getline(ss, firstLine)) {
                if (firstLine.find("->") != string::npos) {
                    startSym = firstLine.substr(0, firstLine.find("->"));
                    startSym.erase(0, startSym.find_first_not_of(" \t\r\n"));
                    startSym.erase(startSym.find_last_not_of(" \t\r\n") + 1);
                    break;
                }
            }
            prods.push_back({0, "S'", {startSym}});
            nterms.insert("S'");
        }

        ss.clear();
        ss.seekg(0);
        while (getline(ss, line)) {
            if (line.empty())
                continue;
            size_t arrow = line.find("->");
            if (arrow == string::npos)
                continue;
            string lhs = line.substr(0, arrow);
            lhs.erase(0, lhs.find_first_not_of(" \t\r\n"));
            lhs.erase(lhs.find_last_not_of(" \t\r\n") + 1);
            string rhsStr = line.substr(arrow + 2);
            stringstream rss(rhsStr);
            string sym;
            vector<string> rhs;
            while (rss >> sym) {
                if (sym == "|")
                    continue;
                rhs.push_back(sym);
                if (!nterms.count(sym) && sym != EPSILON)
                    terms.insert(sym);
            }
            if (rhs.empty())
                rhs.push_back(EPSILON);
            prods.push_back({++pid, lhs, rhs});
        }
        terms.insert("$");
        genTable();
    }

    string Grammar::toJSON() const {
        stringstream ss;
        ss << "{ \"headers\": [";
        bool first = true;
        for (const auto &t : terms) {
            if (!first)
                ss << ",";
            ss << "\"" << t << "\"";
            first = false;
        }
        for (const auto &nt : nterms) {
            if (nt == "S'")
                continue;
            ss << ",\"" << nt << "\"";
        }
        ss << "], \"rows\": [";
        for (size_t i = 0; i < cc.size(); ++i) {
            if (i > 0)
                ss << ",";
            ss << "{ \"id\": " << i;
            for (const auto &t : terms) {
                if (actTbl.count({(int)i, t})) {
                    Act a = actTbl.at({(int)i, t});
                    ss << ", \"" << t << "\": \"";
                    if (a.ty == ACT_S)
                        ss << "s" << a.val;
                    else if (a.ty == ACT_R)
                        ss << "r" << a.val;
                    else if (a.ty == ACT_ACC)
                        ss << "acc";
                    ss << "\"";
                }
            }
            for (const auto &nt : nterms) {
                if (nt == "S'")
                    continue;
                if (gotoTbl.count({(int)i, nt})) {
                    ss << ", \"" << nt << "\": \"" << gotoTbl.at({(int)i, nt}) << "\"";
                }
            }
            ss << "}";
        }
        ss << "] }";
        return ss.str();
    }

    void Grammar::loadRules() {
        // 这是 Lab3 默认规则
        terms = {"{", "}", "if", "(", ")", "then", "else", "while", "=", ";", ">", "<", ">=", "<=", "==", "+", "-", "*", "/", "ID", "NUM", "$"};
        nterms = {"program",  "compoundstmt", "stmt",      "stmts",          "ifstmt",   "whilestmt",     "assgstmt",
                  "boolexpr", "boolop",       "arithexpr", "arithexprprime", "multexpr", "multexprprime", "simpleexpr"};
        auto add = [&](int i, string l, vector<string> r) { prods.push_back({i, l, r}); };
        add(0, "S'", {"program"});
        add(1, "program", {"compoundstmt"});
        add(2, "stmt", {"ifstmt"});
        add(3, "stmt", {"whilestmt"});
        add(4, "stmt", {"assgstmt"});
        add(5, "stmt", {"compoundstmt"});
        add(6, "compoundstmt", {"{", "stmts", "}"});
        add(7, "stmts", {"stmt", "stmts"});
        add(8, "stmts", {EPSILON});
        add(9, "ifstmt", {"if", "(", "boolexpr", ")", "then", "stmt", "else", "stmt"});
        add(10, "whilestmt", {"while", "(", "boolexpr", ")", "stmt"});
        add(11, "assgstmt", {"ID", "=", "arithexpr", ";"});
        add(12, "boolexpr", {"arithexpr", "boolop", "arithexpr"});
        add(13, "boolop", {"<"});
        add(14, "boolop", {">"});
        add(15, "boolop", {"<="});
        add(16, "boolop", {">="});
        add(17, "boolop", {"=="});
        add(18, "arithexpr", {"multexpr", "arithexprprime"});
        add(19, "arithexprprime", {"+", "multexpr", "arithexprprime"});
        add(20, "arithexprprime", {"-", "multexpr", "arithexprprime"});
        add(21, "arithexprprime", {EPSILON});
        add(22, "multexpr", {"simpleexpr", "multexprprime"});
        add(23, "multexprprime", {"*", "simpleexpr", "multexprprime"});
        add(24, "multexprprime", {"/", "simpleexpr", "multexprprime"});
        add(25, "multexprprime", {EPSILON});
        add(26, "simpleexpr", {"ID"});
        add(27, "simpleexpr", {"NUM"});
        add(28, "simpleexpr", {"(", "arithexpr", ")"});
    }

    bool Grammar::isTerm(const string &s) const {
        return terms.count(s);
    }
    bool Grammar::isNonTerm(const string &s) const {
        return nterms.count(s);
    }
    int Grammar::cntProds() const {
        return prods.size();
    }
    const Prod &Grammar::getProd(int id) const {
        return prods[id];
    }
    Act Grammar::getAct(int st, const string &sym) const {
        auto k = make_pair(st, sym);
        if (actTbl.count(k))
            return actTbl.at(k);
        return {ACT_ERR, 0};
    }
    int Grammar::getGOTO(int st, const string &nt) const {
        auto k = make_pair(st, nt);
        if (gotoTbl.count(k))
            return gotoTbl.at(k);
        return -1;
    }
    bool Grammar::updFst(const Prod &p) {
        if (p.id == 0)
            return false;
        bool chg = false;
        size_t k = 0;
        bool nxt = true;
        while (nxt && k < p.rhs.size()) {
            nxt = false;
            string s = p.rhs[k];
            if (s == EPSILON) {
                if (!fst[p.lhs].count(EPSILON)) {
                    fst[p.lhs].insert(EPSILON);
                    chg = true;
                }
                nxt = true;
            } else {
                for (const auto &f : fst[s]) {
                    if (f != EPSILON && !fst[p.lhs].count(f)) {
                        fst[p.lhs].insert(f);
                        chg = true;
                    }
                }
                if (fst[s].count(EPSILON))
                    nxt = true;
            }
            k++;
        }
        if (nxt && !fst[p.lhs].count(EPSILON)) {
            fst[p.lhs].insert(EPSILON);
            chg = true;
        }
        return chg;
    }
    void Grammar::calcFst() {
        for (auto &t : terms)
            fst[t] = {t};
        bool chg = true;
        while (chg) {
            chg = false;
            for (const auto &p : prods)
                chg |= updFst(p);
        }
    }
    void Grammar::calcFlw() {
        if (prods.size() > 0 && prods[0].rhs.size() > 0)
            flw[prods[0].rhs[0]].insert("$");
        else if (nterms.count("program"))
            flw["program"].insert("$");
        bool chg = true;
        while (chg) {
            chg = false;
            for (const auto &p : prods) {
                if (p.id == 0)
                    continue;
                for (size_t i = 0; i < p.rhs.size(); ++i) {
                    string B = p.rhs[i];
                    if (!isNonTerm(B))
                        continue;
                    size_t j = i + 1;
                    bool betaNull = true;
                    while (j < p.rhs.size()) {
                        string beta = p.rhs[j];
                        if (beta == EPSILON) {
                            j++;
                            continue;
                        }
                        bool currNull = false;
                        for (auto &f : fst[beta]) {
                            if (f != EPSILON) {
                                if (!flw[B].count(f)) {
                                    flw[B].insert(f);
                                    chg = true;
                                }
                            } else
                                currNull = true;
                        }
                        if (!currNull) {
                            betaNull = false;
                            break;
                        }
                        j++;
                    }
                    if (betaNull) {
                        for (auto &f : flw[p.lhs]) {
                            if (!flw[B].count(f)) {
                                flw[B].insert(f);
                                chg = true;
                            }
                        }
                    }
                }
            }
        }
    }
    set<Grammar::Itm> Grammar::getCls(const set<Itm> &items) {
        set<Itm> res = items;
        bool chg = true;
        while (chg) {
            chg = false;
            for (const auto &it : res) {
                if (it.dot < prods[it.pid].rhs.size()) {
                    string s = prods[it.pid].rhs[it.dot];
                    if (s == EPSILON)
                        continue;
                    if (isNonTerm(s)) {
                        for (const auto &p : prods) {
                            if (p.lhs == s) {
                                Itm ni = {p.id, 0};
                                if (!res.count(ni)) {
                                    res.insert(ni);
                                    chg = true;
                                }
                            }
                        }
                    }
                }
            }
        }
        return res;
    }
    set<Grammar::Itm> Grammar::getGoto(const set<Itm> &itms, const string &sym) {
        set<Itm> res;
        for (const auto &it : itms) {
            const auto &rhs = prods[it.pid].rhs;
            if (it.dot < rhs.size()) {
                if (rhs[it.dot] == sym && rhs[it.dot] != EPSILON) {
                    res.insert({it.pid, it.dot + 1});
                }
            }
        }
        return getCls(res);
    }
    void Grammar::buildCC() {
        cc.push_back(getCls({{0, 0}}));
        bool chg = true;
        while (chg) {
            chg = false;
            for (size_t i = 0; i < cc.size(); ++i) {
                set<string> seen;
                for (const auto &it : cc[i]) {
                    if (it.dot < prods[it.pid].rhs.size()) {
                        string s = prods[it.pid].rhs[it.dot];
                        if (s != EPSILON)
                            seen.insert(s);
                    }
                }
                for (const auto &s : seen) {
                    auto nxt = getGoto(cc[i], s);
                    if (nxt.empty())
                        continue;
                    int idx = -1;
                    for (size_t j = 0; j < cc.size(); ++j)
                        if (cc[j] == nxt)
                            idx = j;
                    if (idx == -1) {
                        cc.push_back(nxt);
                        gotoTbl[{(int)i, s}] = cc.size() - 1;
                        chg = true;
                    } else if (!gotoTbl.count({(int)i, s})) {
                        gotoTbl[{(int)i, s}] = idx;
                        chg = true;
                    }
                }
            }
        }
    }
    void Grammar::genTable() {
        calcFst();
        calcFlw();
        buildCC();
        for (size_t i = 0; i < cc.size(); ++i) {
            for (const auto &it : cc[i]) {
                const auto &p = prods[it.pid];
                bool isRed = (p.rhs.size() == 1 && p.rhs[0] == EPSILON) || (it.dot == p.rhs.size());
                if (isRed) {
                    if (p.id == 0)
                        actTbl[{(int)i, "$"}] = {ACT_ACC, 0};
                    else {
                        for (const auto &fs : flw[p.lhs])
                            actTbl[{(int)i, fs}] = {ACT_R, p.id};
                    }
                } else {
                    string s = p.rhs[it.dot];
                    if (isTerm(s) && gotoTbl.count({(int)i, s})) {
                        actTbl[{(int)i, s}] = {ACT_S, gotoTbl[{(int)i, s}]};
                    }
                }
            }
        }
    }

    class Lexer {
        istream &src;
        int ln;
        bool isSpc(char c) {
            return c == ' ' || c == '\t' || c == '\n' || c == '\r';
        }
        bool isAlpha(char c) {
            return isalpha(c);
        }
        bool isDig(char c) {
            return isdigit(c);
        }

    public:
        Lexer(istream &in) : src(in), ln(1) {
        }
        vector<Tok> run() {
            vector<Tok> ts;
            char c;
            while (src.get(c)) {
                if (isSpc(c)) {
                    if (c == '\n')
                        ln++;
                    continue;
                }
                string buf;
                buf += c;
                if (isAlpha(c)) {
                    while (src.peek() != EOF && (isAlpha(src.peek()) || isDig(src.peek()) || src.peek() == '_'))
                        buf += src.get();
                    static set<string> kws = {"if", "else", "then", "while"};
                    ts.push_back({kws.count(buf) ? TOK_KW : TOK_ID, buf, ln});
                } else if (isDig(c)) {
                    while (src.peek() != EOF && isDig(src.peek()))
                        buf += src.get();
                    ts.push_back({TOK_NUM, "NUM", ln});
                } else {
                    char next = src.peek();
                    if ((c == '<' || c == '>' || c == '=') && next == '=')
                        buf += src.get();
                    ts.push_back({TOK_SYM, buf, ln});
                }
            }
            ts.push_back({TOK_EOF, "$", ln});
            return ts;
        }
    };

    class Parser {
    public:
        Parser(Grammar &g);
        void run(vector<Tok> &ts);

    private:
        Grammar &G;
        vector<int> hist;
        void printDerive();
        void doShift(int nextSt, const string &sym, stack<int> &st, stack<string> &syms);
        void doReduce(int prodId, stack<int> &st, stack<string> &syms);
        void updateStr(vector<string> &s, const string &l, const vector<string> &r);
    };
    Parser::Parser(Grammar &g) : G(g) {
    }
    void Parser::doShift(int nextSt, const string &sym, stack<int> &st, stack<string> &syms) {
        st.push(nextSt);
        syms.push(sym);
    }
    void Parser::doReduce(int pid, stack<int> &st, stack<string> &syms) {
        const Prod &p = G.getProd(pid);
        hist.push_back(pid);
        int popN = p.rhs.size();
        if (popN == 1 && p.rhs[0] == EPSILON)
            popN = 0; // 关键修改：空串不退栈
        for (int i = 0; i < popN; ++i) {
            st.pop();
            syms.pop();
        }
        int nxt = G.getGOTO(st.top(), p.lhs);
        if (nxt != -1) {
            st.push(nxt);
            syms.push(p.lhs);
        }
    }
    void Parser::run(vector<Tok> &ts) {
        stack<int> st;
        stack<string> syms;
        st.push(0);
        size_t ip = 0;
        while (true) {
            int cur = st.top();
            Tok t = ts[ip];
            string s = t.val;
            Act a = G.getAct(cur, s);
            if (a.ty == ACT_S) {
                doShift(a.val, s, st, syms);
                ip++;
            } else if (a.ty == ACT_R) {
                doReduce(a.val, st, syms);
            } else if (a.ty == ACT_ACC) {
                printDerive();
                return;
            } else {
                if (G.getAct(cur, ";").ty != ACT_ERR) {
                    int errLine = t.ln;
                    if (ip > 0)
                        errLine = ts[ip - 1].ln;
                    cout << "语法错误，第" << errLine << "行，缺少\";\"" << endl;
                    ts.insert(ts.begin() + ip, {TOK_SYM, ";", errLine});
                } else {
                    cerr << "Syntax Error at line " << t.ln << ": unexpected token: " << s << endl;
                    return;
                }
            }
        }
    }
    void Parser::updateStr(vector<string> &str, const string &l, const vector<string> &r) {
        int idx = -1;
        for (int i = str.size() - 1; i >= 0; --i) {
            if (str[i] == l) {
                idx = i;
                break;
            }
        }
        if (idx != -1) {
            str.erase(str.begin() + idx);
            if (!(r.size() == 1 && r[0] == EPSILON)) {
                str.insert(str.begin() + idx, r.begin(), r.end());
            }
        }
    }
    void Parser::printDerive() {
        vector<string> cur = {"program"};
        vector<int> r_hist = hist;
        reverse(r_hist.begin(), r_hist.end());
        auto print = [&](bool arr) {
            for (size_t i = 0; i < cur.size(); ++i)
                cout << cur[i] << (i == cur.size() - 1 ? "" : " ");
            cout << (arr ? " => " : " ") << endl;
        };
        print(true);
        for (size_t i = 0; i < r_hist.size(); ++i) {
            const Prod &p = G.getProd(r_hist[i]);
            updateStr(cur, p.lhs, p.rhs);
            print(i != r_hist.size() - 1);
        }
    }
    void read_prog(string &prog) {
        char c;
        while (scanf("%c", &c) != EOF) {
            prog += c;
        }
    }
    void Analysis() {
        string prog;
        read_prog(prog);
        Grammar g;
        g.genTable();
        stringstream ss(prog);
        Lexer lex(ss);
        vector<Tok> ts = lex.run();
        Parser p(g);
        p.run(ts);
    }
} // namespace Lab3

#endif
