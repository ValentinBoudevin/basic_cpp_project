#!/usr/bin/env bash

set -euo pipefail

SCRIPT_DIR="$(dirname "$(realpath "$0")")"
PROJECT_DIR="$(dirname "${SCRIPT_DIR}")"
BUILD_DIR="${PROJECT_DIR}/build"
COVERAGE_DIR="${BUILD_DIR}/src"

cmake -S "${PROJECT_DIR}" \
  -B "${BUILD_DIR}" \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
  -DCMAKE_BUILD_TYPE=Debug

cmake --build ${BUILD_DIR} -j4
# Run tests
cd ${BUILD_DIR}
#GTEST_COLOR=1 ctest --verbose
${BUILD_DIR}/tests/vpn_mosquitto_test --gtest_output=xml:../TestResults/

cmake --build ${BUILD_DIR} --target coverage