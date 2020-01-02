// reading a text file
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <vector>
#include <sstream>
#include <unordered_map>
#include "Command.h"
#include "GlobalFunction.h"

#include "Interpreter.h"
using namespace std;
using  namespace Global_Functions;


int main(int argc, char* argv[]) {
    creatMapSimToPairVar();
    if (argc ==1){
        cout<<"missing text file to run the simultor"<<endl;
        return 0;
    }
    vector<string> vec2 = lexer(argv[1]);
    parser(vec2, "main");
    finishLock.lock();
    finishLock.unlock();

    return 0;
}
