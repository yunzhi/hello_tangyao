file(GLOB SRC src/*.c)

include_directories(include)

add_library(httpclient STATIC
    ${SRC}
    oneos/http_oneos_wrapper.c
    )