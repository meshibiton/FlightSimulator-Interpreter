//
// Created by meshi on 24/12/2019.
//
#include <iostream>
#include <bits/stdc++.h>
#include <vector>
#include<unordered_map>
#include "Interpreter.h"

using namespace std;
namespace Global_Functions {
    double interpreter(string expression);

    string earseChar(string string1, string chars);

    void parser(vector<string> vectorLexer);

    vector<string> lineVector(string line, char char1);

    vector<string> lexer();

    void printVector(vector<string> tokens);

    string TrimLeft(const std::string &str);

    string TrimRight(string &str);
    void creatMapCommend();

    extern unordered_map<string, Command *> command_table;
    extern unordered_map<string, Var *> symbolTable;
}