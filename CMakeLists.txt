cmake_minimum_required(VERSION 3.10)
project(MosquittoVPN LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -fprofile-arcs -ftest-coverage")

enable_testing()

include_directories(include)
add_subdirectory(src/vpn_mosquitto/)
add_subdirectory(tests)

#Use of the Mosquitto library
find_library(MOSQUITTO_LIBRARY NAMES mosquitto PATHS /usr/lib/x86_64-linux-gnu)
link_directories(/usr/lib/x86_64-linux-gnu)

find_package(jsoncpp REQUIRED)

add_executable(MosquittoExample src/vpn_main.cpp)

target_link_libraries(MosquittoExample 
    ${MOSQUITTO_LIBRARY} 
    vpn_mosquitto
    gcov
    jsoncpp)

add_executable(mqtt_slave src/vpn_main_slave.cpp)

target_link_libraries(mqtt_slave 
    ${MOSQUITTO_LIBRARY} 
    vpn_mosquitto
    jsoncpp)


add_executable(mqtt_master src/vpn_main_master.cpp)

target_link_libraries(mqtt_master
    ${MOSQUITTO_LIBRARY} 
    vpn_mosquitto
    jsoncpp)

#Generate coverage files
add_custom_target(coverage
    # Run the tests to generate the coverage data
    COMMAND ${CMAKE_CTEST_COMMAND} -R vpn_mosquitto_test

    # Generate the coverage report using gcovr for the specific files
    COMMAND gcovr --root=${CMAKE_SOURCE_DIR} --html --html-details
            -o ${CMAKE_BINARY_DIR}/coverage.html
            ${CMAKE_SOURCE_DIR}/src/vpn_mosquitto/vpn_mosquitto.cpp
            ${CMAKE_SOURCE_DIR}/tests/vpn_mosquitto_test.cpp
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}

    # Output some statistics
    COMMAND lcov --directory ${CMAKE_BINARY_DIR}/src/vpn_mosquitto/CMakeFiles/vpn_mosquitto.dir --capture --output-file ${CMAKE_BINARY_DIR}/coverage.info

    COMMAND lcov --remove ${CMAKE_BINARY_DIR}/coverage.info "/usr*" -o ${CMAKE_BINARY_DIR}/coverage.info
    COMMAND lcov --remove ${CMAKE_BINARY_DIR}/coverage.info "*/tests/*" -o ${CMAKE_BINARY_DIR}/coverage.info
    COMMAND lcov --remove ${CMAKE_BINARY_DIR}/coverage.info "/10/*" -o ${CMAKE_BINARY_DIR}/coverage.info
    
    # Generate the coverage report using genhtml
    COMMAND mkdir -p ${CMAKE_BINARY_DIR}/coverage_html
    COMMAND genhtml ${CMAKE_BINARY_DIR}/coverage.info --output-directory ${CMAKE_BINARY_DIR}/coverage_html

    # Output some statistics
    COMMAND echo "Coverage report generated at: file://${CMAKE_BINARY_DIR}/coverage_html/index.html"
)

#Generate datasheet
# Check if doxygen is installed
find_package(Doxygen)
if(DOXYGEN_FOUND)
    # Set the input and output files for doxygen
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/../Doxyfile)
    
    # Configure the doxygen input file
    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
    
    message("Doxygen build started")

    # note the option ALL which allows to build the docs together with the application
    add_custom_target( doc_doxygen ALL
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM )
else (DOXYGEN_FOUND)
  message("Doxygen need to be installed to generate the doxygen documentation")
endif (DOXYGEN_FOUND)