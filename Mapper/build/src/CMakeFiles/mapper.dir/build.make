# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list

# Produce verbose output by default.
VERBOSE = 1

# Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/hadoop/Documents/machinelearning/localcode/DKMeans/Mapper

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/hadoop/Documents/machinelearning/localcode/DKMeans/Mapper/build

# Include any dependencies generated for this target.
include src/CMakeFiles/mapper.dir/depend.make

# Include the progress variables for this target.
include src/CMakeFiles/mapper.dir/progress.make

# Include the compile flags for this target's objects.
include src/CMakeFiles/mapper.dir/flags.make

src/CMakeFiles/mapper.dir/main.cpp.o: src/CMakeFiles/mapper.dir/flags.make
src/CMakeFiles/mapper.dir/main.cpp.o: ../src/main.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/hadoop/Documents/machinelearning/localcode/DKMeans/Mapper/build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object src/CMakeFiles/mapper.dir/main.cpp.o"
	cd /home/hadoop/Documents/machinelearning/localcode/DKMeans/Mapper/build/src && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/mapper.dir/main.cpp.o -c /home/hadoop/Documents/machinelearning/localcode/DKMeans/Mapper/src/main.cpp

src/CMakeFiles/mapper.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/mapper.dir/main.cpp.i"
	cd /home/hadoop/Documents/machinelearning/localcode/DKMeans/Mapper/build/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/hadoop/Documents/machinelearning/localcode/DKMeans/Mapper/src/main.cpp > CMakeFiles/mapper.dir/main.cpp.i

src/CMakeFiles/mapper.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/mapper.dir/main.cpp.s"
	cd /home/hadoop/Documents/machinelearning/localcode/DKMeans/Mapper/build/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/hadoop/Documents/machinelearning/localcode/DKMeans/Mapper/src/main.cpp -o CMakeFiles/mapper.dir/main.cpp.s

src/CMakeFiles/mapper.dir/main.cpp.o.requires:
.PHONY : src/CMakeFiles/mapper.dir/main.cpp.o.requires

src/CMakeFiles/mapper.dir/main.cpp.o.provides: src/CMakeFiles/mapper.dir/main.cpp.o.requires
	$(MAKE) -f src/CMakeFiles/mapper.dir/build.make src/CMakeFiles/mapper.dir/main.cpp.o.provides.build
.PHONY : src/CMakeFiles/mapper.dir/main.cpp.o.provides

src/CMakeFiles/mapper.dir/main.cpp.o.provides.build: src/CMakeFiles/mapper.dir/main.cpp.o

# Object files for target mapper
mapper_OBJECTS = \
"CMakeFiles/mapper.dir/main.cpp.o"

# External object files for target mapper
mapper_EXTERNAL_OBJECTS =

src/mapper: src/CMakeFiles/mapper.dir/main.cpp.o
src/mapper: src/CMakeFiles/mapper.dir/build.make
src/mapper: src/CMakeFiles/mapper.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable mapper"
	cd /home/hadoop/Documents/machinelearning/localcode/DKMeans/Mapper/build/src && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/mapper.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/CMakeFiles/mapper.dir/build: src/mapper
.PHONY : src/CMakeFiles/mapper.dir/build

src/CMakeFiles/mapper.dir/requires: src/CMakeFiles/mapper.dir/main.cpp.o.requires
.PHONY : src/CMakeFiles/mapper.dir/requires

src/CMakeFiles/mapper.dir/clean:
	cd /home/hadoop/Documents/machinelearning/localcode/DKMeans/Mapper/build/src && $(CMAKE_COMMAND) -P CMakeFiles/mapper.dir/cmake_clean.cmake
.PHONY : src/CMakeFiles/mapper.dir/clean

src/CMakeFiles/mapper.dir/depend:
	cd /home/hadoop/Documents/machinelearning/localcode/DKMeans/Mapper/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/hadoop/Documents/machinelearning/localcode/DKMeans/Mapper /home/hadoop/Documents/machinelearning/localcode/DKMeans/Mapper/src /home/hadoop/Documents/machinelearning/localcode/DKMeans/Mapper/build /home/hadoop/Documents/machinelearning/localcode/DKMeans/Mapper/build/src /home/hadoop/Documents/machinelearning/localcode/DKMeans/Mapper/build/src/CMakeFiles/mapper.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/CMakeFiles/mapper.dir/depend

