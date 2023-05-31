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

int main(int argc, char *argv[]){

    MosquittoVPN* mosqui;

    if(argc == 2){
        try {
            mosqui = new MosquittoVPN(argv[1],argv[2]);
        } catch (const std::exception& e) {
            std::cerr << "Caught exception: " << e.what() << std::endl;
            return 1;
        }
    }
    else if(argc >= 4){ 
        try {
            std::cout << "Trying to connect to the Broker : " << argv[1] << " Port : " << argv[2] << " User : " << argv[3] << " Password : " << argv[4] << std::endl;
            mosqui = new MosquittoVPN(argv[1],std::stoi(argv[2]),argv[3],argv[4]);
        } catch (const std::exception& e) {
            std::cerr << "Caught exception: " << e.what() << std::endl;
            return 1;
        }
    }
    else{
        try {
            mosqui = new MosquittoVPN();
        } catch (const std::exception& e) {
            std::cerr << "Caught exception: " << e.what() << std::endl;
            return 1;
        }
    }

    char resolved_path[PATH_MAX];
    char* dir_name;
    if (realpath(argv[0], resolved_path) != NULL) {
        dir_name = dirname(resolved_path);
    } else {
        std::cerr << "Error: Could not resolve path of executable" << std::endl;
        return 0;
    }

    Json::Value up;
    Json::Value down;
    std::filesystem::path file_path(std::string(dir_name) + "/../data_up.json");
    std::filesystem::path file_paths(std::string(dir_name)  + "/../data_down.json");
    struct stat st;

    if (std::filesystem::exists(file_path)) {
        std::ifstream data_up(file_path, std::ifstream::binary);
        data_up >> up;
        std::cout<<up;

    } else {
        std::cout << file_path <<"does not exist." << std::endl;
    }

    if (std::filesystem::exists(file_paths)) {
        std::ifstream data_down(file_paths, std::ifstream::binary);
        data_down >> down;
        std::cout<<down;
    } else {
        std::cout << file_paths << "does nor exist" <<  std::endl;
    }

    std::string s = up.toStyledString() ;
    std::string s_down = down.toStyledString(); 
    
    mosqui->add_topic_sub("connected");
    mosqui->add_topic_sub("disconnected");

    std::thread t1(&MosquittoVPN::add_topic_pub, mosqui,4,"connected",s.c_str());
    std::thread t2(&MosquittoVPN::add_topic_pub, mosqui,6,"disconnected",s_down.c_str());



    t1.join(); 

    delete mosqui;
    return 0;
    
}
