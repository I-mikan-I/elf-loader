FROM ubuntu:mantic

# ********************************************************
# * Anything else you want to do like clean up goes here *
# ********************************************************

# [Optional] Set the default user. Omit if you want to keep the default as root.

RUN apt update -y && apt upgrade -y
RUN apt install -y binutils vim curl clang-17 ca-certificates gpg wget sudo passwd ninja-build clang-tools-17 gdb clang-format-17 less

RUN test -f /usr/share/doc/kitware-archive-keyring/copyright || \
    wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc 2>/dev/null | gpg --dearmor - | tee /usr/share/keyrings/kitware-archive-keyring.gpg >/dev/null

RUN echo 'deb [signed-by=/usr/share/keyrings/kitware-archive-keyring.gpg] https://apt.kitware.com/ubuntu/ jammy main' | tee /etc/apt/sources.list.d/kitware.list >/dev/null \
    apt-get update

RUN echo "ubuntu:1" | chpasswd

RUN apt update -y
RUN apt install cmake -y

USER 1000