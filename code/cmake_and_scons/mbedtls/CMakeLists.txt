file(GLOB SRC *.c)

add_library(mbedlts STATIC
    ${SRC}
    )