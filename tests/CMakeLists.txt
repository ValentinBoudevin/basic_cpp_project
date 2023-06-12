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

enable_testing()

add_executable(vpn_mosquitto_test vpn_mosquitto_test.cpp)

target_link_libraries(vpn_mosquitto_test
 PRIVATE
  GTest::GTest
  GTest::gtest_main
  GMock::GMock 
  ${MOSQUITTO_LIBRARY}
  vpn_mosquitto
  gcov)


file(GLOB OLD_XML_FILES ${CMAKE_SOURCE_DIR}/TestResults/*.xml)
foreach(XML_FILE ${OLD_XML_FILES})
    file(REMOVE ${XML_FILE})
endforeach()

#Use of threads
include(GoogleTest)
gtest_discover_tests(vpn_mosquitto_test
XML_OUTPUT_DIR "${CMAKE_SOURCE_DIR}/TestResults/"
)