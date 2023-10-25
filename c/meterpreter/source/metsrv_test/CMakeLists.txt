cmake_minimum_required(VERSION 3.26)

add_subdirectory(../metsrv metsrv)
add_subdirectory(../extensions/stdapi stdapi)

set(PROJECT_NAME metsrv_test)
set(ALIAS metro)

project(${PROJECT_NAME} C)

# include(${CMAKE_CURRENT_SOURCE_DIR}/../CMakeListsFuncs.txt)
add_definitions(
    -DUNICODE
    -D_UNICODE
    -D_CRT_SECURE_NO_WARNINGS
    -DDEBUGTRACE
)

if(MSVC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /we 4013 /MP")
endif()

include_directories(../)
include_directories(../common)
include_directories(../metsrv)

set(SRC_DIR .)

file(GLOB SRC_FILES
    ${SRC_DIR}/*.c
)

add_executable(${PROJECT_NAME} ${SRC_FILES})

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME ${ALIAS})

set(LINK_LIBS metsrv_static)

if(MSVC)
#    target_link_options(${PROJECT_NAME} PUBLIC "/ignore:4070")
endif()

target_link_libraries(${PROJECT_NAME} ${LINK_LIBS})

add_custom_target(run
    COMMAND ${CMAKE_BINARY_DIR}/${ALIAS}.exe
    USES_TERMINAL
)
