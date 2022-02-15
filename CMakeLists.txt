set(Boost_USE_STATIC_LIBS        ON)  # only find static libs
set(Boost_USE_DEBUG_LIBS         OFF) # ignore debug libs and 
set(Boost_USE_RELEASE_LIBS       ON)  # only find release libs 
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF) 
find_package(Boost 1.71.0 COMPONENTS cpp_int cpp_dec_float)
if(Boost_FOUND)   
    include_directories(${Boost_INCLUDE_DIRS})
    add_executable(piFromCoprimes main.cpp)   
    target_link_libraries(foo ${Boost_LIBRARIES})
endif()