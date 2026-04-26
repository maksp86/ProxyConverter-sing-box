cmake_minimum_required(VERSION 3.20)
project(proxy-converter VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_UNITY_BUILD_BATCH_SIZE 32)

if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    string(REPLACE "/Zi" "/Z7" CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")
    string(REPLACE "/Zi" "/Z7" CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO}")
endif()

find_package(Qt6 REQUIRED COMPONENTS Core Network)

if (WIN32)
    include("cmake/windows/windows.cmake")
endif ()
message("[CMAKE_PREFIX_PATH] ${CMAKE_PREFIX_PATH}")

# for some cross toolchain
list(APPEND CMAKE_FIND_ROOT_PATH ${CMAKE_PREFIX_PATH})
message("[CMAKE_FIND_ROOT_PATH] ${CMAKE_FIND_ROOT_PATH}")

set(BUILD_SHARED_LIBS OFF)

set(THRONE_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/third_party/Throne)

file(GLOB THRONE_CONFIG_COMMON_SRC ${THRONE_ROOT}/src/configs/common/*.cpp)
file(GLOB THRONE_OUTBOUNDS_SRC ${THRONE_ROOT}/src/configs/outbounds/*.cpp)

set(PROJECT_SOURCES
    src/main.cpp
    src/profile_ctor_stub.cpp
    src/throne_cli_utils.cpp
    ${THRONE_CONFIG_COMMON_SRC}
    ${THRONE_OUTBOUNDS_SRC}
    ${THRONE_ROOT}/src/configs/sub/clash.cpp
)

qt_add_executable(proxy-converter
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES})

target_include_directories(proxy-converter PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${THRONE_ROOT})

set_property(TARGET proxy-converter PROPERTY AUTOUIC ON)
set_property(TARGET proxy-converter PROPERTY AUTOMOC ON)
set_property(TARGET proxy-converter PROPERTY AUTORCC ON)

set_target_properties(proxy-converter PROPERTIES
        WIN32_EXECUTABLE TRUE
        UNITY_BUILD ON
)

target_link_libraries(proxy-converter PRIVATE Qt6::Core Qt6::Network)

qt_finalize_executable(proxy-converter)