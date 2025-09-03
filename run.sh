#!/bin/sh -xe

docker_build="docker build docker/ -t cpp_rust_perf"

docker_run="docker run --privileged -it --rm -v $PWD:/src cpp_rust_perf"

build() {
    eval "$docker_build"
}

rebuild() {
    eval "$docker_build --no-cache"
}

shell() {
    eval "$docker_run shell"
}

versions() {
    eval "$docker_run versions"
}

"$@"