#include <iostream>
#include <unistd.h>
#include <mosquitto.h>
#include <thread>   
#include <vpn_mosquitto/vpn_mosquitto.hpp>
#include <functional>
#include <cstring>
#include <queue>
#include <jsoncpp/json/json.h>
#include <sstream>
#include <fstream>

using namespace std;

std::vector<const char*> MosquittoVPN::topic_susbribed;
std::queue<std::pair<std::string, std::string>> MosquittoVPN::disconnected_msg_queue;

/////////////////////////// - CONSTRUCTORS - ///////////////////////////

MosquittoVPN::MosquittoVPN(){
    std::lock_guard<std::mutex> lock(this->mosq_mutex);
    mosquitto_lib_init();
    this->mosq = mosquitto_new(NULL, true, NULL);
    if (!mosq) {
        throw std::runtime_error("Error function : mosquitto_new | Wans't able to create a new moquitto instance" );
    }

    int result = 0;

    mosquitto_connect_callback_set(this->mosq, MosquittoVPN::on_connect);
    mosquitto_message_callback_set(this->mosq, MosquittoVPN::on_message);
    // mosquitto_message_callback_set(this->mosq, MosquittoVPN::on_message_receive);

    result = this->mosquitto_connect_async(this->mosq, "localhost", 1883, 60);
    if (result != MOSQ_ERR_SUCCESS) {
        this->~MosquittoVPN();
        throw std::runtime_error("Error function : mosquitto_connect_async | Code : " + result);
    }

    mosquitto_loop_start(mosq);

    time_t start_time = time(NULL);
    bool configured = false;
    while (difftime(time(NULL), start_time) < 3) {
        if (mosquitto_want_write(mosq)) {
            std::cout << "Success : Broker Correctly configured" << std::endl;
            configured = true;
            break;
        }
    }
    if(!configured){
        throw std::runtime_error("Error : Broker doesn't respond");
    }

    topic_susbribed = {};
    this->is_connected = true;
    this->password = "";
    this->username = "";
    this->host = "";
}

MosquittoVPN::MosquittoVPN(const char* username,const char* password){
    std::lock_guard<std::mutex> lock(this->mosq_mutex);
    this->username = username;
    this->password = password;
    this->host = "";
    mosquitto_lib_init();
    this->mosq = mosquitto_new(NULL, true, NULL);

    int result = 0;

    if (!mosq) {
        throw std::runtime_error("Error function : mosquitto_new | Wans't able to create a new moquitto instance" );
    }

    result = mosquitto_username_pw_set(this->mosq, this->username, this->password);
    if (result != MOSQ_ERR_SUCCESS) {
        this->~MosquittoVPN();
        throw std::runtime_error("Error function : mosquitto_username_pw_set | Code : " + result);
    }

    mosquitto_connect_callback_set(this->mosq, MosquittoVPN::on_connect);
    mosquitto_message_callback_set(this->mosq, MosquittoVPN::on_message);

    result = this->mosquitto_connect_async(this->mosq, "localhost", 1883, 60);
    if (result != MOSQ_ERR_SUCCESS) {
        this->~MosquittoVPN();
        throw std::runtime_error("Error function : mosquitto_connect_async | Code : " + result);
    }

    mosquitto_loop_start(mosq);

    time_t start_time = time(NULL);
    bool configured = false;
    while (difftime(time(NULL), start_time) < 3) {
        if (mosquitto_want_write(mosq)) {
            std::cout << "Success : Broker Correctly configured" << std::endl;
            configured = true;
            break;
        }
    }
    if(!configured){
        throw std::runtime_error("Error : Broker doesn't respond");
    }
    topic_susbribed = {};
    this->is_connected = true;
}

MosquittoVPN::MosquittoVPN(const char* host,int port,const char* username,const char* password){
    std::lock_guard<std::mutex> lock(this->mosq_mutex);
    this->host = host;
    this->username = username;
    this->password = password;
    mosquitto_lib_init();
    this->mosq = mosquitto_new(NULL, true, NULL);

    int result = 0;

    if (!mosq) {
        throw std::runtime_error("Error function : mosquitto_new | Wans't able to create a new moquitto instance" );
    }

    result = mosquitto_username_pw_set(this->mosq, this->username, this->password);
    if (result != MOSQ_ERR_SUCCESS) {
        this->~MosquittoVPN();
        throw std::runtime_error("Error function : mosquitto_username_pw_set | Code : " + result);
    }

    result = this->mosquitto_tls_set(this->mosq, NULL, "/etc/ssl/certs/", NULL, NULL, NULL);
    if( result != MOSQ_ERR_SUCCESS){
        this->~MosquittoVPN();
        throw std::runtime_error("Error function : mosquitto_tls_set | Code : " + result);
    }

    result = this->mosquitto_tls_opts_set(this->mosq, 1, "tlsv1.2", NULL);
    if( result != MOSQ_ERR_SUCCESS){
        this->~MosquittoVPN();
        throw std::runtime_error("Error function : mosquitto_tls_opts_set | Code : " + result);
    }

    mosquitto_connect_callback_set(this->mosq, MosquittoVPN::on_connect);
    //mosquitto_message_callback_set(this->mosq, MosquittoVPN::on_message);
    mosquitto_message_callback_set(this->mosq, MosquittoVPN::on_message);

    result = this->mosquitto_connect_async(this->mosq, this->host, port, 60);
    if (result != MOSQ_ERR_SUCCESS) {
        this->~MosquittoVPN();
        throw std::runtime_error("Error function : mosquitto_connect_async | Code : " + result);
    }

    mosquitto_loop_start(mosq);

    time_t start_time = time(NULL);
    bool configured = false;
    while (difftime(time(NULL), start_time) < 3) {
        if (mosquitto_want_write(mosq)) {
            std::cout << "Success : Broker Correctly configured" << std::endl;
            configured = true;
            break;
        }
    }
    if(!configured){
        throw std::runtime_error("Error : Broker doesn't respond");
    }

    topic_susbribed = {};
    this->is_connected = true;
}

/////////////////////////// - DESTRUCTORS - ///////////////////////////

MosquittoVPN::~MosquittoVPN(){
    mosquitto_disconnect(this->mosq);
    ::mosquitto_destroy(this->mosq);
    mosquitto_lib_cleanup();
    this->host = nullptr;
    this->username = nullptr;
    this->password = nullptr;
    this->mosq_mutex.~mutex();
}

/////////////////////////// - INTERRUPTORS - ///////////////////////////
/*
rc - the return code of the connection response, one of:

* 0 - success
* 1 - connection refused (unacceptable protocol version)
* 2 - connection refused (identifier rejected)
* 3 - connection refused (broker unavailable)
* 4-255 - reserved for future use
*/
void MosquittoVPN::on_connect(struct mosquitto *mosq, void *obj, int rc) {
    if (rc == 0) {
        for(const char* topic : MosquittoVPN::topic_susbribed){
            std::cout << "Subscribed to the topic : " << topic << std::endl;
            mosquitto_subscribe(mosq, nullptr, topic, 0);
        }
    }
    else {
        std::cerr << "Failed to connect to broker" << std::endl;
    }
    while (!disconnected_msg_queue.empty()) {
        std::pair<std::string, std::string> message = disconnected_msg_queue.front();
        disconnected_msg_queue.pop();
        mosquitto_publish(mosq, nullptr, message.first.c_str(), message.second.size(), message.second.c_str(), 0, false);
    }
}

void MosquittoVPN::on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *message) {
    std::cout << "Received message: " << static_cast<char*>(message->payload) << std::endl;
    try {
        std::ofstream outfile("/home/boudevin/Documents/vpn_mosquitto/test.log", std::ios_base::app); // Open the file in append mode
        outfile << static_cast<char*>(message->payload) << std::endl; // Write the string to the file
        outfile.close(); // Close the file
    } catch (const std::exception& e) {
        std::cerr << "Caught exception: " << e.what() << std::endl;
    }
}

void MosquittoVPN::on_message_receive(struct mosquitto *mosq, void *obj, const struct mosquitto_message *message) {
    const char* data = static_cast<char*>(message->payload);
    Json::Value root;
    Json::CharReaderBuilder builder;
    std::string err;
    ifstream file("/home/lyessphinx/sphinxIotlabs/vpn_mosquitto/vpn_log.json");
    Json::Value vpnlog;
    file >> vpnlog;


    std::stringstream ss(data); // create a stringstream object to wrap the data string

    if (!Json::parseFromStream(builder, ss, &root, &err)) {
        std::cout << "Failed to parse JSON string: " << err << std::endl;
        return;
    }

    std::cout << "received message status of tunnel : " << root["tun"].asString() << " is : " << root["status"].asString() << std::endl;

    for (auto& obj : vpnlog) {
        if (obj["tun"] == root["tun"].asString()) {
            // Modification du "status" de l'objet "tun"
            obj["status"] = root["status"].asString() ;
        }
    }

    ofstream outfile("/home/lyessphinx/sphinxIotlabs/vpn_mosquitto/vpn_log.json");
    outfile << vpnlog;
    outfile.close();
}


/////////////////////////// - FUNCTIONS - ///////////////////////////

void MosquittoVPN::add_topic_pub(int time, const char* topic, const char* msg){
    int result = 0;
    while (true) {
        if(this->is_connected){
            this->mosq_mutex.lock();
            result = mosquitto_publish(this->mosq, nullptr, topic, strlen(msg), msg, 0, false);
            this->mosq_mutex.unlock();
            if (result != MOSQ_ERR_SUCCESS) {
                std::cerr << "Error function : mosquitto_publish | Code : " << result << " | Error message : " << mosquitto_strerror(result) << std::endl;
                disconnected_msg_queue.push(std::make_pair(topic, msg));
            }
            else{
                std::cout << "Succes : Publish " << std::string(msg) << "on the topic " << std::string(topic) << std::endl;
            }
        }
        else{
            disconnected_msg_queue.push(std::make_pair(topic, msg));
        }
        std::this_thread::sleep_for(std::chrono::seconds(time));
    }
}

void MosquittoVPN::single_topic_pub(const char* topic, const char* msg){
    int result = 0;
    if(this->is_connected){
        this->mosq_mutex.lock();
        result = mosquitto_publish(this->mosq, nullptr, topic, strlen(msg), msg, 0, false);
        this->mosq_mutex.unlock();
        if (result != MOSQ_ERR_SUCCESS) {
            std::cerr << "Error function : mosquitto_publish | Code : " << result << " | Error message : " << mosquitto_strerror(result) << std::endl;
            disconnected_msg_queue.push(std::make_pair(topic, msg));
        }
        else{
            std::cout << "Succes : Publish " << std::string(msg) << "on the topic " << std::string(topic) << std::endl;
        }
    }
    else{
        disconnected_msg_queue.push(std::make_pair(topic, msg));
    }
}

void MosquittoVPN::add_topic_sub(const char* topic_name){
    topic_susbribed.push_back(topic_name);
    // std::cout << "Subscribed to the topic : " << topic_name << std::endl;
    // mosquitto_subscribe(mosq, nullptr, topic_name, 0);
}

/////////////////////////// - SETTERS - ///////////////////////////

void MosquittoVPN::set_host(const char* host){
    this->host = host;
}

void MosquittoVPN::set_account(const char* user,const char* password){
    this->username = user;
    this->password = password;
}

/////////////////////////// - GETTERS - ///////////////////////////

std::vector<const char*>  MosquittoVPN::get_topic(){
    return this->topic_susbribed;
}

mosquitto* MosquittoVPN::get_mosq(){
    return this->mosq;
}

const char* MosquittoVPN::get_username(){
    return this->username;
}

const char* MosquittoVPN::get_password(){
    return this->password;
}

bool MosquittoVPN::get_is_connected(){
    return this->is_connected;
}
/////////////////////////// - REIMPLEMENT MOSQUITTO LIBRARY FUNCTION IN CLASS - ///////////////////////////

int MosquittoVPN::mosquitto_username_pw_set(mosquitto *mosq, const char *username, const char *password){
    return ::mosquitto_username_pw_set(mosq,username,password);
}

int MosquittoVPN::mosquitto_tls_set(mosquitto *mosq, const char *cafile, const char *capath, const char *certfile, const char *keyfile, int (*pw_callback)(char *buf, int size, int rwflag, void *userdata)){
    return ::mosquitto_tls_set(mosq,cafile, capath, certfile, keyfile, pw_callback);
}

int MosquittoVPN::mosquitto_tls_opts_set(mosquitto *mosq, int cert_reqs, const char *tls_version, const char *ciphers){
    return ::mosquitto_tls_opts_set(mosq, cert_reqs, tls_version, ciphers);
}

int MosquittoVPN::mosquitto_connect_async(mosquitto *mosq, const char *host, int port, int keepalive){
    return ::mosquitto_connect_async(mosq, host, port, keepalive);
}