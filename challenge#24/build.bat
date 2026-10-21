@echo off
setlocal EnableExtensions DisableDelayedExpansion

rem Run from the directory containing this script, regardless of the caller's CWD.
pushd "%~dp0" || exit /b 1

if not exist "SOC24.cpp" (
    echo ERROR: SOC24.cpp not found next to build.bat. 1>&2
    goto :fail
)
if not exist "sub_build_linux.sh" (
    echo ERROR: sub_build_linux.sh not found next to build.bat. 1>&2
    goto :fail
)

rem Create all output directories before launching either toolchain.
for %%D in (msvc clang gcc) do (
    if not exist "%%D\" mkdir "%%D"
    if not exist "%%D\" goto :fail
)

rem Find VS2022 without launching a quoted path inside FOR /F / cmd /c.
rem Prefer Community (installed on Lucifer), then other VS2022 editions.
set "VSROOT="
for %%E in (Community Professional Enterprise BuildTools) do if not defined VSROOT if exist "%ProgramFiles%\Microsoft Visual Studio\2022\%%E\VC\Auxiliary\Build\vcvars64.bat" set "VSROOT=%ProgramFiles%\Microsoft Visual Studio\2022\%%E"
if not defined VSROOT (
    echo ERROR: Visual Studio 2022 with MSVC x64 tools not found. 1>&2
    goto :fail
)

rem Initialize the selected VS2022 toolchain even if a different cl.exe is on PATH.
call "%VSROOT%\VC\Auxiliary\Build\vcvars64.bat" >nul
if errorlevel 1 (
    echo ERROR: Failed to initialize Visual Studio 2022 MSVC x64. 1>&2
    goto :fail
)
set "MSVC_CL=%VCToolsInstallDir%bin\Hostx64\x64\cl.exe"
rem Check that vcvars64 did not reuse an environment from another VS version.
echo(%MSVC_CL% | findstr /I /B /L /C:"%VSROOT%\VC\Tools\MSVC" >nul
if errorlevel 1 (
    echo ERROR: Active compiler is not from the selected Visual Studio 2022 installation. 1>&2
    goto :fail
)
if not exist "%MSVC_CL%" (
    echo ERROR: VS2022 x64 cl.exe not found after initialization. 1>&2
    goto :fail
)

echo Visual Studio: %VSROOT%
echo Compiler:      %MSVC_CL%

:build_msvc
echo.
echo ============================================================
echo [1/2] Building MSVC x64 Release: msvc\SOC24.exe
echo ============================================================
"%MSVC_CL%" /nologo /std:c++14 /permissive- /O2 /GL /DNDEBUG /EHsc /MD /Wall /WX /diagnostics:caret /FC /Fo"msvc\SOC24.obj" /Fe"msvc\SOC24.exe" "SOC24.cpp" /link /LTCG /INCREMENTAL:NO
if errorlevel 1 (
    echo ERROR: MSVC compilation failed. Linux build skipped. 1>&2
    goto :fail
)

where wsl.exe >nul 2>&1
if errorlevel 1 (
    echo ERROR: wsl.exe not found. 1>&2
    goto :fail
)

echo.
echo ============================================================
echo [2/2] Building WSL Clang and GCC using sub_build_linux.sh
echo ============================================================
rem bash -e stops on a failed clang++ command: the supplied script has no set -e.
wsl.exe --exec bash -e ./sub_build_linux.sh
if errorlevel 1 (
    echo ERROR: WSL compilation failed. 1>&2
    goto :fail
)

echo.
echo SUCCESS: msvc\SOC24.exe, clang\SOC24 and gcc\SOC24
popd
exit /b 0

:fail
echo BUILD FAILED. 1>&2
popd
exit /b 1
