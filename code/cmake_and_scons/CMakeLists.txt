# CMake最低版本号要求
cmake_minimum_required(VERSION 3.1)
# 项目信息
project(cmakeTest) # 写入工程名

set (CMAKE_C_COMPILER "/usr/bin/gcc")
set (CMAKE_CXX_COMPILER "/usr/bin/g++")

set (CMAKE_C_FLAGS "-g")

# 如果需要进行调试需要设置这个命令
set(CMAKE_BUILD_TYPE Debug)

# 向工程添加多个特定的头文件搜索路径，路径之间用空格分隔
# 相当于指定g++编译器的-I参数
include_directories (${CMAKE_SOURCE_DIR})
include_directories (${CMAKE_SOURCE_DIR}/inc)
include_directories (${CMAKE_SOURCE_DIR}/httpclient/include)
include_directories (${CMAKE_SOURCE_DIR}/cJSON)

# 把src目录下的源文件存列表存放到变量SOURCES里
file(GLOB SOURCES src/*.c)

# 指定生成目标
add_executable(${PROJECT_NAME} ${SOURCES})

add_subdirectory(mbedtls)
add_subdirectory(httpclient)
add_subdirectory(cJSON)

target_link_libraries(${CMAKE_PROJECT_NAME} mbedlts httpclient cJSON)

# 设置生成目标后exe文件存放位置
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
