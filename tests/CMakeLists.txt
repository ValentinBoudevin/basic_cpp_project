include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.11.0
)
FetchContent_MakeAvailable(googletest)
add_library(GTest::GTest INTERFACE IMPORTED)
target_link_libraries(GTest::GTest INTERFACE gtest_main)

add_library(GMock::GMock INTERFACE IMPORTED)
target_link_libraries(GMock::GMock INTERFACE gmock_main)

#Use of the Mosquitto library
find_library(MOSQUITTO_LIBRARY NAMES mosquitto PATHS /usr/lib/x86_64-linux-gnu)
link_directories(/usr/lib/x86_64-linux-gnu)

add_executable(vpn_mosquitto_test vpn_mosquitto_test.cpp)

target_link_libraries(vpn_mosquitto_test
 PRIVATE
  GTest::GTest
  GMock::GMock 
  ${MOSQUITTO_LIBRARY}
  vpn_mosquitto
  gcov)

#Use of threads
include(GoogleTest)
gtest_discover_tests(vpn_mosquitto_test
  PROPERTIES
  XML_OUTPUT_DIR "${CMAKE_BINARY_DIR}/TestResults"
  XML_OUTPUT_NAME "TestResults.xml")