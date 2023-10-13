cmake_minimum_required(VERSION 3.26)

add_subdirectory(../metsrv metsrv)
add_subdirectory(../extensions/stdapi stdapi)
set(PROJECT_NAME zoo)

project(${PROJECT_NAME} C)

# include(${CMAKE_CURRENT_SOURCE_DIR}/../CMakeListsFuncs.txt)

add_definitions(
    -D_USRDLL
    -DUNICODE
    -D_UNICODE
    -D_CRT_SECURE_NO_WARNINGS
    -DDEBUGTRACE
)

include_directories(../common)
include_directories(../ReflectiveDLLInjection/common)

if(MSVC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MP")
endif()


file(GLOB SRC_FILES
    ./*.c
)

add_library(test SHARED 
    dll.c
    ../def/plugin.def
)
set_target_properties(test PROPERTIES LINK_FLAGS "/DEF:\"../def/plugin.def\"")

add_executable(${PROJECT_NAME} main.c)

target_link_libraries(${PROJECT_NAME} metsrv ext_server_stdapi)

add_custom_target(run
    COMMAND ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.${TARGET_ARCH}.exe
    USES_TERMINAL
)


# get_cmake_property(_variableNames VARIABLES)
# list (SORT _variableNames)
# foreach (_variableName ${_variableNames})
# message(STATUS "${_variableName}=${${_variableName}}")
# endforeach()

# Post processing (required for all Meterpreter DLLs)
#editbin(${PROJECT_NAME} ${BIN_SUBSYSTEM})
#copyoutput(${PROJECT_NAME} ${BIN_OUTPUT_DIR})
