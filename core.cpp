#include <cstdio>
#include <emscripten/bind.h>
#include <fstream>
#include <iostream>
#include <string>

#include "LL1Parser.h"
#include "LRParser.h"
#include "LexAnalysis.h"
#include "RegexGraph.h"
#include "TranslationSchema.h"

using namespace emscripten;
using namespace std;

std::string run_lab_adapter(int lab_id, std::string input_content) {
    std::ofstream infile("input.txt");
    infile << input_content;
    infile.close();
    freopen("input.txt", "r", stdin);
    freopen("output.txt", "w", stdout);

    try {
        switch (lab_id) {
            case 1: {
                Lab1::Analysis();
                break;
            }
            case 2: {
                Lab2::Analysis();
                break;
            }
            case 3: {
                Lab3::Analysis();
                break;
            }
            case 4: {
                Lab4::Analysis();
                break;
            }
            default:
                std::cout << "未知实验 ID: " << lab_id << std::endl;
        }
    } catch (...) {
        std::cout << "运行时发生异常！" << std::endl;
    }

    fflush(stdout);
    std::cout.flush();
    std::ifstream outfile("output.txt");
    std::string result((std::istreambuf_iterator<char>(outfile)), std::istreambuf_iterator<char>());
    outfile.close();
    return result;
}

std::string generate_grammar_table_adapter(std::string input_grammar) {
    try {
        Lab3::Grammar g;
        g.resetAndLoad(input_grammar);
        return g.toJSON();
    } catch (...) {
        return "{\"error\": \"C++ 内部错误\"}";
    }
}

std::string generate_regex_graph_adapter(std::string regex) {
    try {
        Lab1Visual::RegexEngine engine;
        return engine.buildGraph(regex);
    } catch (...) {
        return "digraph G { label=\"Error parsing regex\"; }";
    }
}

std::string generate_sets_adapter(std::string input_grammar) {
    try {
        Lab3::Grammar g;
        g.resetAndLoad(input_grammar);
        return g.getSetsJSON();
    } catch (...) {
        return "{\"error\": \"Calculation Failed\"}";
    }
}

EMSCRIPTEN_BINDINGS(my_module) {
    emscripten::function("processInput", &run_lab_adapter);
    emscripten::function("generateGrammarTable", &generate_grammar_table_adapter);
    emscripten::function("generateRegexGraph", &generate_regex_graph_adapter);
    emscripten::function("generateSets", &generate_sets_adapter);
}
