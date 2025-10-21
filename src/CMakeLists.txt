#获取当前目录下的所有源文件
file(GLOB SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cc)

#创建静态库或共享库
add_library(mymuduo_core SHARED ${SRC_FILES})

#设置头文件的路径
target_include_directories(mymuduo_core PUBLIC ${CMAKE_SOURCE_DIR}/include)

# 设置编译选项
target_compile_options(mymuduo_core PRIVATE -Wall -Wextra -std=c++17)

# 链接pthread库
target_link_libraries(mymuduo_core pthread)