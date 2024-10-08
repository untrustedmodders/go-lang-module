cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

if(POLICY CMP0092)
    cmake_policy(SET CMP0092 NEW) # Don't add -W3 warning level by default.
endif()


project(plugify-module-golang VERSION 1.0.0.0 DESCRIPTION "Go Language Module" HOMEPAGE_URL "https://github.com/untrustedmodders/plugify-module-golang" LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

if(UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif()

#
# Plugify
#
set(PLUGIFY_BUILD_SHARED_LIB ON CACHE INTERNAL "")
set(PLUGIFY_BUILD_FUNCTION ON CACHE INTERNAL "")
set(PLUGIFY_BUILD_ASSEMBLY ON CACHE INTERNAL "")
if(LINUX)
    set(PLUGIFY_USE_STATIC_STDLIB ON CACHE INTERNAL "")
endif()
add_subdirectory(external/plugify)

#
# Dyncall
#
include(cmake/dyncall.cmake)

#
# Go Language Module for Plugify
#
file(GLOB_RECURSE GOLM_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "src/*.cpp")
set(GOLM_PCH_FILE "src/pch.h")

add_library(${PROJECT_NAME} SHARED ${GOLM_SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

set(GOLM_LINK_LIBRARIES plugify::plugify plugify::plugify-assembly asmjit::asmjit dyncall_s dyncallback_s)

if(NOT COMPILER_SUPPORTS_FORMAT)
    set(GOLM_LINK_LIBRARIES ${GOLM_LINK_LIBRARIES} fmt::fmt-header-only)
endif()

target_include_directories(${PROJECT_NAME} PRIVATE ${dyncall_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE ${GOLM_LINK_LIBRARIES})
target_precompile_headers(${PROJECT_NAME} PRIVATE ${GOLM_PCH_FILE})

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wextra -Wconversion -Werror)# -Wshadow -Wpedantic
endif()

if(APPLE)
    target_link_options(${PROJECT_NAME} PRIVATE "-Wl,-exported_symbols_list,${CMAKE_CURRENT_SOURCE_DIR}/sym/exported_symbols.lds")
elseif(UNIX)
    target_link_options(${PROJECT_NAME} PRIVATE "-Wl,--version-script,${CMAKE_CURRENT_SOURCE_DIR}/sym/version_script.lds")
endif()

include(GenerateExportHeader)
generate_export_header(${PROJECT_NAME} EXPORT_MACRO_NAME GOLM_EXPORT EXPORT_FILE_NAME ${CMAKE_BINARY_DIR}/exports/module_export.h)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_BINARY_DIR}/exports)

target_compile_definitions(${PROJECT_NAME} PRIVATE
        GOLM_PLATFORM_WINDOWS=$<BOOL:${WIN32}>
        GOLM_PLATFORM_APPLE=$<BOOL:${APPLE}>
        GOLM_PLATFORM_LINUX=$<BOOL:${LINUX}>
        GOLM_IS_DEBUG=$<STREQUAL:${CMAKE_BUILD_TYPE},Debug>
        GOLM_LIBRARY_SUFFIX="${CMAKE_SHARED_LIBRARY_SUFFIX}"
        GOLM_LIBRARY_PREFIX="${CMAKE_SHARED_LIBRARY_PREFIX}")

set(GOLM_VERSION "0" CACHE STRING "Set version name")
set(GOLM_PACKAGE "${PROJECT_NAME}" CACHE STRING "Set package name")

configure_file(
    ${CMAKE_SOURCE_DIR}/${PROJECT_NAME}.pmodule.in
    ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.pmodule
)