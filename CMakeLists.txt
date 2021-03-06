cmake_minimum_required(VERSION 2.8)
project(LinuxServerDev)

# include (sylar/cmake/utils.cmake)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O0 -ggdb -std=c++11 -Wall -Wno-deprecated -Werror -Wno-unused-function -Wno-builtin-macro-redefined")

include_directories(.)
include_directories(/usr/local/include)

link_directories(/usr/local/lib)

find_library(YAMLCPP yaml-cpp)

set(
    LIB_SRC
    sylar/log.cc
    sylar/util.cc
    sylar/config.cc
    sylar/thread.cc
)

add_library(sylar SHARED ${LIB_SRC})
# force_redefine_file_macro_for_sources(sylar)  #__FILE__
#add_library(sylar_static STATIC ${LIB_SRC})
#SET_TARGET_PROPERTIES {sylar_static PROPERTIES OUTPUT_NAME "sylar"}


set(LIB_LIB
    sylar
    pthread
    ${YAMLCPP}


)
add_executable(test tests/test.cc)
add_dependencies(test sylar)
# force_redefine_file_macro_for_sources(test)  #__FILE__
target_link_libraries(test ${LIB_LIB})

add_executable(test_config tests/test_config.cc)
add_dependencies(test_config sylar)
# force_redefine_file_macro_for_sources(test_config)  #__FILE__
target_link_libraries(test_config sylar ${LIB_LIB})


add_executable(test_thread tests/test_thread.cc)
add_dependencies(test_thread sylar)
# force_redefine_file_macro_for_sources(test_config)  #__FILE__
target_link_libraries(test_thread sylar ${LIB_LIB})

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)