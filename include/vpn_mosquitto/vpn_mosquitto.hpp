#ifndef MosquittoVPN_hpp
#define MosquittoVPN_hpp

#include <iostream>
#include <unistd.h>
#include <mosquitto.h>
#include <vector>
#include <semaphore.h>
#include <thread>
#include <mutex>
#include <queue>
#include <stdexcept>

class MosquittoVPN {

    private :

        mosquitto *mosq;
        const char *host;
        const char *username;
        const char *password;
        std::mutex mosq_mutex;
        bool is_connected;

        static std::vector<const char*> topic_susbribed;
        static std::queue<std::pair<std::string, std::string>> disconnected_msg_queue;

        /**
         * @brief Function used to subscribe to some MQTT topics after a connection with a successful mosquitto_init()
         * 
         * @param mosq : the mosquitto instance making the callback.
         * @param obj : the user data provided in <mosquitto_new>
         * @param rc :  the return code of the connection response, one of:
         * 0 - success
         * 1 - connection refused (unacceptable protocol version)
         * 2 - connection refused (identifier rejected)
         * 3 - connection refused (broker unavailable)
         * 4-255 - reserved for future use
         */
        static void on_connect(struct mosquitto* mosq, void *obj, int rc);

        /**
         * @brief Function used to publisj to some MQTT topics after a connection with a successful mosquitto_init()
         * 
         * @param mosq : the mosquitto instance making the callback.
         * @param obj : the user data provided in <mosquitto_new>
         * @param message : the message data. This variable and associated memory will be freed by the library after the callback completes. The client should make copies of any of the data it requires.
         */
        static void on_message(struct mosquitto* mosq,void *obj, const struct mosquitto_message *message);


        static void on_message_receive(struct mosquitto* mosq,void *obj, const struct mosquitto_message *message);

        int mosquitto_username_pw_set(mosquitto *mosq, const char *username, const char *password);
        int mosquitto_tls_set(mosquitto *mosq, const char *cafile, const char *capath, const char *certfile, const char *keyfile, int (*pw_callback)(char *buf, int size, int rwflag, void *userdata));
        int mosquitto_tls_opts_set(mosquitto *mosq, int cert_reqs, const char *tls_version, const char *ciphers);
        int mosquitto_connect_async(mosquitto *mosq, const char *host, int port, int keepalive);

    public :

        /**
         * @brief Construct a new MosquittoVPN object which could be use with localhost without credentials
         */
        MosquittoVPN();

        /**
         * @brief Construct a new MosquittoVPN object which could be use with localhost with credentials
         * 
         * @param username : username use to communicate with the MQTT Broker
         * @param password : password use to communicate with the MQTT Broker
         */
        MosquittoVPN(const char* username,const char* password);

        /**
         * @brief Construct a new Mosquitto VPN object
         * 
         * @param host : domain name of the MQTT broker to contact
         * @param username : the username to send as a string
         * @param password : the password to send as a string
         * @param port : the port used as a int
         */
        MosquittoVPN(const char* host,int port,const char* username,const char* password);

        /**
         * @brief Destroy the MosquittoVPN object
         * 
         */
        ~MosquittoVPN();

        /**
         * @brief Add a topic to subscription once the MQTT connection will be init
         * 
         * @param topic_name : name of the topic to subscribe
         */
        void add_topic_sub(const char* topic_name);

        /**
         * @brief Fonction permettant d'envoyer des messages à un MQTT Broker
         * 
         * @param time : durée entre deux envois sur un topic
         * @param topic : nom du topic à envoyer
         * @param msg : message à envoyer sur le topic
         */
        void add_topic_pub(int time, const char* topic, const char* msg);

        void single_topic_pub(const char* topic, const char* msg);

        /**
         * @brief Get the mosq object
         * 
         * @return mosquitto* : return the mosq object which can be use to call spefic methods from official c++ mosquitto library
         */
        mosquitto* get_mosq();

        /**
         * @brief Get the password object
         * 
         * @return const char* : the password configured which will use for mosquitto_init()
         */
        const char* get_password();

        /**
         * @brief Get the username object
         * 
         * @return const char* : the username configured which will use for mosquitto_init()
         */
        const char* get_username();

        /**
         * @brief Get the is connected object
         * 
         * @return true : is well connected
         * @return false : is not connected
         */
        bool get_is_connected();
        
        /**
         * @brief Return the topic setup to be used by Mosquitto
         * 
         * @return std::vector<const char*> : vector of topics
         */
        std::vector<const char*>  get_topic();

        /**
         * @brief Set the hostname to connect with the remote broker
         * 
         * @param host : the host to send as a string.
         */
        void set_host(const char* host);

        /**
         * @brief Set the password used to connect to the remote MQTT Broker
         * 
         * @param username : the username to send as a string, or NULL to disable authentication.
         * @param password : the password to send as a string. Set to NULL when username is valid in order to send just a username.
         * 
         */
        void set_account(const char* user,const char* password);

};

#endif