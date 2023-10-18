cmake_minimum_required(VERSION 3.26)

add_subdirectory(../json-c json-c)

set(PROJECT_NAME metsrv)
set(ALIAS metro)

project(${PROJECT_NAME} C)

# include(${CMAKE_CURRENT_SOURCE_DIR}/../CMakeListsFuncs.txt)

add_definitions(
    -D_USRDLL
    -DUNICODE
    -D_UNICODE
    -D_CRT_SECURE_NO_WARNINGS
    -DDEBUGTRACE
)

if(MSVC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MP")
endif()

include_directories(../)
include_directories(../common)
include_directories(../ReflectiveDLLInjection/common)

set(SRC_DIR .)
set(MOD_DEF_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../def)

file(GLOB SRC_FILES
    ${SRC_DIR}/*.c
#    ${MOD_DEF_DIR}/metsrv.def
)
add_library(${PROJECT_NAME} SHARED ${SRC_FILES})
add_library(${PROJECT_NAME}_static ${SRC_FILES})


set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${ALIAS}.${TARGET_ARCH})
set_target_properties(${PROJECT_NAME}_static PROPERTIES OUTPUT_NAME ${ALIAS}_static.${TARGET_ARCH})

if(MSVC)
#    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/DEF:\"${MOD_DEF_DIR}/metsrv.def\"")
#    set_source_files_properties(${MOD_DEF_DIR}/metsrv.def PROPERTIES HEADER_FILE_ONLY TRUE)
endif()

set(LINK_LIBS winhttp wininet crypt32 ws2_32 json-c)

if(MSVC)
    target_link_options(${PROJECT_NAME} PUBLIC "/ignore:4070")
else()
    set(LINK_LIBS ${LINK_LIBS} ws2_32)
endif()

target_link_libraries(${PROJECT_NAME} ${LINK_LIBS})
target_link_libraries(${PROJECT_NAME}_static ${LINK_LIBS})
# Post processing (required for all Meterpreter DLLs)
# editbin(${PROJECT_NAME} ${BIN_SUBSYSTEM})
# copyoutput(${PROJECT_NAME} ${BIN_OUTPUT_DIR})
