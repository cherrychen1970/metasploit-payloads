set(PROJECT_NAME ext_server_stdapi)

project(${PROJECT_NAME} C CXX)

include(${CMAKE_CURRENT_SOURCE_DIR}/../CMakeListsFuncs.txt)

add_definitions(
    -D_USRDLL
    -DCINTERFACE
    -DCOBJMACROS
    -D_CRT_SECURE_NO_WARNINGS
)

if(MSVC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MP")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
else()
    include_directories(../../source/mingw-include)
endif()

include_directories(../../source/common)
include_directories(../../source/jpeg-8)
include_directories(../../source/ReflectiveDLLInjection/common)
include_directories(../../source/extensions/stdapi/server)

link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../jpeg/build)

set(SRC_DIR ../../source/extensions/stdapi/server)
set(MOD_DEF_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../source/def)

#GLOB_RECURSE
file(GLOB SRC_FILES
    ${SRC_DIR}/*.c
    ${SRC_DIR}/audio/*.c
    ${SRC_DIR}/fs/*.c
    ${SRC_DIR}/net/*.c
    ${SRC_DIR}/net/config/*.c
    ${SRC_DIR}/net/socket/*.c
    ${SRC_DIR}/railgun/*.c
    ${SRC_DIR}/resource/*.c
    ${SRC_DIR}/resource/*.rc
    ${SRC_DIR}/sys/*.c
    ${SRC_DIR}/sys/config/*.c
    ${SRC_DIR}/sys/eventlog/*.c
    ${SRC_DIR}/sys/power/*.c
    ${SRC_DIR}/sys/process/*.c
    ${SRC_DIR}/sys/registry/*.c
    ${SRC_DIR}/ui/*.c
    ${MOD_DEF_DIR}/extension.def
)

#list(REMOVE_ITEM SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/${SRC_DIR}/resource/hook.c)

add_library(${PROJECT_NAME} SHARED ${SRC_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${PROJECT_NAME}.${TARGET_ARCH})
if(MSVC)
    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/DEF:\"${MOD_DEF_DIR}/extension.def\"")
    set_source_files_properties(${MOD_DEF_DIR}/extension.def PROPERTIES HEADER_FILE_ONLY TRUE)
endif()

set(LINK_LIBS
    jpeg
    mpr
    netapi32
    psapi
    winmm
    iphlpapi
    shlwapi
    ws2_32
    strmiids
)

if(MSVC)
    target_link_options(${PROJECT_NAME} PUBLIC "/ignore:4070")
endif()

target_link_libraries(${PROJECT_NAME} ${LINK_LIBS})

# Post processing (required for all Meterpreter DLLs)
editbin(${PROJECT_NAME} ${BIN_SUBSYSTEM})
copyoutput(${PROJECT_NAME} ${BIN_OUTPUT_DIR})
