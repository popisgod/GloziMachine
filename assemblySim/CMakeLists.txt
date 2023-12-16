cmake_minimum_required(VERSION 3.0)

project(lc3_Sim)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Add executable and specify all source files
add_executable(${PROJECT_NAME}	${CMAKE_CURRENT_SOURCE_DIR}/lc3_processor.c 
							${CMAKE_CURRENT_SOURCE_DIR}/lc3_config.h 
							${CMAKE_CURRENT_SOURCE_DIR}/utils_core.c)

