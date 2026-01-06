#ifndef REGEX_GRAPH_H
#define REGEX_GRAPH_H

#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <stack>
#include <string>
#include <vector>


using namespace std;

namespace Lab1Visual {

    // NFA 状态节点
    struct State {
        int id;
        bool isEnd;
        // 转移：字符 -> 目标状态ID列表
        // 使用 -1 或特定字符表示 Epsilon 边
        multimap<char, int> transitions;

        State(int _id) : id(_id), isEnd(false) {
        }
    };

    // NFA 片段
    struct NFAFragment {
        int startState;
        int endState;
    };

    class RegexEngine {
    private:
        int stateCounter;
        vector<State> states;

        int newState() {
            states.push_back(State(stateCounter));
            return stateCounter++;
        }

        void addTransition(int from, int to, char symbol) {
            states[from].transitions.insert({symbol, to});
        }

        // 核心：将中缀正则转为后缀 (Shunting-yard 简化版)
        // 支持: a (字符), | (或), * (闭包), . (连接, 显式处理)
        string toPostfix(string regex) {
            string postfix = "";
            stack<char> opStack;

            // 预处理：插入连接符 '.'
            // 例如 ab -> a.b, a*b -> a*.b
            string formatted = "";
            for (size_t i = 0; i < regex.length(); i++) {
                char c1 = regex[i];
                if (i + 1 < regex.length()) {
                    char c2 = regex[i + 1];
                    formatted += c1;
                    if (c1 != '(' && c1 != '|' && c2 != ')' && c2 != '*' && c2 != '|' && c2 != '+') {
                        formatted += '.';
                    }
                } else {
                    formatted += c1;
                }
            }

            // 转后缀
            for (char c : formatted) {
                if (isalnum(c)) {
                    postfix += c;
                } else if (c == '(') {
                    opStack.push(c);
                } else if (c == ')') {
                    while (!opStack.empty() && opStack.top() != '(') {
                        postfix += opStack.top();
                        opStack.pop();
                    }
                    if (!opStack.empty())
                        opStack.pop();
                } else {
                    while (!opStack.empty() && precedence(opStack.top()) >= precedence(c)) {
                        postfix += opStack.top();
                        opStack.pop();
                    }
                    opStack.push(c);
                }
            }
            while (!opStack.empty()) {
                postfix += opStack.top();
                opStack.pop();
            }
            return postfix;
        }

        int precedence(char c) {
            if (c == '*')
                return 3;
            if (c == '.')
                return 2;
            if (c == '|')
                return 1;
            return 0;
        }

    public:
        RegexEngine() : stateCounter(0) {
        }

        // Thompson 构造法
        string buildGraph(string regex) {
            states.clear();
            stateCounter = 0;
            string postfix = toPostfix(regex);
            stack<NFAFragment> stack;

            for (char c : postfix) {
                if (isalnum(c)) { // 字符 a, b, c...
                    int start = newState();
                    int end = newState();
                    addTransition(start, end, c);
                    stack.push({start, end});
                } else if (c == '.') { // 连接 AB
                    if (stack.size() < 2)
                        continue; // 错误保护
                    NFAFragment n2 = stack.top();
                    stack.pop();
                    NFAFragment n1 = stack.top();
                    stack.pop();
                    // n1.end -> n2.start (Epsilon)
                    addTransition(n1.endState, n2.startState, '#'); // # 代表 Epsilon
                    stack.push({n1.startState, n2.endState});
                } else if (c == '|') { // 并联 A|B
                    if (stack.size() < 2)
                        continue;
                    NFAFragment n2 = stack.top();
                    stack.pop();
                    NFAFragment n1 = stack.top();
                    stack.pop();
                    int start = newState();
                    int end = newState();
                    // start -> n1.start, start -> n2.start
                    addTransition(start, n1.startState, '#');
                    addTransition(start, n2.startState, '#');
                    // n1.end -> end, n2.end -> end
                    addTransition(n1.endState, end, '#');
                    addTransition(n2.endState, end, '#');
                    stack.push({start, end});
                } else if (c == '*') { // 闭包 A*
                    if (stack.empty())
                        continue;
                    NFAFragment n = stack.top();
                    stack.pop();
                    int start = newState();
                    int end = newState();
                    // start -> n.start
                    addTransition(start, n.startState, '#');
                    // n.end -> start (loop back)
                    addTransition(n.endState, n.startState, '#'); // Back
                    // n.end -> end
                    addTransition(n.endState, end, '#');
                    // start -> end (zero times)
                    addTransition(start, end, '#');
                    stack.push({start, end});
                }
            }

            if (stack.empty())
                return "digraph G { label=\"Invalid Regex\"; }";

            NFAFragment finalNFA = stack.top();
            states[finalNFA.endState].isEnd = true;

            // 生成 DOT
            stringstream ss;
            ss << "digraph G {" << endl;
            ss << "  rankdir=LR;" << endl; // 从左到右布局
            ss << "  node [fontname=\"Helvetica\", shape=circle, style=filled, fillcolor=\"#ffffff\", color=\"#34495e\"];" << endl;
            ss << "  edge [fontname=\"Helvetica\", color=\"#34495e\"];" << endl;

            // 标记起点和终点
            ss << "  " << finalNFA.startState << " [label=\"Start\", fillcolor=\"#d1ecf1\", shape=circle];" << endl;
            ss << "  " << finalNFA.endState << " [label=\"End\", fillcolor=\"#d4edda\", shape=doublecircle];" << endl;

            for (const auto &s : states) {
                for (auto const &[key, val] : s.transitions) {
                    string label = "";
                    if (key == '#')
                        label = "ε";
                    else
                        label += key;
                    ss << "  " << s.id << " -> " << val << " [label=\"" << label << "\"];" << endl;
                }
            }
            ss << "}" << endl;
            return ss.str();
        }
    };
} // namespace Lab1Visual

#endif
