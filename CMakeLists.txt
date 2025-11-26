cmake_minimum_required(VERSION 3.10)

project(TinyDDS VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17) # C++17
set(CMAKE_CXX_STANDARD_REQUIRED NO) # 强制要求编译器必须支持指定的C++标准

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic") # 开启所有警告

include_directories(${CMAKE_SOURCE_DIR}/include) # 设置头文件搜索路径

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin) # 可执行文件输出路径
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib) # 库文件输出路径
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib) # 静态库文件输出路径

# 递归搜索源文件并存储在变量 TINYDDS_SOURCES 中
file(GLOB_RECURSE TINYDDS_SOURCES
    "src/rtps/*.cpp"
    "src/dds/*.cpp"
    "src/discovery/*.cpp"
    "src/qos/*.cpp"
    "src/transport/*.cpp"
)

# 创建静态库目标 tinydds
add_library(tinydds STATIC ${TINYDDS_SOURCES})

target_link_libraries(tinydds pthread) # 目标 tinydds 链接到 pthread 库

# add_subdirectory(tests) # 添加测试子目录
# add_subdirectory(examples) # 添加示例子目录

message(STATUS "TinyDDS version: ${PROJECT_VERSION}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# 启用测试
enable_testing()

# 添加测试程序
add_executable(test_sequence_number  tests/test_sequence_number.cpp)
# 编译时：编译器会找到 tinydds 库的头文件路径
# 链接时：链接器会将 tinydds 的二进制代码合并到最终的可执行文件中
# 运行时：可执行文件能够调用 tinydds 库中的函数和类
target_link_libraries(test_sequence_number tinydds)

# 添加 CDR 测试
add_executable(test_cdr tests/test_cdr.cpp)
target_link_libraries(test_cdr tinydds)

# 添加 LOC 测试
add_executable(test_locator tests/test_locator.cpp)
# target_link_libraries(test_locator tinydds)
