cmake_minimum_required(VERSION 3.10)
project(open_gl_app)

set(CMAKE_CXX_STANDARD 17)

file(GLOB_RECURSE SOURCES "${CMAKE_SOURCE_DIR}/src/*.cpp" "${CMAKE_SOURCE_DIR}/src/*.c")

message(STATUS "Source files found:")
foreach(src_file ${SOURCES})
    message(STATUS "  ${src_file}")
endforeach()

add_executable(open_gl_app ${SOURCES})

target_include_directories(open_gl_app PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/external
)

target_link_libraries(open_gl_app PRIVATE
    "${CMAKE_SOURCE_DIR}/libs/glfw3.lib"
)
