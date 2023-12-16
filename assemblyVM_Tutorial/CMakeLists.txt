cmake_minimum_required(VERSION 3.0)

project(assemblyVM)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# target_include_directories(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/include/)

# Add executable and specify all source files
add_executable(${PROJECT_NAME}	${CMAKE_CURRENT_SOURCE_DIR}/main.c 
							${CMAKE_CURRENT_SOURCE_DIR}/memory.c 
							${CMAKE_CURRENT_SOURCE_DIR}/opfs.c)

