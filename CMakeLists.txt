cmake_minimum_required(VERSION 3.12)
project(MyProject VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug
    CACHE STRING "Choose the type of build (Debug or Release)" FORCE)
endif()

configure_file(config.h.in config.h)

find_package(SFML 2.5.1 
  COMPONENTS 
    system window graphics network audio REQUIRED)

include_directories(
  "${CMAKE_CURRENT_SOURCE_DIR}"
  "C:/SFML-2.5.1/include/SFML/Window"
)
add_executable(main ${PROJECT_SOURCE_DIR}/src/main.cpp)

target_link_libraries(main sfml-graphics sfml-window sfml-system)
target_include_directories(main PRIVATE "${PROJECT_BINARY_DIR}")

install(TARGETS main DESTINATION bin)