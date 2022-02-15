ARG VARIANT="buster"
FROM debian:${VARIANT}

# Install dependencies
RUN apt-get -qq update; \
    apt-get install -qqy --no-install-recommends \
        libstdc++-7-dev clang libboost-all-dev; \
    rm -rf /var/lib/apt/lists/*

# These commands copy your files into the specified directory in the image
# and set that as the working location

RUN mkdir -p /usr/src/pifromcoprimes
WORKDIR /usr/src/pifromcoprimes
COPY main.cpp main.cpp

# This command compiles your app using GCC, adjust for your source code
RUN clang++ -pthread -std=c++17 -O3 -o main main.cpp

# This command runs your application, comment out this line to compile only
CMD ["bash", "-c", "./main"]

LABEL Name=pifromcoprimes Version=0.0.1
