cmake_minimum_required(VERSION 3.16.3)

set(APP_NAME YukikazeServer)
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "limited configs" FORCE)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g3 -ggdb")

project(${APP_NAME})

find_package(Threads)
find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)

include_directories(../)
include_directories(../include)
include_directories(../Amatsukaze)
include_directories(../Amatsukaze/Amatsukaze)
include_directories(../Amatsukaze/Amatsukaze/linux)
include_directories(../Amatsukaze/common)
include_directories(../Amatsukaze/include)
include_directories(../Amatsukaze/include_gpl)
set(SRCS ../api/yukikaze.grpc.pb.cc ../api/yukikaze.pb.cc main.cpp encode.cpp subprocess.cpp yukikaze_server.cpp)
add_executable(${APP_NAME} ${SRCS})

target_link_libraries(${APP_NAME}
        Threads::Threads
        protobuf::libprotobuf
        gRPC::grpc++_unsecure
        gRPC::grpc++_reflection
)
set_target_properties(${APP_NAME} PROPERTIES OUTPUT_NAME yukikaze_server)