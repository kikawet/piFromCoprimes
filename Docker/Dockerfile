ARG VARIANT="3.20"
FROM alpine:${VARIANT}

# Install dependencies
RUN apk update && apk add --no-cache g++ boost-dev cmake make

# These commands copy your files into the specified directory in the image
# and set that as the working location

RUN mkdir -p /usr/src/pifromcoprimes
WORKDIR /usr/src/pifromcoprimes
COPY CMakeLists.txt .
COPY pfc.cpp .

# This command compiles your app using GCC, adjust for your source code
RUN cmake -DCMAKE_BUILD_TYPE=Release . &&\
    make pfc

RUN addgroup -S nonroot && adduser -S nonroot -G nonroot

USER nonroot

# This command runs your application, comment out this line to compile only
CMD ["/bin/sh", "-c", "./pfc"]

LABEL Name=pifromcoprimes Version=1.2.0
