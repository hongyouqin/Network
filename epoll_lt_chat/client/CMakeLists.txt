cmake_minimum_required(VERSION 2.8)

project(chat_client)

#模式
set (CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -g")

aux_source_directory(. SRCS)

link_libraries("-pthread")

#生成可执行文件
add_executable(chat_client ${SRCS})