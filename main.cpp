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


int main() {
 //   lock_guard<mutex> lock(finishLock);
    creatMapSimToPairVar();
    vector<string> vec2 = lexer();
    parser(vec2, "main");
  //  std::this_thread::sleep_for (std::chrono::seconds(200));
    finishLock.lock();
//    std::cout << "Hello, World!" << std::endl;
    finishLock.unlock();

    return 0;
}
