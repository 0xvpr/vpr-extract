# Creator:    VPR
# Created:    February 20th, 2022
# Updated:    June 12th, 2023

FROM ubuntu:22.04

# Set env to avoid user input interruption during installation
ENV TZ=America/New_York
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

# Install normal goodies
RUN apt-get update && apt upgrade -y
RUN apt-get install -y --no-install-recommends \
    git \
    gcc \
    g++ \
    gdb \
    curl \
    wget \
    vim \
    tree \
    make \
    cmake \
    build-essential \
    mingw-w64 \
    python3-dev \
    libtbb-dev

# Move to working directory
ENV HOME=/root
WORKDIR /var/vpr-extract/
