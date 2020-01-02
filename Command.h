//
// Created by meshi on 19/12/2019.
//

#ifndef FINALPROJECT_COMMAND_H
#define FINALPROJECT_COMMAND_H

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

using namespace std;


/// this is Base class-command, its an inteface- abstract class.
class Command {
public:
    ///this method will be implement in his children.
    virtual int execute(vector<string>) = 0;

    virtual ~Command() {}
};

//// Derived classes
class OpenSeverCommand : public Command {
private:
    int numPort;
    int numParm;
public:
    int execute(vector<string>) override;

//    void openDataServer();
    void setNumPort(int numPort);

    void setNumParm(int numParm);
    ~OpenSeverCommand() override ;
};

class ConnectCommand : public Command {
private:
    int numPort;
    string ip;
    int numParm;
public:
    int execute(vector<string>) override;

    void connectControlClient(string ip, int port);


    virtual ~ConnectCommand() override ;
};

class PrintCommand : public Command {
private:
    string printValue;
    int numParm;
public:
    //print the value
    int execute(vector<string>) override;


    virtual ~PrintCommand() override ;
};

class SleepCommand : public Command {
private:
    int mili;
    int numparm;
public:
    //sleeping
    int execute(vector<string>) override;


    virtual ~SleepCommand() override ;
};

class DefineVarCommand : public Command {
private:
    int numParm;
public:


    int execute(vector<string>) override;

    void BuildList(vector<string> v);

    virtual ~DefineVarCommand() override ;
};

class Var : public DefineVarCommand {
private:
    string nameVar;
    double value;
    //the string like in xml file.
    string sim;
    //arrow left or right
    string side;
public:


    int execute(vector<string>) override;

    //constructor
    Var(string nameVar1, string side1, string sim1);

    Var(string nameVar1, double value);

    void setValue(double value);

    double getValue() const;

    const string &getSim() const;

    const string &getSide() const;

    virtual ~Var() override ;
};

class ConditionParser : public Command {
    ///Composite Pattern-holds list of commands
private:
public:


    virtual int execute(vector<string>);

    bool checkIfTrue(string);

    virtual ~ConditionParser() override ;

    string condition;
};

///child of conditionParser
class IfCommand : public ConditionParser {
private:
    bool flagCondition = false;
public:


    int execute(vector<string>) override;

    virtual ~IfCommand() override ;

};

///child of conditionParser
class LoopCommand : public ConditionParser {
private:
    bool flagCondition = false;
public:


    int execute(vector<string>) override;

    virtual ~LoopCommand() override ;
};

//extern unordered_map<string, Var *> symbolTable;
//extern unordered_map<string, Command *> command_table;

#endif //FINALPROJECT_COMMAND_H