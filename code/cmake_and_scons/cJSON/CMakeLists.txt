file(GLOB SRC *.c)

include_directories(.)

add_library(cJSON STATIC
    ${SRC}
    )