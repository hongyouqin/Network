cmake_minimum_required(VERSION 2.8)

project(server)

#模式
set (CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -g")

aux_source_directory(. SRCS)


#生成可执行文件
add_executable(server ${SRCS})