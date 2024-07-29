cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_DEBUG_POSTFIX d)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -s")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -ggdb")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

project(pfc VERSION 1.0)
add_executable(pfc pfc.cpp)
set_target_properties(pfc PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})

find_package(Boost CONFIG REQUIRED COMPONENTS thread chrono random)
target_include_directories(pfc PRIVATE ${Boost_INCLUDE_DIR})
target_link_libraries(pfc PRIVATE ${Boost_LIBRARIES} ${Boost_DEPENDENCIES})

find_package(OpenCL 3.0)

IF (${OpenCL_FOUND})
    find_package(SSE REQUIRED)
    
    add_library(sfmt STATIC gpu/lib/SFMT-src-1.5.1/SFMT.c)
    target_compile_definitions(sfmt PRIVATE -DSFMT_MEXP=19937)
    IF ("${SSE2_FOUND}")
        target_compile_options(sfmt PRIVATE -msse2 -DHAVE_SSE2)
    ENDIF ("${SSE2_FOUND}")

    set_target_properties(sfmt PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/gpu/lib/SFMT-src-1.5.1"
    )

    project(pfc_gpu VERSION 1.0)
    find_package(Boost CONFIG REQUIRED random system filesystem)
    add_executable(pfc_gpu gpu/pfc_gpu.cpp)
    target_compile_definitions(pfc_gpu PRIVATE -DSFMT_MEXP=19937)

    IF ("${SSE2_FOUND}")
        target_compile_options(pfc_gpu PRIVATE -msse2 -DHAVE_SSE2)
    ENDIF ("${SSE2_FOUND}")

    set_target_properties(pfc_gpu PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
    target_include_directories(pfc_gpu PRIVATE ${OpenCL_INCLUDE_DIR} ${Boost_INCLUDE_DIR})
    target_link_libraries(pfc_gpu PRIVATE 
        sfmt 
        ${OpenCL_LIBRARIES} ${OpenCL_DEPENDENCIES}
        ${Boost_LIBRARIES} ${Boost_DEPENDENCIES}
    )
ENDIF (${OpenCL_FOUND})
