#!/bin/bash
clang++ -v -std=c++11 -O2 -DNDEBUG -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion -Werror -pthread SOC24.cpp -o ./clang/SOC24
g++ -v -std=c++11 -O2 -DNDEBUG -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion -Werror -pthread SOC24.cpp -o ./gcc/SOC24
