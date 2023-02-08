cmake_minimum_required(VERSION 3.14)

project(bus_mdu)
find_package(SystemCLanguage CONFIG REQUIRED)
set (CMAKE_CXX_STANDARD ${SystemC_CXX_STANDARD})

add_compile_options(-Wall -Wextra -Werror=init-self -Wno-unused-parameter)

# Add source files
include_directories(include)
include_directories(simple_bus_headers)
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Create executable
add_executable(bus_mdu ${SOURCES})
target_link_libraries(bus_mdu SystemC::systemc)
