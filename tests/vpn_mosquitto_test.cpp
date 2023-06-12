#include <vpn_mosquitto/vpn_mosquitto.hpp>
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <iostream>
#include <unistd.h>
#include <thread>   
#include <cstring>

using ::testing::AtLeast;                        
using ::testing::_;
using ::testing::Invoke;
using ::testing::Return;

// class MockMosquittoVPN : public MosquittoVPN {
// public:
//     MOCK_METHOD(void, on_connect, (struct mosquitto* mosq, void *obj, int rc), ());
//     MOCK_METHOD(void, on_message, (struct mosquitto* mosq, void *obj, const struct mosquitto_message *message), ());
//     MOCK_METHOD(void, mosquitto_alive, (), ());
//     MOCK_METHOD(void, add_topic_sub, (const char* topic_name), ());
//     MOCK_METHOD(void, add_topic_pub, (int time, const char* topic, const char* msg), ());
//     MOCK_METHOD(mosquitto*, get_mosq, (), ());
//     MOCK_METHOD(const char*, get_password, (), ());
//     MOCK_METHOD(const char*, get_username, (), ());
//     MOCK_METHOD(void, set_host, (const char* host), ());
//     MOCK_METHOD(void, set_account, (const char* user, const char* password), ());
//     MOCK_METHOD3(mosquitto_username_pw_set, int(struct mosquitto *mosq, const char *username, const char *password));    MOCK_METHOD6(mosquitto_tls_set, int(mosquitto *mosq, const char *cafile, const char *capath, const char *certfile, const char *keyfile, int (*pw_callback)(char *buf, int size, int rwflag, void *userdata)));
//     MOCK_METHOD4(mosquitto_tls_opts_set, int(mosquitto *mosq, int cert_reqs, const char *tls_version, const char *ciphers));
//     MOCK_METHOD4(mosquitto_connect, int(mosquitto *mosq, const char *host, int port, int keepalive));

// };

// TEST(constructor_1, constructor_1){
//     MosquittoVPN* mosqui = new MosquittoVPN();
//     ASSERT_TRUE(mosqui->get_mosq() != nullptr);
// };

// TEST(MosquittoVPN, constructor_2){
//     MosquittoVPN* mosqui = new MosquittoVPN("sphinx","oxilabs");
//     ASSERT_TRUE(std::strcmp(mosqui->get_username() , "sphinx") == 0);
// };

TEST(MosquittoVPN, constructor_3_sucessed){
    MosquittoVPN* mosqui = new MosquittoVPN("mqtt01.iotvpn.io",8883,"sphinx","oxilabs");
    ASSERT_TRUE(std::strcmp(mosqui->get_username() , "sphinx") == 0);
};

TEST(MosquittoPN, destructor_1){
    MosquittoVPN* mosqui = new MosquittoVPN("mqtt01.iotvpn.io",8883,"sphinx","oxilabs");
    delete mosqui;
    EXPECT_EQ(mosqui->get_password(), nullptr);
};

// TEST(MosquittoVPN, get_password_ok){
//     MosquittoVPN* mosqui = new MosquittoVPN("mqtt01.iotvpn.io",8883,"sphinx","oxilabs");
//     ASSERT_TRUE(mosqui->get_password() == "oxilabs");
// };

// // TEST(constructor_3, constructor_3_fail_password) {
// //   // Create a mock object for MosquittoVPN class
// //   MockMosquittoVPN mock = MockMosquittoVPN();
  
// //   // Set the expectation for mosquitto_username_pw_set() function
// //   EXPECT_CALL(mock, mosquitto_username_pw_set(_, _, _))
// //     .Times(1)
// //     .WillOnce(Return(MOSQ_ERR_SUCCESS)); // Return value of MOSQ_ERR_SUCCESS to indicate success

// //   mock = MockMosquittoVPN();

// //   // Verify the result is MOSQ_ERR_SUCCESS
// //   ASSERT_TRUE(mock->get_is_connected());
// // }


// // TEST(MosquittoVPN, get_username_ok){
// //     MosquittoVPN* mosqui = new MosquittoVPN("mqtt01.iotvpn.io",8883,"sphinx","oxilabs");
// //     ASSERT_TRUE(mosqui->get_username() == "sphinx");
// // };

// // /*
// // TEST(MosquittoVPN, get_password_null){
// //     MosquittoVPN* mosqui = new MosquittoVPN();
// //     ASSERT_TRUE(std::strcmp(mosqui->get_password() , "") == 0);
// // };

// // TEST(MosquittoVPN, get_username_null){
// //     MosquittoVPN* mosqui = new MosquittoVPN();
// //     ASSERT_TRUE(std::strcmp(mosqui->get_username() , "") == 0);
// // };
// // */


// // TEST(MosquittoVPN, get_mosq){
// //     MosquittoVPN* mosqui = new MosquittoVPN("mqtt01.iotvpn.io",8883,"sphinx","oxilabs");
// //     EXPECT_NE(mosqui->get_mosq(),nullptr);
// // };

// // TEST(MosquittoVPN, get_topic_empty){
// //     MosquittoVPN* mosqui = new MosquittoVPN("mqtt01.iotvpn.io",8883,"sphinx","oxilabs");
// //     std::vector<const char *> topics = mosqui->get_topic();
// //     ASSERT_TRUE(topics.size() == 0);
// // };

// // TEST(MosquittoVPN, add_topic_sub){
// //     MosquittoVPN* mosqui = new MosquittoVPN("mqtt01.iotvpn.io",8883,"sphinx","oxilabs");
// //     mosqui->add_topic_sub("test");
// //     std::vector<const char *> topics = mosqui->get_topic();
// //     ASSERT_TRUE(topics.size() == 1);
// //     ASSERT_TRUE(topics.front() == "test");
// // };

// // TEST(MosquittoVPN, connect_fail_tls_set){
// //     MockMosquitto mock;
// //     EXPECT_CALL(mock, mosquitto_username_pw_set(_, _, _))
// //         .WillOnce(testing::Return(MOSQ_ERR_SUCCESS));
// //     EXPECT_CALL(mock, mosquitto_tls_set(_, _, _, _, _, _))
// //         .WillOnce(testing::Return(MOSQ_ERR_INVAL));
// //     EXPECT_CALL(mock, mosquitto_init())
// //         .WillOnce(testing::Invoke(&mock, &MockMosquitto::mosquitto_init_impl)); // call the real mosquitto_init() method
// //     int result = mock.mosquitto_init();
// //     EXPECT_EQ(result, MOSQ_ERR_INVAL);
// // }

// // TEST(MosquittoVPN, connect_fail_tls_opts_set){
// //     MockMosquitto mock_mosquitto;
// //     EXPECT_CALL(mock_mosquitto, mosquitto_username_pw_set(mock_mosquitto.get_mosq(),"sphinx","oxilabs")).WillOnce(testing::Return(MOSQ_ERR_INVAL));
// //     EXPECT_CALL(mock_mosquitto, mosquitto_init()).WillOnce(testing::Invoke([&mock_mosquitto](){
// //         return mock_mosquitto.mosquitto_username_pw_set(mock_mosquitto.get_mosq(),"sphinx","oxilabs");
// //     }));
// //     int result = mock_mosquitto.mosquitto_init();
// //     ASSERT_EQ(result, MOSQ_ERR_INVAL);
// // };

// int main(int argc, char **argv){
//     testing::InitGoogleTest(&argc,argv);
//     return RUN_ALL_TESTS();
// }
