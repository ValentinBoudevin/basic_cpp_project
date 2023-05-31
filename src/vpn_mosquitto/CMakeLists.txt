add_library(vpn_mosquitto vpn_mosquitto.cpp)

find_library(MOSQUITTO_LIBRARY NAMES mosquitto PATHS /usr/lib/x86_64-linux-gnu)
link_directories(/usr/lib/x86_64-linux-gnu)

find_package(jsoncpp REQUIRED)

target_link_libraries(vpn_mosquitto 
    ${MOSQUITTO_LIBRARY}
    gcov
    jsoncpp)


