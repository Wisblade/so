@echo off
setlocal EnableExtensions

rem Run from the directory containing this script, regardless of the caller's CWD.
cd /d "%~dp0" || exit /b 1

set "EXE=.\x64\Release\SOC24.exe"
set "RESULTS=results.txt"

if not exist "%EXE%" (
    echo [ERROR] Missing executable: %EXE% 1>&2
    exit /b 1
)

rem Windows always starts a fresh results file. Linux will append to it.
> "%RESULTS%" echo SOC24 - consolidated benchmark results
if errorlevel 1 exit /b 1

for %%F in (
    poker_hands.json
    SOC24_SAMPLE_100000_hands.json
    SOC24_ALL_2598960_hands.json
) do (
    call :run "%%F"
    if errorlevel 1 exit /b 1
)

echo [MSVC] All 3 runs completed. Output: "%RESULTS%"

wsl.exe --exec bash ./sub_run_linux.sh

exit /b 0

:run
if not exist "%~1" (
    echo [ERROR] Missing input: %~1 1>&2
    exit /b 1
)

echo [MSVC] Running %~1 ...
>> "%RESULTS%" echo.
>> "%RESULTS%" echo ==================== MSVC - %~1 ====================
"%EXE%" "%~1" >> "%RESULTS%" 2>&1
if errorlevel 1 (
    echo [ERROR] Failed: %~1. See "%RESULTS%". 1>&2
    exit /b 1
)
echo [MSVC] Completed %~1
exit /b 0
