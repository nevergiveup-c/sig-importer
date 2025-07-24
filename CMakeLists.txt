cmake_minimum_required(VERSION 3.31)
project(sig_importer)

set(CMAKE_CXX_STANDARD 20)

set(IDA_SDK_DIR "${CMAKE_SOURCE_DIR}/src/ida-sdk")
set(IDA_INCLUDE_DIR "${IDA_SDK_DIR}/include")
set(IDA_LIB_DIR "${IDA_SDK_DIR}/lib")

file(GLOB_RECURSE SOURCES
        "src/shared/*.cpp"
        "src/shared/*.h"
        "src/search/*.h"
        "src/search/*.cpp"
        "src/import/*.h"
        "src/import/*.cpp"
        "src/headers-only/*.cpp"
        "src/headers-only/*.h"
        "src/*.cpp"
        "src/*.h"
)

add_library(sig_importer SHARED ${SOURCES})

set_target_properties(sig_importer PROPERTIES PREFIX "")

target_include_directories(sig_importer
        PRIVATE
        "${CMAKE_SOURCE_DIR}/src/shared"
        "${CMAKE_SOURCE_DIR}/src/headers-only"
        "${CMAKE_SOURCE_DIR}/src/search"
        "${CMAKE_SOURCE_DIR}/src/import"
        "${CMAKE_SOURCE_DIR}/src"
        PUBLIC
        "${IDA_INCLUDE_DIR}"
)

target_link_directories(sig_importer
        PUBLIC
        "${IDA_LIB_DIR}"
)

target_link_libraries(sig_importer
        "${IDA_LIB_DIR}/ida.lib"
        "${IDA_LIB_DIR}/idalib.lib"
        "${IDA_LIB_DIR}/network.lib")

target_compile_definitions(sig_importer PRIVATE
        __NT__
        __IDP__
        dont_use_snprintf=snprintf
        dont_use_fgetc=fgetc
        _strtoui64=strtoull
)

