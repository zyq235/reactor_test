# CMakeList.txt: reactor_test 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#

# 将源代码添加到此项目的可执行文件。
add_executable (reactor_test "reactor_cli.cpp" "reactor_cli.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET reactor_test PROPERTY CXX_STANDARD 20)
endif()