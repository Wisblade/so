# Coding Challenge #24 - Don't Fold

A standalone C++ solution to [Stack Overflow Coding Challenge #24: Don't Fold](https://stackoverflow.com/beta/challenges/80010077/challenge-24-dont-fold).

The program builds a lookup table covering every possible five-card poker hand, looks up the hands from the input file, and accumulates their scores.

**One C++ source file. No third-party library dependencies.**

## Build

Tested on Windows x64 with MSVC 19.44 (Visual Studio 2022 Community), and on Ubuntu 26.04 under WSL with GCC 15.2 and Clang 21.1.8.

From Windows, run `build.bat` to build all three versions (Visual Studio 2022 and WSL with the corresponding Linux compilers installed).

Alternatively, compile each version separately using the commands documented in `SOC24.cpp`.

## Run

```bat
msvc\SOC24.exe poker_hands.json
```

```sh
./gcc/SOC24 poker_hands.json
./clang/SOC24 poker_hands.json
```

Expected result for the original challenge input: **82 hands, 3248 points**.

## Additional test data

`SOC24.7z` contains optional larger JSON datasets, including an exhaustive set of all **2,598,960** possible five-card hands.

The archive is tracked with Git LFS. If you cloned the repository without downloading LFS objects, run `git lfs pull` before extracting it. An archive extractor supporting 7z is needed only to unpack the optional datasets.

Build instructions and Doxygen documentation are also included directly in `SOC24.cpp`.
