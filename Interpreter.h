//
// Created by meshi on 11/11/2019.
//

#ifndef Interpreter_Interpreter_H
#define Interpreter_Interpreter_H

#include <string>
#include <stack>
#include <queue>
#include <list>
#include <iterator>
#include <map>
#include "Expression.h"
#include <ctype.h>
#include <sstream>
#include <cstring>
#include <iostream>

using namespace std;


class Variable : public Expression {
private:
    string name;
    double value;
public:
    Variable &operator++();//++x
    Variable &operator--();//--x
    Variable &operator+=(double);

    Variable &operator-=(double);

    Variable &operator++(int);//x++
    Variable &operator--(int);//x--
    double calculate() override;

    Variable(string name, double value);

    ~Variable();
    //  double getValue() const;
};


class UnaryOperator : public Expression {
public:
    //  UnaryOperator(Expression *expression1);
    virtual ~UnaryOperator() {}

protected:
    Expression *expression;
};

class UPlus : public UnaryOperator {
public:
    double calculate() override;

    UPlus(Expression *expression);

    ~UPlus() override;
};

class UMinus : public UnaryOperator {
public:
    double calculate() override;

    UMinus(Expression *expression1);

    ~UMinus();

};

class BinaryOperator : public Expression {
public:
    virtual ~BinaryOperator() {}
    //  BinaryOperator(Expression *right,Expression *left);
protected:
    Expression *right;
    Expression *left;
};

class Plus : public BinaryOperator {
public:
    double calculate() override;

    Plus(Expression *left, Expression *right);

    ~Plus();
};

class Minus : public BinaryOperator {
public:
    double calculate() override;

    Minus(Expression *left, Expression *right);

    ~Minus();
};

class Mul : public BinaryOperator {
public:
    double calculate() override;

    Mul(Expression *left, Expression *right);

    ~Mul();
};

class Div : public BinaryOperator {
public:
    double calculate() override;

    Div(Expression *left, Expression *right);

    ~Div();
};

class relationalOperators : public BinaryOperator {
public:
    virtual double calculate() = 0;

    relationalOperators(Expression *left, Expression *right);

    ~relationalOperators();
};

class Bigger : public relationalOperators {
public:
    double calculate() override;

    using relationalOperators::relationalOperators;

    ~Bigger() = default;
};

class BiggerEqual : public relationalOperators {
public:
    double calculate() override;

    using relationalOperators::relationalOperators;

    ~BiggerEqual() = default;
};

class Smaller : public relationalOperators {
public:
    double calculate() override;

    using relationalOperators::relationalOperators;

    ~Smaller() = default;
};

class SmallerEqual : public relationalOperators {
public:
    double calculate() override;

    using relationalOperators::relationalOperators;

    ~SmallerEqual() = default;
};

class Equals : public relationalOperators {
public:
    double calculate() override;

    using relationalOperators::relationalOperators;

    ~Equals() = default;
};

class NotEquals : public relationalOperators {
public:
    double calculate() override;

    using relationalOperators::relationalOperators;

    ~NotEquals() = default;
};

class Value : public Expression {
public:
    double calculate() override;

    Value(double value) : value1(value) {};

    virtual ~Value();

private:
    const double value1;
};

class Interpreter {
private:
    stack<string> operators;
    queue<string> output;
    list<string> tokens;
    map<string, double> valuesOfVariables;
public:
    queue<string> polish(string expression);

    bool check(string c, stack<string>);

    bool checkIfOperator(string c);

    bool checkIfRelativeOperator(string c);

    bool checkIfNumber(string c);

    bool checkIfBracket(string c);

    bool checkIfDigit(char c);

    Expression *interpret(string expression);

    Expression *Initial(string var);

    void setVariables(string initialize);

    bool checkIfVariable(string c);

    bool checkIfTwice(list<string> tokens);
    // bool checkUnary()

};


#endif //Interpreter_Interpreter_H
