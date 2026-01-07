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

    struct State {
        int id;
        bool isEnd;
        multimap<char, int> transitions;
        State(int _id) : id(_id), isEnd(false) {
        }
    };

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

        string toPostfix(string regex) {
            string postfix = "";
            stack<char> opStack;

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

        string buildGraph(string regex) {
            states.clear();
            stateCounter = 0;
            string postfix = toPostfix(regex);
            stack<NFAFragment> stack;

            for (char c : postfix) {
                if (isalnum(c)) {
                    int start = newState();
                    int end = newState();
                    addTransition(start, end, c);
                    stack.push({start, end});
                } else if (c == '.') {
                    if (stack.size() < 2)
                        continue;
                    NFAFragment n2 = stack.top();
                    stack.pop();
                    NFAFragment n1 = stack.top();
                    stack.pop();

                    addTransition(n1.endState, n2.startState, '#');
                    stack.push({n1.startState, n2.endState});
                } else if (c == '|') {
                    if (stack.size() < 2)
                        continue;
                    NFAFragment n2 = stack.top();
                    stack.pop();
                    NFAFragment n1 = stack.top();
                    stack.pop();
                    int start = newState();
                    int end = newState();

                    addTransition(start, n1.startState, '#');
                    addTransition(start, n2.startState, '#');
                    addTransition(n1.endState, end, '#');
                    addTransition(n2.endState, end, '#');
                    stack.push({start, end});
                } else if (c == '*') {
                    if (stack.empty())
                        continue;
                    NFAFragment n = stack.top();
                    stack.pop();
                    int start = newState();
                    int end = newState();

                    addTransition(start, n.startState, '#');
                    addTransition(n.endState, n.startState, '#');
                    addTransition(n.endState, end, '#');
                    addTransition(start, end, '#');
                    stack.push({start, end});
                }
            }

            if (stack.empty())
                return "digraph G { label=\"Invalid Regex\"; }";

            NFAFragment finalNFA = stack.top();
            states[finalNFA.endState].isEnd = true;

            stringstream ss;
            ss << "digraph G {" << endl;
            ss << "  rankdir=LR;" << endl;
            ss << "  node [fontname=\"Helvetica\", shape=circle, style=filled, fillcolor=\"#ffffff\", color=\"#34495e\"];" << endl;
            ss << "  edge [fontname=\"Helvetica\", color=\"#34495e\"];" << endl;

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
