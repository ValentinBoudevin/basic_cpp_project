#include <vpn_mosquitto/vpn_mosquitto.hpp>
#include <functional>
#include <thread>
#include <jsoncpp/json/json.h>
#include <fstream>
#include <filesystem>
#include <iostream>
#include <chrono>
#include <sys/stat.h>
#include <functional>
#include <string.h>
#include <unistd.h>
#include <libgen.h>
#include <limits.h>
#include <cstring>

int main(int argc, char *argv[]){

    //On vérifie le nombre d'arguments
    if(argc < 6 ){ 
        std::cerr << "Error : Not enough arguments. Should be <host> <port> <user> <password> <tun> <state>" << std::endl;
        return -1;
    }

    //On récupère le chemin d'execution
    char resolved_path[PATH_MAX];
    char* dir_name;
    if (realpath(argv[0], resolved_path) != NULL) {
        dir_name = dirname(resolved_path);
    } else {
        std::cerr << "Error: Could not resolve path of executable" << std::endl;
        return 0;
    }

    //On créé un objet mosquitto
    MosquittoVPN* mosqui;
    try {
        std::cout << "Trying to connect to the Broker : " << argv[1] << " Port : " << argv[2] << " User : " << argv[3] << " Password : " << argv[4] << std::endl;
        mosqui = new MosquittoVPN(argv[1],std::stoi(argv[2]),argv[3],argv[4]);
    } catch (const std::exception& e) {
        std::cerr << "Caught exception: " << e.what() << std::endl;
        return 1;
    }

    //On écrit des fichiers JSON d'entrée
    Json::Value tun;
    tun["tun"] = argv[5];
    tun["status"] = argv[6];

    std::string tun_string = tun.toStyledString() ;

    //Envoi UNIQUE du JSON sur le topic correspondanty
    for(int i = 0; i < 3; i++){
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        std::cout << "Try to publish :  " << tun_string << std::endl;
        if (std::strcmp(argv[6], "up") == 0 ){
            mosqui->single_topic_pub("connect_tun",tun_string.c_str());
        }
        else if (std::strcmp(argv[6], "down") == 0){
            mosqui->single_topic_pub("disconnect_tun",tun_string.c_str());
        }
    }

    //Envoi REGULIER du JSON sur le topic correspondanty
    // if (std::strcmp(argv[6], "up") == 0 ){
    //     std::thread t1(&MosquittoVPN::add_topic_pub, mosqui,3,"connect_tun",tun_string.c_str());
    //      t1.join(); 
    // }
    // else if (std::strcmp(argv[6], "down") == 0){
    //     std::thread t1(&MosquittoVPN::add_topic_pub, mosqui,3,"disconnect_tun",tun_string.c_str());
    //      t1.join(); 
    // }
    

    //delete mosqui;
    return 0;

}