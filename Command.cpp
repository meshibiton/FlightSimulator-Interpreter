//
// Created by meshi on 23/12/2019.
//

#include <utility>
#include "Command.h"
#include <iostream>
#include <bits/stdc++.h>
#include <vector>
#include<unordered_map>
#include "Interpreter.h"
#include "GlobalFunction.h"
#include <iterator>



using namespace Global_Functions;

using namespace std;
//global variables-we will be able to reach them everywhere..


///opensevercommand class function
//-------------------------------------------------
int OpenSeverCommand::execute(vector<string> v) {
    this->numPort = stoi(v.at(0));
    this->numParm = 1;
    //here we will need to call the func ‫‪openDataServe
    openDataServer(this->numPort);
    return this->numParm;
}
//
//void OpenSeverCommand::openDataServer() {
//    //openning the thread and etc
//
//}

void OpenSeverCommand::setNumPort(int numPort) {
    OpenSeverCommand::numPort = numPort;
}

void OpenSeverCommand::setNumParm(int numParm) {
    OpenSeverCommand::numParm = numParm;
}

//------------------------------------------------------


///ConnectCommand class function
//--------------------------------------------------------
int ConnectCommand::execute(vector<string> v) {
    this->ip = v.at(0);
    this->numPort = stoi(v.at(1));
    this->numParm = 2;
    Global_Functions::connectControlClient(this->ip, this->numPort);
    return this->numParm;

}



///Var class function
//--------------------------------------------------------
int Var::execute(vector<string> v) {
    //need to do tell the simulator about the change- if the side is >
   // cout << "map" << std::endl;
}

Var::Var(string nameVar1, string side1, string sim1) { // Constructor with parameters
    this->nameVar = nameVar1;
    this->sim = sim1;
    this->side = side1;
}

//just a var ,doesnt belong to the simulator
Var::Var(string nameVar1, double value) { // Constructor with parameters
    this->nameVar = nameVar1;
    this->value = value;
}

void Var::setValue(double value1) {
    this->value = value1;
}

const string &Var::getSide() const {
    return side;
}

const string &Var::getSim() const {
    return sim;
}

double Var::getValue() const {
    return value;
}

//--------------------------------------------------------

//breaks -> sim("/controls/flight/speedbreak")
//breaks = 0
//--------------------------------------------------------
int DefineVarCommand::execute(vector<string> v) {
//    lockSimulatorTable.lock();
    //if var already exit
    if (symbolTable.count(v.at(0)) > 0) {
        //that's mean we are gonna change his value ,contain '='
        if (v.at(1).compare("=") == 0) {
            //update his value
            double newValue = interpreter(v.at(2));
            symbolTable.at(v.at(0))->setValue(newValue);
            // symbolTable.at(v.at(0))->setValue(stoi(v.at(2)));
            if (v.at(0).compare("rpm")==0){
                cout<<to_string(symbolTable.at(v.at(0))->getValue())<<endl;
            }
            symbolTable.at(v.at(0))->execute(v);
            this->numParm = 3;
            //now we will push a new message to the client queue,he will send them to the simulator
            string path = symbolTable.at(v.at(0))->getSim().substr(1);
            //thats mean it gonna effect the simulator
            if (symbolTable.at(v.at(0))->getSide() == "->") {
                Global_Functions::queueMessages.push("set " + path + " " + to_string(newValue) + "\r\n");
            }
        }
    } else {
        //case "var h0=heading"-var that doesnt belong to simulator
        if (v.at(1).compare("=") == 0) {
            Var *v1 = new Var(v.at(0), interpreter(v.at(2)));
            //insert it to our map-string name var and var with his info
            symbolTable[v.at(0)] = v1;
            this->numParm = 3;
        } else {
            //create new var that do belong the simulator
            Var *v1 = new Var(v.at(0), v.at(1), v.at(3));
            //insert it to our map-string name var and var with his info
            symbolTable[v.at(0)] = v1;
            //add this var to the simulator map so after reciving data we will be able to update the var
            if (v1->getSide() == "<-") {
                unordered_map<string, pair<Var*,float >>::iterator iter;
                iter = mapSimToPairVar.find(v1->getSim());
                if(iter!= mapSimToPairVar.end()) {
                    //update him to point to the symboltable var
                    (*iter).second.first = v1 ;
                }
            }
            this->numParm = 4;
        }

    }
    lockSimulatorTable.unlock();
    return this->numParm;


}

//--------------------------------------------------------

//--------------------------------------------------------
int ConditionParser::execute(vector<string> v) {
    //create the condtions vector
    vector<string> conditions;
    int i;
    //minmum-{,},condtion.
    int count = 4;
    this->condition = v.at(1);
    if (v.at(2).compare("{") == 0) {
        i = 3;
        while ((v.at(i).compare("}")) != 0) {
            //add the condtion to the vector
            conditions.push_back(v.at(i));
            i++;
            count++;
        }
    }
    //check which case is it
    //will go to his child-if or loop command,and run the excute commands
    if (v.at(0).compare("while") == 0) {
        LoopCommand loopCommand;
        loopCommand.condition = this->condition;
        loopCommand.execute(conditions);
    } else if (v.at(0).compare("if") == 0) {
        IfCommand ifCommand;
        ifCommand.condition = this->condition;
        ifCommand.execute(conditions);
    }
    return count;
}

bool ConditionParser::checkIfTrue(string condition) {
    double result;
    result = interpreter(condition);
    if (result) {
        return true;
    }
    return false;
}
//--------------------------------------------------------


int IfCommand::execute(vector<string> v) {
    //initialize the condition bool
    this->flagCondition = checkIfTrue(this->condition);
    if (this->flagCondition) {
        //if the condition is true we send the comannds to be excute by the func parser
        parser(v, "commandOnly");
    }
    return 0;
}
//--------------------------------------------------------

int LoopCommand::execute(vector<string> v) {
    //initialize the condition bool
    this->flagCondition = checkIfTrue(this->condition);
    while (this->flagCondition) {
        //if the condition is true we send the comannds to be excute by the func parser
        parser(v, "commandOnly");
        this->flagCondition = checkIfTrue(this->condition);
    }
    return 0;
}

//----------------------------------
int SleepCommand::execute(vector<string> v) {
//make the thread sleep
    this->numparm = 1;
    this->mili = interpreter(v.at(0));
    std::this_thread::sleep_for (std::chrono::milliseconds(10*(this->mili)));
    return this->numparm;
}


//--------------------------------------------
int PrintCommand::execute(vector<string> v) {
    if (v.at(0).rfind("\"", 0) == 0) {
//    if ((v.at(0)).rfind("\"", 0) != std::string::npos) {
//        //that's means it's string,delet the quotes
        this->printValue = earseChar(v.at(0), "\"");
    } else {
        //in case of expression
        this->printValue = to_string(interpreter(v.at(0)));
    }
    this->numParm = 1;
    std::cout << this->printValue << std::endl;
    return this->numParm;
}


//--------------------------------------------------
//void DefineVarCommand:: BuildList(vector<string> v) {
//    list<Command *> commandss;
//    int i = 0;
//    if (v.at(0).compare("{")==0) {
//        i = 1;
//        while ((v.at(i).compare("}")) != 0) {
//            //check if it exit in the vars map ,that's means its var
//            if (symbolTable.count(v.at(i)) > 0) {
//                if (v.at(i+1).compare("=")==0) {
//                    //update his value
//                    symbolTable.at(v.at(i))->setValue(stoi(v.at(i+2)));
//                    //we will add him to the list we need to excute
//                    commandss.push_back(symbolTable.at(v.at(i)));
//                    i = i + 3;
//                }
////                //if its print command/sleep command
////                if (v.at(i).compare("Print")) {
////                    PrintCommand printCommand = command_table.at(v.at(i));
//////                    this->commands.push_back(command_table.at(v.at(i)));
////i=i++;
////                }
//
//            }
//        }
//    }
//}
//            //check if it exit in the vars map ,that's means its var
//            if (symbolTable.count(v.at(i)) > 0) {
//                if (v.at(i + 1).compare("=")) {
//                    //update his value
//                    symbolTable.at(v.at(i))->setValue(stoi(v.at(2)));
//                    //we will add him to the list we need to excute
//                    this->commands.push_back(symbolTable.at(v.at(i)));
//                }
//                    //if its print command/sleep command
//               if(v.at(i).compare("Print")) {
//                   PrintCommand printCommand=command_table.at(v.at(i));
//                    this->commands.push_back(command_table.at(v.at(i)));
//                }
//            }
//        }