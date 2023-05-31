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
    if(argc < 4){
        std::cout << "Error : not enough arguments. Should be <host> <port> <user> <password>" << std::endl;
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

    //On récupère les JSON envoyés par SlaveVPN
    mosqui->add_topic_sub("connect_tun");
    mosqui->add_topic_sub("disconnect_tun");
     
    //Boucle while() pour laisser le programme en attente d'évènements
    while (true)
    {
        /* code */
    }

    delete mosqui;
    return 0;

}