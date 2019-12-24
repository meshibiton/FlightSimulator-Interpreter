//
// Created by meshi on 11/11/2019.
//

#include "Interpreter.h"
#include "Command.h"
#include "GlobalFunction.h"
using namespace Global_Functions;
//unary functions
double UPlus::calculate() {
    return this->expression->calculate();
}

UPlus::~UPlus() {
    if (this->expression != nullptr) {
        delete this->expression;
    }
    //   cout << "UPlus object deleted!!!" << endl;
}

UPlus::UPlus(Expression *expression1) {
    this->expression = expression1;
}

double UMinus::calculate() {
    return -1 * this->expression->calculate();
}

UMinus::~UMinus() {
    if (this->expression != nullptr) {
        delete this->expression;
    }
    // cout << "UMinus object deleted!!!" << endl;
}

UMinus::UMinus(Expression *expression1) {
    this->expression = expression1;
}

//value functions
double Value::calculate() {
    return this->value1;
}

//Value::Value(double value1) {
//    this->value = value1;
//}

Value::~Value() {
}

//binary functions
Plus::~Plus() {
    if (this->left != nullptr) {
        delete this->left;
    }
    if (this->right != nullptr) {
        delete this->right;
    }
    //   cout << "Plus object deleted!!!" << endl;
}

double Plus::calculate() {
    return this->left->calculate() + this->right->calculate();
}

// plus constructor
Plus::Plus(Expression *left1, Expression *right1) {
    this->left = left1;
    this->right = right1;
}

double Minus::calculate() {
    return this->left->calculate() - this->right->calculate();
}

Minus::~Minus() {
    if (this->left != nullptr) {
        delete this->left;
    }
    if (this->right != nullptr) {
        delete this->right;
    }
    //   cout << "Minus object deleted!!!" << endl;
}

Minus::Minus(Expression *left1, Expression *right1) {
    this->left = left1;
    this->right = right1;
}

double Mul::calculate() {
    return this->left->calculate() * this->right->calculate();
}

Mul::~Mul() {
    if (this->left != nullptr) {
        delete this->left;
    }
    if (this->right != nullptr) {
        delete this->right;
    }
    //  cout << "Mul object deleted!!!" << endl;
}

Mul::Mul(Expression *left1, Expression *right1) {
    this->left = left1;
    this->right = right1;
}

double Div::calculate() {

    if (this->right->calculate() == 0) {
        throw "division by zero";
    } else {
        return this->left->calculate() / this->right->calculate();
    }
}

Div::~Div() {
    if (this->left != nullptr) {
        delete this->left;
    }
    if (this->right != nullptr) {
        delete this->right;
    }
    // cout << "Div object deleted!!!" << endl;
}

Div::Div(Expression *left1, Expression *right1) {
    this->left = left1;
    this->right = right1;
}


//relation Operators class---------------------------------------------
relationalOperators::relationalOperators(Expression *left1, Expression *right1) {
    this->left = left1;
    this->right = right1;
}
//relationalOperators::~relationalOperators() {
//    // cout << "relationOperators object deleted!!!" << endl;
//}

relationalOperators::~relationalOperators() {
    if (this->left != nullptr) {
        delete this->left;
    }
    if (this->right != nullptr) {
        delete this->right;
    }
    // cout << "relationOperators object deleted!!!" << endl;
}


double Bigger::calculate() {
    if (this->left->calculate() > this->right->calculate()) {
        return 1;
    }
    return 0;
}

double BiggerEqual::calculate() {
    if (this->left->calculate() >= this->right->calculate()) {
        return 1;
    }
    return 0;
}

double Smaller::calculate() {
    if (this->left->calculate() < this->right->calculate()) {
        return 1;
    }
    return 0;
}

double SmallerEqual::calculate() {
    if (this->left->calculate() <= this->right->calculate()) {
        return 1;
    }
    return 0;
}

double Equals::calculate() {
    if (this->left->calculate() == this->right->calculate()) {
        return 1;
    }
    return 0;
}

double NotEquals::calculate() {
    if (this->left->calculate() != this->right->calculate()) {
        return 1;
    }
    return 0;
}

//----------------------------------------------------------
//variable functions
double Variable::calculate() {
    return this->value;
}

Variable::~Variable() {
}

Variable::Variable(string name1, double value1) {
    this->name = name1;
    this->value = value1;
}

//Variable::Variable(double value1, string name1) {
//    this->name = name1;
//    this->value = value1;
//}

Variable &Variable::operator++() {
    this->value = this->value + 1;
    return *this;
}

Variable &Variable::operator--() {
    this->value = this->value - 1;
    return *this;
}

Variable &Variable::operator+=(double value1) {
    this->value = this->value + value1;
    return *this;
}

Variable &Variable::operator-=(double value1) {
    this->value = this->value - value1;
    return *this;
}

Variable &Variable::operator++(int) {
    this->value = this->value + 1;
    return *this;
}

Variable &Variable::operator--(int) {
    this->value = this->value - 1;
    return *this;
}

//Interpreter class

bool Interpreter::checkIfOperator(string c) {
    if ((c == "-") || (c == "*") || (c == "+") || (c == "/") || (c == "-u") || (c == "+u")||(c=="==")||(c=="<=")||(c==">=")
        ||(c==">")||(c=="<")||(c=="!=")) {
        return true;
    }
    return false;
}

bool Interpreter::checkIfRelativeOperator(string relative) {
    if((relative=="!=")||(relative=="==")||(relative=="<=")||(relative==">=")){
        return true;
    }
    return false;
}
bool Interpreter::checkIfDigit(char c) {
    if (c >= '0' && c <= '9') {
        return true;
    }
    return false;
}

bool Interpreter::checkIfNumber(string c) {
    int countP = 0;
    if (c.empty()) {
        return false;
    }
    for (char c1:c) {
        if ((!this->checkIfDigit(c1))) {
            if ((c1 == '.') && (c[0] != '.') && (countP <= 1)) {
                countP++;
            } else {
                return false;
            }
        }
    }
    return true;
}

bool Interpreter::checkIfVariable(string c) {
    char letter;
    bool inputValid = true;
    unsigned int i;
    //check if first is letter
    if (!isalpha(c[0]) && (c[0] != '_')) {
        throw "unvalid input";
    }
    for (i = 1; i < c.size(); i++) {
        letter = c[i];
        //if letter or '_'
        if ((isalpha(letter)) || (letter == '_')) {
            i++;
        }
            //if there is a number like X1
        else if (this->checkIfDigit(letter)) {
            i++;
        } else {
            inputValid = false;
            break;
        }
    }
    if (inputValid) {
        return true;
    } else {
        return false;
    }

}

bool Interpreter::checkIfBracket(string c) {
    if ((c == "(") || (c == ")")) {
        return true;
    }
    return false;
}

//check precedence
bool Interpreter::check(string c, stack<string> stack) {
    if (stack.empty()) {
        return false;
    }
    if (((stack.top() == "*") || (stack.top() == "/") || (stack.top() == "-u") || (stack.top() == "+u")) &&
        ((c == "-") || (c == "+"))) {
        return true;
    }
    //the unary have the highest precedence
    if ((stack.top() == "-u" || (stack.top() == "+u")) && (c != "-u") && (c != "+u")) {
        return true;
    }
    //<=,>=,<,> have priority on == , !=
    if(((stack.top()=="<=")||(stack.top()=="<")||(stack.top()==">=")||(stack.top()==">"))&&(c=="==")||(c=="!=")){
        return true;
    }
    //otherwise each operator have priority on relative operation
    if(this->checkIfRelativeOperator(c)||(c=="<")||(c==">")){
        return true;
    }

    return false;
}

//create variable expression /value expression
Expression *Interpreter::Initial(string var) {
    double num1;
    Expression *expression;
    if (this->checkIfNumber(var)) {
        num1 = stod(var);
        expression = new Value(num1);
    } else {
        if (symbolTable.count(var) == 0) {
            throw "not found in setVeraible";
        } else {
            expression = new Variable(var, symbolTable.at(var)->getValue());
        }
    }
    return expression;
}

void Interpreter::setVariables(string initialize) {
    string word;
    string var, value;
    double value1;
    unsigned int i = 0;
    bool negative = false;
    stringstream stream(initialize);
    while (getline(stream, word, ';')) {
        while (word[i] != '=') {
            var = var + "" + word[i];
            i++;
        }
        if (!this->checkIfVariable(var)) {
            throw "illegal variable assignment!";
        }
        value = word.substr(i + 1);
        if (value[0] == '-') {
            negative = true;
        }
        if (!this->checkIfNumber(value)) {
            if (negative) {
                if (!this->checkIfNumber(value.substr(1))) {
                    throw "illegal variable assignment!";
                }
            } else {
                throw "illegal variable assignment!";
            }
        }
        value1 = stod(value);
//        if (this->valuesOfVariables.count(var) > 0) {
//            this->valuesOfVariables.at(var) = value1;
//        } else {
//            this->valuesOfVariables.insert(pair<string, double>(var, value1));
//        }
        i = 0;
        var = "", value = "";
        negative = false;
    }
}


//create polish notion
queue<string> Interpreter::polish(string expression) {
    expression=earseChar(expression," ");
    string d;
    string c1;
    string c, nextString;
    string nextRelative="";
    bool flag=false;
    string unvalid;
    unsigned int i;
    //create tokens
    for (i = 0; i < expression.size(); i++) {
        c = expression[i];
        if ((i < expression.size() - 1)&&((c=="=")||(c==">")||(c=="<")||(c=="!"))) {
            nextRelative =c+ expression[i + 1];
        }
        if (this->checkIfBracket(c)) {
            unvalid = expression[i + 1];
            //in case the expression unvalid like (u)(u)
            if ((c == ")") && (i != expression.size() - 1) && (!(this->checkIfOperator(unvalid)||(unvalid!="=")||(unvalid!="!"))) && (unvalid != ")")) {
                throw "illegal ";
            }
            this->tokens.push_back(c);
        } else if ((this->checkIfOperator(c))||(this->checkIfRelativeOperator(nextRelative))) {
            if(this->checkIfRelativeOperator(nextRelative)){
                flag=true;
                if (i < expression.size() - 2) {
                    nextString = expression[i + 2];
                    //if there are two operator in a row
                    if (this->checkIfOperator(nextString)||(nextString=="=")||(nextString=="!")) {
                        throw "illegal math expression";
                    }
                }
            }
            else if (i < expression.size() - 1) {
                nextString = expression[i + 1];
                //if there are two operator in a row
                if (this->checkIfOperator(nextString)) {
                    throw "illegal math expression";
                }
            }
            //check if unary
            if ((i == 0) || (expression[i - 1] == '(')) {
                //mark him as unary for next step
                c = c + "u";
                this->tokens.push_back(c);
            } else {
                if(flag){
                    this->tokens.push_back(nextRelative);
                    i++;
                }
                else {
                    this->tokens.push_back(c);
                }
            }
        } else {
            while ((!(this->checkIfOperator(c))) && (!(this->checkIfBracket(c))) && (i < expression.size())&&(!this->checkIfRelativeOperator(nextRelative))) {
                c1 = c1 + "" + c;
                i++;
                c = expression[i];
                if((i < expression.size() - 1)&&((c=="=")||(c==">")||(c=="<")||(c=="!"))) {
                    nextRelative = c+""+expression[i + 1];
                }
            }
            this->tokens.push_back(c1);
            c1 = "";
            i--;
        }
        nextRelative="";
        flag= false;
    }

    // make it reverse polish notion
    for (string token1: tokens) {
        if (this->checkIfOperator(token1)) {
            while (check(token1, this->operators)) {
                d = this->operators.top();
                this->output.push(d);
                this->operators.pop();
            }
            this->operators.push(token1);
        } else if (token1 == "(") {
            this->operators.push(token1);
        } else if (token1 == ")") {
            while (this->operators.top() != "(") {
                d = this->operators.top();
                this->operators.pop();
                this->output.push(d);
            }
            this->operators.pop();
        } else if (this->checkIfNumber(token1) || (this->checkIfVariable(token1))) {
            this->output.push(token1);
        }
    }
    while (!this->operators.empty()) {
        d = this->operators.top();
        this->output.push(d);
        this->operators.pop();

    }
    return this->output;
}

//create expression
Expression *Interpreter::interpret(string expression) {
    queue<string> q1 = this->polish(expression);
    Expression *expression1, *left, *right, *final;
    stack<Expression *> values;
    while (!q1.empty()) {
        while (!this->checkIfOperator(q1.front()) && (!q1.empty())) {
            //to check validation of bracket(if left it means there are not equal)
            if (this->checkIfBracket(q1.front())) {
                throw "illegal";
            }
            values.push(this->Initial(q1.front()));
            q1.pop();
        }
        //if unary
        if ((q1.front() == "-u") || (q1.front() == "+u")) {
            switch (q1.front()[0]) {
                case '-':
                    // expression2=this->Initial()
                    expression1 = new UMinus(values.top());
                    values.pop();
                    values.push(expression1);
                    q1.pop();
                    break;
                case '+':
                    expression1 = new UPlus(values.top());
                    values.pop();
                    values.push(expression1);
                    q1.pop();
                    break;

                default:
                    break;
            }
        }
            //if binary
        else if (this->checkIfOperator(q1.front())) {
            right = values.top();
            values.pop();
            left = values.top();
            values.pop();
            //in case of relative operator such as "=="/"<="/">="/"!="
            if(this->checkIfRelativeOperator(q1.front())){
                if(q1.front()=="=="){
                    values.push(new Equals(left, right));
                    q1.pop();
                }
                else if(q1.front()=="!="){
                    values.push(new NotEquals(left, right));
                    q1.pop();
                }
                else if(q1.front()==">="){
                    values.push(new BiggerEqual(left, right));
                    q1.pop();
                }
                else if(q1.front()=="<="){
                    values.push(new SmallerEqual(left, right));
                    q1.pop();
                }

            }
            else {
                switch (q1.front()[0]) {
                    case '-':
                        values.push(new Minus(left, right));
                        q1.pop();
                        break;
                    case '+':
                        values.push(new Plus(left, right));
                        q1.pop();
                        break;
                    case '*':
                        values.push(new Mul(left, right));
                        q1.pop();
                        break;
                    case '/':
                        values.push(new Div(left, right));
                        q1.pop();
                        break;
                    case '>':
                        values.push(new Bigger(left, right));
                        q1.pop();
                        break;
                    case '<':
                        values.push(new Smaller(left, right));
                        q1.pop();
                        break;
                    default:
                        break;
                }
            }
        }

    }
    final = values.top();
    values.pop();
    return final;
}












