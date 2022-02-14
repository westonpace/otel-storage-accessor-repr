cmake_minimum_required(VERSION 3.21)
project(ot_seg_repr)

set(CMAKE_CXX_STANDARD 14)

add_executable(ot_seg_repr main.cc)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(ot_seg_repr PRIVATE Threads::Threads)

find_package(opentelemetry-cpp CONFIG REQUIRED)
target_include_directories(ot_seg_repr PRIVATE ${OPENTELEMETRY_CPP_INCLUDE_DIRS})
target_link_libraries(ot_seg_repr PRIVATE ${OPENTELEMETRY_CPP_LIBRARIES})

find_package(CURL REQUIRED)
target_include_directories(ot_seg_repr PRIVATE ${CURL_INCLUDE_DIR})
target_link_libraries(ot_seg_repr PRIVATE ${CURL_LIBRARIES})
