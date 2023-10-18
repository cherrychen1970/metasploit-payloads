set(PROJECT_NAME json-c)

project(${PROJECT_NAME} C)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MP")

set(SRC_DIR .)
file(GLOB SRC_FILES
    ${SRC_DIR}/*.c
)
add_library(${PROJECT_NAME} ${SRC_FILES})