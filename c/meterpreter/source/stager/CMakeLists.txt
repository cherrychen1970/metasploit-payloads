cmake_minimum_required(VERSION 3.26)

add_subdirectory(../metsrv metsrv)
add_subdirectory(../extensions/stdapi stdapi)
set(PROJECT_NAME stager)

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


file(GLOB SRC_FILES
    ./*.c
)
#add_library(${PROJECT_NAME} SHARED ${SRC_FILES})
add_executable(${PROJECT_NAME} ${SRC_FILES})

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${PROJECT_NAME}.${TARGET_ARCH})

set(LINK_LIBS winhttp wininet crypt32)

if(MSVC)
    target_link_options(${PROJECT_NAME} PUBLIC "/ignore:4070")
else()
    set(LINK_LIBS ${LINK_LIBS} ws2_32)
endif()

target_link_libraries(${PROJECT_NAME} ${LINK_LIBS})

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
