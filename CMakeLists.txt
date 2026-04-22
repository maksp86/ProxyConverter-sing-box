cmake_minimum_required(VERSION 3.20)
project(proxy_converter_throne LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(PROXY_CONVERTER_HEADLESS "Build the converter without GUI-only Qt dependencies." ON)
option(PROXY_CONVERTER_STATIC_MSVC_RUNTIME "Link the MSVC runtime statically." ON)
option(PROXY_CONVERTER_BUNDLE_QT_RUNTIME "Bundle Qt runtime files during install for portable artifacts." ON)

if (MSVC AND PROXY_CONVERTER_STATIC_MSVC_RUNTIME)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

find_package(Qt6 REQUIRED COMPONENTS Core Network)

set(THRONE_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/third_party/Throne)

file(GLOB THRONE_CONFIG_COMMON_SRC ${THRONE_ROOT}/src/configs/common/*.cpp)
file(GLOB THRONE_OUTBOUNDS_SRC ${THRONE_ROOT}/src/configs/outbounds/*.cpp)

add_executable(proxy-converter
    src/main.cpp
    src/profile_ctor_stub.cpp
    src/throne_cli_utils.cpp
    ${THRONE_CONFIG_COMMON_SRC}
    ${THRONE_OUTBOUNDS_SRC}
    ${THRONE_ROOT}/src/configs/sub/clash.cpp)

if (PROXY_CONVERTER_HEADLESS)
    target_compile_definitions(proxy-converter PRIVATE THRONE_CLI_HEADLESS)
endif()

target_include_directories(proxy-converter PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${THRONE_ROOT}
    ${CMAKE_BINARY_DIR})

target_link_libraries(proxy-converter PRIVATE Qt6::Core Qt6::Network)

install(TARGETS proxy-converter
    RUNTIME DESTINATION .
    BUNDLE DESTINATION .)

if (PROXY_CONVERTER_BUNDLE_QT_RUNTIME AND COMMAND qt_generate_deploy_app_script)
    set(_deploy_opts NO_UNSUPPORTED_PLATFORM_ERROR)

    if (MSVC AND PROXY_CONVERTER_STATIC_MSVC_RUNTIME)
        list(APPEND _deploy_opts NO_COMPILER_RUNTIME)
    endif()

    qt_generate_deploy_app_script(
        TARGET proxy-converter
        OUTPUT_SCRIPT proxy_converter_deploy_script
        ${_deploy_opts}
        NO_TRANSLATIONS
    )

    install(SCRIPT ${proxy_converter_deploy_script})
endif()

set(CPACK_PACKAGE_NAME "proxy-converter")
set(CPACK_PACKAGE_VENDOR "ProxyConverter-sing-box")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "CLI converter for sing-box and Xray proxy links")
set(CPACK_PACKAGE_FILE_NAME
    "${CPACK_PACKAGE_NAME}-${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}")

if (WIN32)
    set(CPACK_GENERATOR "ZIP")
else()
    set(CPACK_GENERATOR "TGZ")
endif()

include(CPack)
