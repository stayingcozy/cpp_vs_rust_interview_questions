FROM debian:trixie-20250811-slim

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    clang-19 \
    cmake \
    curl \
    git \
    libc++-19-dev \
    libc++abi-19-dev \
    libssl-dev \
    vim \
    powercap-utils \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/*

RUN update-alternatives --install /usr/bin/clang++ clang++ /usr/bin/clang++-19 10 \
    && useradd -m dev

RUN mkdir /root/bin
WORKDIR /root/bin
SHELL ["/bin/bash", "-o", "pipefail", "-c"]
ENV PATH="/root/bin/:/root/.local/bin/:${PATH}"

# https://www.rust-lang.org/tools/install
ENV CARGO_HOME="/root/bin/.cargo" PATH="/root/bin/.cargo/bin:${PATH}"
RUN curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh -s -- -y

ARG SCRIPTS_VER=unknown
COPY *.cpp ./

RUN clang++ -o versions versions.cpp
RUN ./versions

RUN clang++ -o run run.cpp
ENTRYPOINT ["./run"]

RUN cat /etc/os-release