cmake_minimum_required(VERSION 3.23)
project(5py)

set(CMAKE_CXX_STANDARD 17)
include_directories(C:/Users/MrCre/AppData/Local/Programs/Python/Python311/include)
link_libraries(C:/Users/MrCre/AppData/Local/Programs/Python/Python311/libs/python311.lib)
link_libraries(C:/Users/MrCre/AppData/Local/Programs/Python/Python311/libs/libpython311.a)

add_executable(5py main.cpp commitViz.cpp commitViz.h)
