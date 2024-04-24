cmake_minimum_required(VERSION 3.16.3)

set(APP_NAME YukikazeClient)
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "limited configs" FORCE)

project(${APP_NAME})

find_package(Threads)
find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)

set(SRCS ../api/yukikaze.grpc.pb.cc ../api/yukikaze.pb.cc main.cpp yukikaze_client.cpp)
add_executable(${APP_NAME} ${SRCS})

target_link_libraries(${APP_NAME}
        Threads::Threads
        protobuf::libprotobuf
        gRPC::grpc++_unsecure
        gRPC::grpc++_reflection
        )
set_target_properties(${APP_NAME} PROPERTIES OUTPUT_NAME yukikaze_client)