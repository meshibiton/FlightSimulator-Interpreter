
// Created by meshi on 24/12/2019.

#include <iostream>
#include <bits/stdc++.h>
#include <vector>
#include<unordered_map>
#include "Interpreter.h"
#include "Command.h"
#include "GlobalFunction.h"
#include <algorithm>
#include <iostream>
#include <thread>
#include <sys/socket.h>
#include <string>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <netinet/in.h>
#include <map>
#include <string>
#include <algorithm>
#include <utility>
#include <bits/unordered_map.h>
#include <unordered_map>
#include <mutex>
#include <sys/socket.h>
#include <string>
#include <iostream>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace std;
using namespace this_thread;
using namespace chrono;

namespace Global_Functions {

    //our global maps and variables
    unordered_map<string, Command *> command_table;
    unordered_map<string, Var *> symbolTable;
    unordered_map<string, pair<Var *, float>> mapSimToPairVar;
    queue<string> queueMessages;
    std::mutex finishLock;
    std::mutex lockSimulatorTable;
    bool closeSocketServer = false;
    bool isDoneCloseSocketClient = false;


//remove right space-------------------------
    string TrimRight(std::string &str) {
        str.erase(str.find_last_not_of(' ') + 1);
        return str;
    }


//remove prefixing space--------------------------------------------
    string TrimLeft(const std::string &str) {
        auto pos = str.find_first_not_of(' ');
        return str.substr(pos != std::string::npos ? pos : 0);
    }


//create the commend table--------------------------------------------
    void creatMapCommend() {
        command_table["openDataServer"] = new OpenSeverCommand();
        command_table["connectControlClient"] = new ConnectCommand();
        command_table["if"] = new ConditionParser();
        command_table["while"] = new ConditionParser();
        command_table["var"] = new DefineVarCommand();
        command_table["Print"] = new PrintCommand();
        command_table["Sleep"] = new SleepCommand();
    }


//remove space from the end--------------------------------------------

    string earseChar(string string1, string chars) {
        for (char c: chars) {
            string1.erase(std::remove(string1.begin(), string1.end(), c), string1.end());
            return string1;
        }

    }

//print the vector--------------------------------------------
    void printVector(vector<string> tokens) {
        // Printing the token vector
        for (int i = 0; i < tokens.size(); i++)
            //    cout << tokens[i] + ' ';
            cout << tokens[i] + ' ';
    }


//create vector by split by the ' ' char--------------------------------------------

    vector<string> lineVector(string line, char char1) {
        // Vector of string to save tokens
        vector<string> tokens;
        stringstream check1(line);
        string intermediate;
        // Tokenizing w.r.t. space ' '
        while (getline(check1, intermediate, char1)) {
            tokens.push_back(intermediate);
        }
        return tokens;
    }

//implement the execute of each line--------------------------------------------

    void parser(vector<string> vectorLexer, string flagCondition) {
//        finishLock.lock();
        vector<string>::iterator it1;
        string tempString;
        int index = 0;
        int oldVectorSize = vectorLexer.size();
        for (int i = 0; i < oldVectorSize;) {
            tempString = vectorLexer[0];

            //when to delete the first object before sending
            if (tempString == "connectControlClient" || tempString == "openDataServer" || tempString == "var"
                || tempString == "Sleep" || tempString == "Print") {
                vectorLexer.erase(vectorLexer.begin());
                i = i + 1;

            }
            it1 = vectorLexer.begin();
            // if that  var  already in the map
            //ex:  warp = 32000
            if (symbolTable.count(tempString) > 0) {
                index = command_table.at("var")->execute(vectorLexer);
                vectorLexer.erase(it1, it1 + index);
                i = i + index;
            } else {
                index = command_table.at(tempString)->execute(vectorLexer);
                vectorLexer.erase(it1, it1 + index);
                i = i + index;
            }

        }
        //the loop came from the main
        if (flagCondition == "main") {
            //kind of messege to close the socket client, and the client will send to the server, the server will unlock
            isDoneCloseSocketClient = true;
        }
    }


//create tokens -vector of tokens--------------------------------------------

    vector<string> lexer() {
        creatMapCommend();
        //read from file
        string line;
        vector<string> lexerVactor;
        ifstream myfile("fly.txt");
        if (myfile.is_open()) {
            while (getline(myfile, line)) {
                vector<string> tempResultVector;
                //check if the prefix is openDataServer
                //example:  openDataServer(5400)
                line = earseChar(line, "\t");
                line = TrimLeft(line);

                if (line.rfind("openDataServer", 0) == 0) {
                    line = earseChar(line, ")");
                    replace(line.begin(), line.end(), '(', ' ');
                    tempResultVector = lineVector(line, ' ');
                    lexerVactor.insert(lexerVactor.end(), tempResultVector.begin(), tempResultVector.end());


                } else
                    //check if the prefix is connectControlClient
                    //example:   connectControlClient("127.0.0.1",5402)
                if (line.rfind("connectControlClient", 0) == 0) {
                    line = earseChar(line, ")");
                    line = earseChar(line, "\"");
                    replace(line.begin(), line.end(), '(', ' ');
                    replace(line.begin(), line.end(), ',', ' ');
                    tempResultVector = lineVector(line, ' ');
                    lexerVactor.insert(lexerVactor.end(), tempResultVector.begin(), tempResultVector.end());
                } else
                    //check if the prefix is var
                if (line.rfind("var", 0) == 0) {
                    //example:    var warp = 0 +4
                    if (line.find('=') != std::string::npos) {
                        tempResultVector = lineVector(line, '=');
                        lexerVactor.push_back(lineVector(line, ' ')[0]);
                        lexerVactor.push_back(lineVector(line, ' ')[1]);
                        lexerVactor.push_back("=");
                        lexerVactor.push_back(tempResultVector[1]);
                        //ex:    var warp -> sim("/sim/time/warp")
                    } else {
                        line = earseChar(line, ")");
                        line = earseChar(line, "\"");
                        replace(line.begin(), line.end(), '(', ' ');
                        tempResultVector = lineVector(line, ' ');
                        lexerVactor.insert(lexerVactor.end(), tempResultVector.begin(), tempResultVector.end());
                    }
                } else
                    //check if the prefix is while
                    //ex:  while rpm <= 750 {
                if (line.rfind("while", 0) == 0) {
                    string condition = "";
                    line = earseChar(line, ")");
                    replace(line.begin(), line.end(), '(', ' ');
                    tempResultVector = lineVector(line, ' ');

                    for (int i = 1; tempResultVector.size(); i++) {
                        if (tempResultVector[i] == "{") {
                            break;
                        } else {
                            condition = condition + tempResultVector[i];
                        }
                    }
                    lexerVactor.push_back("while");
                    lexerVactor.push_back(condition);
                    lexerVactor.push_back("{");
                } else
                    //	check if the prefix is Print
                    //ex:  Print(rpm)
                if (line.rfind("Print", 0) == 0) {
                    line = earseChar(line, ")");
//                line = earseChar(line, "\"");
                    tempResultVector = lineVector(line, '(');
                    lexerVactor.insert(lexerVactor.end(), tempResultVector.begin(), tempResultVector.end());
                } else
                    //check if the prefix is while
                    //example: if (a == b) {
                if (line.rfind("if", 0) == 0) {

                    replace(line.begin(), line.end(), '(', ' ');
                    replace(line.begin(), line.end(), ')', ' ');
                    tempResultVector = lineVector(line, '=');
                    lexerVactor.insert(lexerVactor.end(), tempResultVector.begin(), tempResultVector.end());
                } else
                    //ex: primer = 3, 	aileron = -roll / 70
                    //ex: if line contain '='
                if (line.find('=') != std::string::npos) {
//                replace(line.begin(), line.end(), '(', ' ');
//                replace(line.begin(), line.end(), ')', ' ');
                    tempResultVector = lineVector(line, '=');
                    lexerVactor.push_back(tempResultVector[0]);
                    lexerVactor.push_back("=");
                    lexerVactor.push_back(tempResultVector[1]);

                }
//            all the other line
//           ex: Print(rpm)
                else {
                    replace(line.begin(), line.end(), '(', ' ');
                    line = earseChar(line, ")");
                    tempResultVector = lineVector(line, ' ');
                    lexerVactor.insert(lexerVactor.end(), tempResultVector.begin(), tempResultVector.end());
                }
            }
            //remove the spaces from the and the beginning
            for (int i = 0; i < lexerVactor.size(); i++) {
                lexerVactor[i] = TrimLeft(lexerVactor[i]);
                lexerVactor[i] = TrimRight(lexerVactor[i]);
            }
            myfile.close();
            return lexerVactor;
        } else {
            cout << "Unable to open file" << endl;
        }
    }


//return the result of expression--------------------------------------------

    double interpreter(string expression) {
        Interpreter *i = new Interpreter();
        Expression *e = nullptr;
        try {
            e = i->interpret(expression);
            double result = e->calculate();
            //std::cout << "6: " << result << std::endl;//-10
            delete e;
            delete i;
            return result;
        } catch (const char *e) {
            if (e != nullptr) {
                delete e;
            }
            if (i != nullptr) {
                delete i;
            }
        }
    }
////------------ function for openDataServer------------------

//update each line we get from the simultor

    void updateVariablesVul(string line) {

        unordered_map<string, pair<Var *, float >>::iterator iter;
        string stringNum = "";
        int counter = 0;
        for (int i = 0; i < line.length(); i++) {
            char char1 = line[i];
            if (char1 != ',' && char1 != '\n') {
                stringNum += char1;
            } else {
                switch (counter) {
                    case 0:
                        iter = mapSimToPairVar.find("/instrumentation/airspeed-indicator/indicated-speed-kt");
                        break;
                    case 1:
                        iter = mapSimToPairVar.find("/sim/time/warp");
                        break;
                    case 2:
                        iter = mapSimToPairVar.find("/controls/switches/magnetos");
                        break;
                    case 3:
                        iter = mapSimToPairVar.find("/instrumentation/heading-indicator/indicated-heading-deg");
                        break;
                    case 4:
                        iter = mapSimToPairVar.find("/instrumentation/altimeter/indicated-altitude-ft");
                        break;
                    case 5:
                        iter = mapSimToPairVar.find("/instrumentation/altimeter/pressure-alt-ft");
                        break;
                    case 6:
                        iter = mapSimToPairVar.find("/instrumentation/attitude-indicator/indicated-pitch-deg");
                        break;
                    case 7:
                        iter = mapSimToPairVar.find("/instrumentation/attitude-indicator/indicated-roll-deg");
                        break;
                    case 8:
                        iter = mapSimToPairVar.find("/instrumentation/attitude-indicator/internal-pitch-deg");
                        break;
                    case 9:
                        iter = mapSimToPairVar.find("/instrumentation/attitude-indicator/internal-roll-deg");
                        break;
                    case 10:
                        iter = mapSimToPairVar.find("/instrumentation/encoder/indicated-altitude-ft");
                        break;
                    case 11:
                        iter = mapSimToPairVar.find("/instrumentation/encoder/pressure-alt-ft");
                        break;
                    case 12:
                        iter = mapSimToPairVar.find("/instrumentation/gps/indicated-altitude-ft");
                        break;
                    case 13:
                        iter = mapSimToPairVar.find("/instrumentation/gps/indicated-ground-speed-kt");
                        break;
                    case 14:
                        iter = mapSimToPairVar.find("/instrumentation/gps/indicated-vertical-speed");
                        break;

                    case 15:
                        iter = mapSimToPairVar.find("/instrumentation/heading-indicator/offset-deg");
                        break;
                    case 16:
                        iter = mapSimToPairVar.find("/instrumentation/magnetic-compass/indicated-heading-deg");
                        break;
                    case 17:
                        iter = mapSimToPairVar.find("/instrumentation/slip-skid-ball/indicated-slip-skid");
                        break;
                    case 18:
                        iter = mapSimToPairVar.find("/instrumentation/turn-indicator/indicated-turn-rate");
                        break;
                    case 19:
                        iter = mapSimToPairVar.find("/instrumentation/vertical-speed-indicator/indicated-speed-fpm");
                        break;
                    case 20:
                        iter = mapSimToPairVar.find("/controls/flight/aileron");
                        break;
                    case 21:
                        iter = mapSimToPairVar.find("/controls/flight/elevator");
                        break;
                    case 22:
                        iter = mapSimToPairVar.find("/controls/flight/rudder");
                        break;
                    case 23:
                        iter = mapSimToPairVar.find("/controls/flight/flaps");
                        break;
                    case 24:
                        iter = mapSimToPairVar.find("/controls/engines/engine/throttle");
                        break;
                    case 25:
                        iter = mapSimToPairVar.find("/controls/engines/current-engine/throttle");
                        break;
                    case 26:
                        iter = mapSimToPairVar.find("/controls/switches/master-avionics");
                        break;
                    case 27:
                        iter = mapSimToPairVar.find("/controls/switches/starter");
                        break;
                    case 28:
                        iter = mapSimToPairVar.find("/engines/active-engine/auto-start");
                        break;
                    case 29:
                        iter = mapSimToPairVar.find("/controls/flight/speedbrake");
                        break;
                    case 30:
                        iter = mapSimToPairVar.find("/sim/model/c172p/brake-parking");
                        break;
                    case 31:
                        iter = mapSimToPairVar.find("/controls/engines/engine/primer");
                        break;
                    case 32:
                        iter = mapSimToPairVar.find("/controls/engines/current-engine/mixture");
                        break;
                    case 33:
                        iter = mapSimToPairVar.find("/controls/switches/master-bat");
                        break;
                    case 34:
                        iter = mapSimToPairVar.find("/controls/switches/master-alt");
                        break;
                    case 35:
                        iter = mapSimToPairVar.find("/engines/engine/rpm");
                        break;
                    default:
                        break;
                }

                float numDouble = stof(stringNum);

                //avoid from critical section
                lockSimulatorTable.lock();
                iter->second.second = numDouble;
                if (iter->second.first != nullptr) {
                    iter->second.first->setValue(numDouble);
                }
                lockSimulatorTable.unlock();
                stringNum = "";
                counter++;
            }
        }
    }

    //clean the buffer before sending it to updat the map, send line after line
    void updateBufAndValue(char buffer[]) {

        string lineForTable;
        string str;
        str = buffer;
        int isExist;
        size_t position = str.find("\n");      // position of "live" in str
        //substr the first line mybe she isn"t completly
        isExist = str.find("\n");
        while (isExist != -1) {

            lineForTable = str.substr(0, isExist+1);
            updateVariablesVul(lineForTable);
            str = str.substr(isExist + 1, str.length());
            isExist = str.find("\n");
        }
    }

//the function that starting the thread of serverside
    void serverSide(int client_socket) {
        while (!closeSocketServer) {
            char buffer[1024] = {0};
            //reading from client(simulator) 36
            int valread = read(client_socket, buffer, 1024);
            cout<<"-----------buffer------------------"<<endl;
            cout<<buffer<<endl;
            updateBufAndValue(buffer);
       //c     sleep_for(seconds(2));
            const char *hello = "Hello, I can hear you! \n";
            send(client_socket, hello, strlen(hello), 0);
        }
        finishLock.lock();
        if (closeSocketServer) {
            finishLock.unlock();
        }
//        close(client_socket);
////        finishLock.unlock();

    }



//open data server created uniq socket by special  port and we running the socket on separate thread

    void openDataServer(int port) {

        //create socket
        int socketfd = socket(AF_INET, SOCK_STREAM, 0);
        if (socketfd == -1) {
            //error
            std::cerr << "Could not create a socket" << std::endl;

        }
        //bind socket to IP address
        // we first need to create the sockaddr obj.
        sockaddr_in address; //in means IP4
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY; //give me any IP allocated for my machine
        address.sin_port = htons(5400);//5400
        //we need to convert our number
        // to a number that the network understands.

        //the actual bind command
        if (bind(socketfd, (struct sockaddr *) &address, sizeof(address)) == -1) {
            std::cerr << "Could not bind the socket to an IP" << std::endl;
            return;
        }
        //making socket listen to the port
        if (listen(socketfd, 5) == -1) { //can also set to SOMAXCON (max connections)
            std::cerr << "Error during listening command" << std::endl;
            return;
        } else {
            std::cout << "Server is now listening ..." << std::endl;
        }
        // accepting a client
        int client_socket = accept(socketfd, (struct sockaddr *) &address,
                                   (socklen_t *) &address);
        if (client_socket == -1) {
            std::cerr << "Error accepting client" << std::endl;
            return;

        }
        close(socketfd);

//        finishLock.lock();
        std::thread thread1(serverSide, client_socket);

        if (thread1.joinable()) {
            thread1.detach();
        }
    }

    void creatMapSimToPairVar() {

        mapSimToPairVar["/instrumentation/airspeed-indicator/indicated-speed-kt"] = make_pair(nullptr, 0);
        mapSimToPairVar["/sim/time/warp"] = std::make_pair(nullptr, 0);
        mapSimToPairVar["/controls/switches/magnetos"] = std::make_pair(nullptr, 0);
        mapSimToPairVar["/instrumentation/heading-indicator/offset-deg"] = std::make_pair(nullptr, 0);
        mapSimToPairVar["/instrumentation/altimeter/indicated-altitude-ft"] = std::make_pair(nullptr, 0);
        mapSimToPairVar["/instrumentation/altimeter/pressure-alt-ft"] = std::make_pair(nullptr, 0);
        mapSimToPairVar["/instrumentation/attitude-indicator/indicated-pitch-deg"] = std::make_pair(nullptr, 0);
        mapSimToPairVar["/instrumentation/attitude-indicator/indicated-roll-deg"] = std::make_pair(nullptr, 0);
        mapSimToPairVar["/instrumentation/attitude-indicator/internal-pitch-deg"] = std::make_pair(nullptr, 0);
        mapSimToPairVar["/instrumentation/attitude-indicator/internal-roll-deg"] = std::make_pair(nullptr, 0);
        mapSimToPairVar["/instrumentation/encoder/indicated-altitude-ft"] = std::make_pair(nullptr, 0);
        mapSimToPairVar["/instrumentation/encoder/pressure-alt-ft"] = std::make_pair(nullptr, 0);
        mapSimToPairVar["/instrumentation/gps/indicated-altitude-ft"] = std::make_pair(nullptr, 0);
        mapSimToPairVar["/instrumentation/gps/indicated-ground-speed-kt"] = std::make_pair(nullptr, 0);
        mapSimToPairVar["/instrumentation/gps/indicated-vertical-speed"] = std::make_pair(nullptr, 0);
        mapSimToPairVar["/instrumentation/heading-indicator/indicated-heading-deg"] = std::make_pair(nullptr, 0);
        mapSimToPairVar["/instrumentation/magnetic-compass/indicated-heading-deg"] = std::make_pair(nullptr, 0);
        mapSimToPairVar["/instrumentation/slip-skid-ball/indicated-slip-skid"] = std::make_pair(nullptr, 0);
        mapSimToPairVar["/instrumentation/turn-indicator/indicated-turn-rate"] = std::make_pair(nullptr, 0);
        mapSimToPairVar["/instrumentation/vertical-speed-indicator/indicated-speed-fpm"] = std::make_pair(nullptr, 0);
        mapSimToPairVar["/controls/flight/aileron"] = std::make_pair(nullptr, 0);
        mapSimToPairVar["/controls/flight/elevator"] = std::make_pair(nullptr, 0);
        mapSimToPairVar["/controls/flight/rudder"] = std::make_pair(nullptr, 0);
        mapSimToPairVar["/controls/flight/flaps"] = std::make_pair(nullptr, 0);
        mapSimToPairVar["/controls/engines/engine/throttle"] = std::make_pair(nullptr, 0);
        mapSimToPairVar["/controls/engines/current-engine/throttle"] = std::make_pair(nullptr, 0);
        mapSimToPairVar["/controls/switches/master-avionics"] = std::make_pair(nullptr, 0);
        mapSimToPairVar["/controls/switches/starter"] = std::make_pair(nullptr, 0);
        mapSimToPairVar["/engines/active-engine/auto-start"] = std::make_pair(nullptr, 0);
        mapSimToPairVar["/controls/flight/speedbrake"] = std::make_pair(nullptr, 0);
        mapSimToPairVar["/sim/model/c172p/brake-parking"] = std::make_pair(nullptr, 0);
        mapSimToPairVar["/controls/engines/engine/primer"] = std::make_pair(nullptr, 0);
        mapSimToPairVar["/controls/engines/current-engine/mixture"] = std::make_pair(nullptr, 0);
        mapSimToPairVar["/controls/switches/master-bat"] = std::make_pair(nullptr, 0);
        mapSimToPairVar["/controls/switches/master-alt"] = std::make_pair(nullptr, 0);
        mapSimToPairVar["/engines/engine/rpm"] = std::make_pair(nullptr, 0);


    }
////-----------------------------connect as a client to the simulator--------------------------------------------------


// Client side C/C++ program to demonstrate Socket programming

    void connectControlClient(string ip, int port) {

        std::thread thread2(clientSide, ip, port); //closing the listening socket);
        std::this_thread::sleep_for(std::chrono::seconds(2));
        if (thread2.joinable()) {
            thread2.detach();
        }

    }

//
//        close(client_socket);


//the function that starting the thread of server side
    void clientSide(string ip, int port) {
        //create socket
        int client_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (client_socket == -1) {
            //error
            std::cerr << "Could not create a socket" << std::endl;

        }

        //We need to create a sockaddr obj to hold address of server
        sockaddr_in address; //in means IP4
        address.sin_family = AF_INET;//IP4
        address.sin_addr.s_addr = inet_addr("127.0.0.1");  //the localhost address
        address.sin_port = htons(5402);//5402
        //we need to convert our number (both port & localhost)
        // to a number that the network understands.

        // Requesting a connection with the server on local host with port 8081
        int is_connect = connect(client_socket, (struct sockaddr *) &address, sizeof(address));
        if (is_connect == -1) {
            std::cerr << "Could not connect to host server" << std::endl;
            return;
        } else {
            std::cout << "Client is now connected to server" << std::endl;
            //std::this_thread::sleep_for (std::chrono::seconds(5));
            while (!Global_Functions::isDoneCloseSocketClient) {
                if (!Global_Functions::queueMessages.empty()) {
                    //take messege from the global queue
                           char bufferMessege[queueMessages.front().length()];
                 //   insert to the buffer messege
                strcpy(bufferMessege, queueMessages.front().c_str());


//                    int is_sent = send(client_socket, bufferMessege, queueMessages.front().length(), 0);
                    int is_sent =  write(client_socket,queueMessages.front().c_str(), queueMessages.front().length());
                    if (is_sent == -1) {
                        std::cout << "Error sending message" << std::endl;

                    } else {
                        char buffer[1024] = {0};
                        int valread = read(client_socket, buffer, 1024);
                        std::cout << buffer << std::endl;
                    //    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                    }
                    queueMessages.pop();
                }



            }
            close(client_socket);
            Global_Functions::closeSocketServer = true;


        }
    }
}