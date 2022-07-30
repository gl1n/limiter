cmake_minimum_required(VERSION 3.16)

project(mydocker)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -g")
set(CMAKE_CXX_STANDARD 11)

aux_source_directory(${PROJECT_SOURCE_DIR}/src SOURCES)

add_executable(myctn
  ${SOURCES}
  )
target_include_directories(myctn
  PRIVATE
    ${PROJECT_SOURCE_DIR}/third_party
    ${PROJECT_SOURCE_DIR}/include
    )
