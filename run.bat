@echo off
setlocal enabledelayedexpansion

set "docker_build=docker build docker/ -t cpp_rust_perf"
set "docker_run=docker run --privileged -it --rm -v %cd%:/src cpp_rust_perf"

if "%1"=="build" (
    echo Executing: !docker_build!
    !docker_build!
    goto :eof
)

if "%1"=="rebuild" (
    set "rebuild_cmd=docker build docker/ -t cpp_rust_perf --no-cache"
    echo Executing: !rebuild_cmd!
    !rebuild_cmd!
    goto :eof
)

if "%1"=="shell" (
    echo Executing: !docker_run! shell
    !docker_run! shell
    goto :eof
)

if "%1"=="versions" (
    echo Executing: !docker_run! versions
    !docker_run! versions
    goto :eof
)

if "%1"=="" (
    echo Usage: run.bat [command]
    echo Commands:
    echo   build         - Build the main Docker image
    echo   rebuild       - Rebuild the main Docker image without cache
    echo   shell         - Start interactive shell in main container
    echo   versions      - Show versions in main container
    goto :eof
)

echo Unknown command: %1