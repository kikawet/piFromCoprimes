ARG VARIANT="3.14"
FROM alpine:${VARIANT}

# Install dependencies
# libstdc++-7-dev
RUN apk update && apk add --no-cache libstdc++ g++ boost-dev  

# These commands copy your files into the specified directory in the image
# and set that as the working location

RUN mkdir -p /usr/src/pifromcoprimes
WORKDIR /usr/src/pifromcoprimes
COPY main.cpp main.cpp

# This command compiles your app using GCC, adjust for your source code
RUN g++ -O3 -Wall -Wextra -pedantic -std=c++17 -o main main.cpp -pthread -lboost_thread -lboost_chrono -lboost_random

# This command runs your application, comment out this line to compile only
CMD ["/bin/sh", "-c", "./main"]

LABEL Name=pifromcoprimes Version=0.0.1
