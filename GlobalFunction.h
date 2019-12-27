//
// Created by meshi on 24/12/2019.
//
#include <iostream>
#include <bits/stdc++.h>
#include <vector>
#include<unordered_map>
#include "Interpreter.h"
#include "Command.h"

using namespace std;
namespace Global_Functions {
    double interpreter(string expression);

    string earseChar(string string1, string chars);

    void parser(vector<string> vectorLexer,string flagCondition);

    vector<string> lineVector(string line, char char1);

    vector<string> lexer();

    void printVector(vector<string> tokens);

    string TrimLeft(const std::string &str);

    string TrimRight(string &str);
    void creatMapCommend();

    ////--------------server connect function-----------------
    void  serverSide( int client_socket);
    void openDataServer(int port);
    void  updateBufAndValue(char buffer []);
    string earseChar(string string1, string chars);
    void updateVariablesVul(string);
    void  creatMapSimToPairVar();
    ////--------------client connect function-----------------
    void  clientSide( int client_socket);
    void connectControlClient(string ip, int port);


    extern unordered_map<string, Command *> command_table;
    extern unordered_map<string, Var *> symbolTable;
    extern unordered_map<string, pair<Var* ,float>> mapSimToPairVar;
    extern bool  closeSocketServer;
    extern bool  isDoneCloseSocketClient;
//    extern std::mutex finishLock;
//    extern std::mutex lockSimulatorTable;
    extern queue<string> queueMessages;

}