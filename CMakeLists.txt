# CMakeList.txt: 顶层 CMake 项目文件，在此处执行全局配置
# 并包含子项目。
#
cmake_minimum_required (VERSION 3.8)

# 如果支持，请为 MSVC 编译器启用热重载。
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("MyMuduoCore")

#设置全局的c++标准
set (CMAKE_CXX_STANDARD 11) 
set (CMAKE_CXX_STANDARD_REQUIRED True)

#链接必要的库，比如刚刚我们写好的在src文件Cmakelists中muduo-core_lib静态库，还有全局链接库
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

set(LIBS
    pthread
)

#添加子目录
add_subdirectory(src)


