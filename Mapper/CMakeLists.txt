############################ PROJECT NAME	
project(mapper)

############################ VERBOSITY
SET(CMAKE_VERBOSE_MAKEFILE on)

############################ VERSION
# Check cMake version
CMAKE_MINIMUM_REQUIRED( VERSION 2.6.0 )

############################ COLORIZE MAKEFILE
# Colorized output
SET( CMAKE_COLOR_MAKEFILE TRUE )

############################ CHOOSE COMPILE MODE
SET(CMAKE_CXXFLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")
SET(CMAKE_CXXFLAGS_RELEASE "$ENV{CXXFLAGS} -O2 -Wall")
SET(CMAKE_BUILD_TYPE "Release")
############################ CMAKE PACKAGE DIRECTORY
# Set cmakeModules folder
SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/")

#add_executable(mapper src/main.cpp)

#install(TARGETS mapper RUNTIME DESTINATION bin)

add_subdirectory(src)