# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.26

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Produce verbose output by default.
VERBOSE = 1

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /opt/homebrew/Cellar/cmake/3.26.0/bin/cmake

# The command to remove a file.
RM = /opt/homebrew/Cellar/cmake/3.26.0/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/matiny0000/Documents/PhD/terse

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/matiny0000/Documents/PhD/terse/build

# Include any dependencies generated for this target.
include test/googletest-main/googletest/CMakeFiles/gtest.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include test/googletest-main/googletest/CMakeFiles/gtest.dir/compiler_depend.make

# Include the progress variables for this target.
include test/googletest-main/googletest/CMakeFiles/gtest.dir/progress.make

# Include the compile flags for this target's objects.
include test/googletest-main/googletest/CMakeFiles/gtest.dir/flags.make

test/googletest-main/googletest/CMakeFiles/gtest.dir/src/gtest-all.cc.o: test/googletest-main/googletest/CMakeFiles/gtest.dir/flags.make
test/googletest-main/googletest/CMakeFiles/gtest.dir/src/gtest-all.cc.o: /Users/matiny0000/Documents/PhD/terse/test/googletest-main/googletest/src/gtest-all.cc
test/googletest-main/googletest/CMakeFiles/gtest.dir/src/gtest-all.cc.o: test/googletest-main/googletest/CMakeFiles/gtest.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/matiny0000/Documents/PhD/terse/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object test/googletest-main/googletest/CMakeFiles/gtest.dir/src/gtest-all.cc.o"
	cd /Users/matiny0000/Documents/PhD/terse/build/test/googletest-main/googletest && /usr/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT test/googletest-main/googletest/CMakeFiles/gtest.dir/src/gtest-all.cc.o -MF CMakeFiles/gtest.dir/src/gtest-all.cc.o.d -o CMakeFiles/gtest.dir/src/gtest-all.cc.o -c /Users/matiny0000/Documents/PhD/terse/test/googletest-main/googletest/src/gtest-all.cc

test/googletest-main/googletest/CMakeFiles/gtest.dir/src/gtest-all.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/gtest.dir/src/gtest-all.cc.i"
	cd /Users/matiny0000/Documents/PhD/terse/build/test/googletest-main/googletest && /usr/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/matiny0000/Documents/PhD/terse/test/googletest-main/googletest/src/gtest-all.cc > CMakeFiles/gtest.dir/src/gtest-all.cc.i

test/googletest-main/googletest/CMakeFiles/gtest.dir/src/gtest-all.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/gtest.dir/src/gtest-all.cc.s"
	cd /Users/matiny0000/Documents/PhD/terse/build/test/googletest-main/googletest && /usr/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/matiny0000/Documents/PhD/terse/test/googletest-main/googletest/src/gtest-all.cc -o CMakeFiles/gtest.dir/src/gtest-all.cc.s

# Object files for target gtest
gtest_OBJECTS = \
"CMakeFiles/gtest.dir/src/gtest-all.cc.o"

# External object files for target gtest
gtest_EXTERNAL_OBJECTS =

lib/libgtest.a: test/googletest-main/googletest/CMakeFiles/gtest.dir/src/gtest-all.cc.o
lib/libgtest.a: test/googletest-main/googletest/CMakeFiles/gtest.dir/build.make
lib/libgtest.a: test/googletest-main/googletest/CMakeFiles/gtest.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/matiny0000/Documents/PhD/terse/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX static library ../../../lib/libgtest.a"
	cd /Users/matiny0000/Documents/PhD/terse/build/test/googletest-main/googletest && $(CMAKE_COMMAND) -P CMakeFiles/gtest.dir/cmake_clean_target.cmake
	cd /Users/matiny0000/Documents/PhD/terse/build/test/googletest-main/googletest && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/gtest.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
test/googletest-main/googletest/CMakeFiles/gtest.dir/build: lib/libgtest.a
.PHONY : test/googletest-main/googletest/CMakeFiles/gtest.dir/build

test/googletest-main/googletest/CMakeFiles/gtest.dir/clean:
	cd /Users/matiny0000/Documents/PhD/terse/build/test/googletest-main/googletest && $(CMAKE_COMMAND) -P CMakeFiles/gtest.dir/cmake_clean.cmake
.PHONY : test/googletest-main/googletest/CMakeFiles/gtest.dir/clean

test/googletest-main/googletest/CMakeFiles/gtest.dir/depend:
	cd /Users/matiny0000/Documents/PhD/terse/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/matiny0000/Documents/PhD/terse /Users/matiny0000/Documents/PhD/terse/test/googletest-main/googletest /Users/matiny0000/Documents/PhD/terse/build /Users/matiny0000/Documents/PhD/terse/build/test/googletest-main/googletest /Users/matiny0000/Documents/PhD/terse/build/test/googletest-main/googletest/CMakeFiles/gtest.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : test/googletest-main/googletest/CMakeFiles/gtest.dir/depend

