# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list

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
CMAKE_COMMAND = /usr/local/Cellar/cmake/2.8.12.1/bin/cmake

# The command to remove a file.
RM = /usr/local/Cellar/cmake/2.8.12.1/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The program to use to edit the cache.
CMAKE_EDIT_COMMAND = /usr/local/Cellar/cmake/2.8.12.1/bin/ccmake

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/kleinerm/projects/Horde3D

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/kleinerm/projects/Horde3D

# Include any dependencies generated for this target.
include Horde3D/Source/ColladaConverter/CMakeFiles/ColladaConv.dir/depend.make

# Include the progress variables for this target.
include Horde3D/Source/ColladaConverter/CMakeFiles/ColladaConv.dir/progress.make

# Include the compile flags for this target's objects.
include Horde3D/Source/ColladaConverter/CMakeFiles/ColladaConv.dir/flags.make

Horde3D/Source/ColladaConverter/CMakeFiles/ColladaConv.dir/converter.o: Horde3D/Source/ColladaConverter/CMakeFiles/ColladaConv.dir/flags.make
Horde3D/Source/ColladaConverter/CMakeFiles/ColladaConv.dir/converter.o: Horde3D/Source/ColladaConverter/converter.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /Users/kleinerm/projects/Horde3D/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object Horde3D/Source/ColladaConverter/CMakeFiles/ColladaConv.dir/converter.o"
	cd /Users/kleinerm/projects/Horde3D/Horde3D/Source/ColladaConverter && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/ColladaConv.dir/converter.o -c /Users/kleinerm/projects/Horde3D/Horde3D/Source/ColladaConverter/converter.cpp

Horde3D/Source/ColladaConverter/CMakeFiles/ColladaConv.dir/converter.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/ColladaConv.dir/converter.i"
	cd /Users/kleinerm/projects/Horde3D/Horde3D/Source/ColladaConverter && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /Users/kleinerm/projects/Horde3D/Horde3D/Source/ColladaConverter/converter.cpp > CMakeFiles/ColladaConv.dir/converter.i

Horde3D/Source/ColladaConverter/CMakeFiles/ColladaConv.dir/converter.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/ColladaConv.dir/converter.s"
	cd /Users/kleinerm/projects/Horde3D/Horde3D/Source/ColladaConverter && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /Users/kleinerm/projects/Horde3D/Horde3D/Source/ColladaConverter/converter.cpp -o CMakeFiles/ColladaConv.dir/converter.s

Horde3D/Source/ColladaConverter/CMakeFiles/ColladaConv.dir/converter.o.requires:
.PHONY : Horde3D/Source/ColladaConverter/CMakeFiles/ColladaConv.dir/converter.o.requires

Horde3D/Source/ColladaConverter/CMakeFiles/ColladaConv.dir/converter.o.provides: Horde3D/Source/ColladaConverter/CMakeFiles/ColladaConv.dir/converter.o.requires
	$(MAKE) -f Horde3D/Source/ColladaConverter/CMakeFiles/ColladaConv.dir/build.make Horde3D/Source/ColladaConverter/CMakeFiles/ColladaConv.dir/converter.o.provides.build
.PHONY : Horde3D/Source/ColladaConverter/CMakeFiles/ColladaConv.dir/converter.o.provides

Horde3D/Source/ColladaConverter/CMakeFiles/ColladaConv.dir/converter.o.provides.build: Horde3D/Source/ColladaConverter/CMakeFiles/ColladaConv.dir/converter.o

Horde3D/Source/ColladaConverter/CMakeFiles/ColladaConv.dir/daeMain.o: Horde3D/Source/ColladaConverter/CMakeFiles/ColladaConv.dir/flags.make
Horde3D/Source/ColladaConverter/CMakeFiles/ColladaConv.dir/daeMain.o: Horde3D/Source/ColladaConverter/daeMain.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /Users/kleinerm/projects/Horde3D/CMakeFiles $(CMAKE_PROGRESS_2)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object Horde3D/Source/ColladaConverter/CMakeFiles/ColladaConv.dir/daeMain.o"
	cd /Users/kleinerm/projects/Horde3D/Horde3D/Source/ColladaConverter && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/ColladaConv.dir/daeMain.o -c /Users/kleinerm/projects/Horde3D/Horde3D/Source/ColladaConverter/daeMain.cpp

Horde3D/Source/ColladaConverter/CMakeFiles/ColladaConv.dir/daeMain.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/ColladaConv.dir/daeMain.i"
	cd /Users/kleinerm/projects/Horde3D/Horde3D/Source/ColladaConverter && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /Users/kleinerm/projects/Horde3D/Horde3D/Source/ColladaConverter/daeMain.cpp > CMakeFiles/ColladaConv.dir/daeMain.i

Horde3D/Source/ColladaConverter/CMakeFiles/ColladaConv.dir/daeMain.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/ColladaConv.dir/daeMain.s"
	cd /Users/kleinerm/projects/Horde3D/Horde3D/Source/ColladaConverter && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /Users/kleinerm/projects/Horde3D/Horde3D/Source/ColladaConverter/daeMain.cpp -o CMakeFiles/ColladaConv.dir/daeMain.s

Horde3D/Source/ColladaConverter/CMakeFiles/ColladaConv.dir/daeMain.o.requires:
.PHONY : Horde3D/Source/ColladaConverter/CMakeFiles/ColladaConv.dir/daeMain.o.requires

Horde3D/Source/ColladaConverter/CMakeFiles/ColladaConv.dir/daeMain.o.provides: Horde3D/Source/ColladaConverter/CMakeFiles/ColladaConv.dir/daeMain.o.requires
	$(MAKE) -f Horde3D/Source/ColladaConverter/CMakeFiles/ColladaConv.dir/build.make Horde3D/Source/ColladaConverter/CMakeFiles/ColladaConv.dir/daeMain.o.provides.build
.PHONY : Horde3D/Source/ColladaConverter/CMakeFiles/ColladaConv.dir/daeMain.o.provides

Horde3D/Source/ColladaConverter/CMakeFiles/ColladaConv.dir/daeMain.o.provides.build: Horde3D/Source/ColladaConverter/CMakeFiles/ColladaConv.dir/daeMain.o

Horde3D/Source/ColladaConverter/CMakeFiles/ColladaConv.dir/main.o: Horde3D/Source/ColladaConverter/CMakeFiles/ColladaConv.dir/flags.make
Horde3D/Source/ColladaConverter/CMakeFiles/ColladaConv.dir/main.o: Horde3D/Source/ColladaConverter/main.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /Users/kleinerm/projects/Horde3D/CMakeFiles $(CMAKE_PROGRESS_3)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object Horde3D/Source/ColladaConverter/CMakeFiles/ColladaConv.dir/main.o"
	cd /Users/kleinerm/projects/Horde3D/Horde3D/Source/ColladaConverter && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/ColladaConv.dir/main.o -c /Users/kleinerm/projects/Horde3D/Horde3D/Source/ColladaConverter/main.cpp

Horde3D/Source/ColladaConverter/CMakeFiles/ColladaConv.dir/main.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/ColladaConv.dir/main.i"
	cd /Users/kleinerm/projects/Horde3D/Horde3D/Source/ColladaConverter && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /Users/kleinerm/projects/Horde3D/Horde3D/Source/ColladaConverter/main.cpp > CMakeFiles/ColladaConv.dir/main.i

Horde3D/Source/ColladaConverter/CMakeFiles/ColladaConv.dir/main.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/ColladaConv.dir/main.s"
	cd /Users/kleinerm/projects/Horde3D/Horde3D/Source/ColladaConverter && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /Users/kleinerm/projects/Horde3D/Horde3D/Source/ColladaConverter/main.cpp -o CMakeFiles/ColladaConv.dir/main.s

Horde3D/Source/ColladaConverter/CMakeFiles/ColladaConv.dir/main.o.requires:
.PHONY : Horde3D/Source/ColladaConverter/CMakeFiles/ColladaConv.dir/main.o.requires

Horde3D/Source/ColladaConverter/CMakeFiles/ColladaConv.dir/main.o.provides: Horde3D/Source/ColladaConverter/CMakeFiles/ColladaConv.dir/main.o.requires
	$(MAKE) -f Horde3D/Source/ColladaConverter/CMakeFiles/ColladaConv.dir/build.make Horde3D/Source/ColladaConverter/CMakeFiles/ColladaConv.dir/main.o.provides.build
.PHONY : Horde3D/Source/ColladaConverter/CMakeFiles/ColladaConv.dir/main.o.provides

Horde3D/Source/ColladaConverter/CMakeFiles/ColladaConv.dir/main.o.provides.build: Horde3D/Source/ColladaConverter/CMakeFiles/ColladaConv.dir/main.o

Horde3D/Source/ColladaConverter/CMakeFiles/ColladaConv.dir/optimizer.o: Horde3D/Source/ColladaConverter/CMakeFiles/ColladaConv.dir/flags.make
Horde3D/Source/ColladaConverter/CMakeFiles/ColladaConv.dir/optimizer.o: Horde3D/Source/ColladaConverter/optimizer.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /Users/kleinerm/projects/Horde3D/CMakeFiles $(CMAKE_PROGRESS_4)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object Horde3D/Source/ColladaConverter/CMakeFiles/ColladaConv.dir/optimizer.o"
	cd /Users/kleinerm/projects/Horde3D/Horde3D/Source/ColladaConverter && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/ColladaConv.dir/optimizer.o -c /Users/kleinerm/projects/Horde3D/Horde3D/Source/ColladaConverter/optimizer.cpp

Horde3D/Source/ColladaConverter/CMakeFiles/ColladaConv.dir/optimizer.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/ColladaConv.dir/optimizer.i"
	cd /Users/kleinerm/projects/Horde3D/Horde3D/Source/ColladaConverter && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /Users/kleinerm/projects/Horde3D/Horde3D/Source/ColladaConverter/optimizer.cpp > CMakeFiles/ColladaConv.dir/optimizer.i

Horde3D/Source/ColladaConverter/CMakeFiles/ColladaConv.dir/optimizer.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/ColladaConv.dir/optimizer.s"
	cd /Users/kleinerm/projects/Horde3D/Horde3D/Source/ColladaConverter && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /Users/kleinerm/projects/Horde3D/Horde3D/Source/ColladaConverter/optimizer.cpp -o CMakeFiles/ColladaConv.dir/optimizer.s

Horde3D/Source/ColladaConverter/CMakeFiles/ColladaConv.dir/optimizer.o.requires:
.PHONY : Horde3D/Source/ColladaConverter/CMakeFiles/ColladaConv.dir/optimizer.o.requires

Horde3D/Source/ColladaConverter/CMakeFiles/ColladaConv.dir/optimizer.o.provides: Horde3D/Source/ColladaConverter/CMakeFiles/ColladaConv.dir/optimizer.o.requires
	$(MAKE) -f Horde3D/Source/ColladaConverter/CMakeFiles/ColladaConv.dir/build.make Horde3D/Source/ColladaConverter/CMakeFiles/ColladaConv.dir/optimizer.o.provides.build
.PHONY : Horde3D/Source/ColladaConverter/CMakeFiles/ColladaConv.dir/optimizer.o.provides

Horde3D/Source/ColladaConverter/CMakeFiles/ColladaConv.dir/optimizer.o.provides.build: Horde3D/Source/ColladaConverter/CMakeFiles/ColladaConv.dir/optimizer.o

Horde3D/Source/ColladaConverter/CMakeFiles/ColladaConv.dir/utils.o: Horde3D/Source/ColladaConverter/CMakeFiles/ColladaConv.dir/flags.make
Horde3D/Source/ColladaConverter/CMakeFiles/ColladaConv.dir/utils.o: Horde3D/Source/ColladaConverter/utils.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /Users/kleinerm/projects/Horde3D/CMakeFiles $(CMAKE_PROGRESS_5)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object Horde3D/Source/ColladaConverter/CMakeFiles/ColladaConv.dir/utils.o"
	cd /Users/kleinerm/projects/Horde3D/Horde3D/Source/ColladaConverter && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/ColladaConv.dir/utils.o -c /Users/kleinerm/projects/Horde3D/Horde3D/Source/ColladaConverter/utils.cpp

Horde3D/Source/ColladaConverter/CMakeFiles/ColladaConv.dir/utils.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/ColladaConv.dir/utils.i"
	cd /Users/kleinerm/projects/Horde3D/Horde3D/Source/ColladaConverter && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /Users/kleinerm/projects/Horde3D/Horde3D/Source/ColladaConverter/utils.cpp > CMakeFiles/ColladaConv.dir/utils.i

Horde3D/Source/ColladaConverter/CMakeFiles/ColladaConv.dir/utils.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/ColladaConv.dir/utils.s"
	cd /Users/kleinerm/projects/Horde3D/Horde3D/Source/ColladaConverter && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /Users/kleinerm/projects/Horde3D/Horde3D/Source/ColladaConverter/utils.cpp -o CMakeFiles/ColladaConv.dir/utils.s

Horde3D/Source/ColladaConverter/CMakeFiles/ColladaConv.dir/utils.o.requires:
.PHONY : Horde3D/Source/ColladaConverter/CMakeFiles/ColladaConv.dir/utils.o.requires

Horde3D/Source/ColladaConverter/CMakeFiles/ColladaConv.dir/utils.o.provides: Horde3D/Source/ColladaConverter/CMakeFiles/ColladaConv.dir/utils.o.requires
	$(MAKE) -f Horde3D/Source/ColladaConverter/CMakeFiles/ColladaConv.dir/build.make Horde3D/Source/ColladaConverter/CMakeFiles/ColladaConv.dir/utils.o.provides.build
.PHONY : Horde3D/Source/ColladaConverter/CMakeFiles/ColladaConv.dir/utils.o.provides

Horde3D/Source/ColladaConverter/CMakeFiles/ColladaConv.dir/utils.o.provides.build: Horde3D/Source/ColladaConverter/CMakeFiles/ColladaConv.dir/utils.o

# Object files for target ColladaConv
ColladaConv_OBJECTS = \
"CMakeFiles/ColladaConv.dir/converter.o" \
"CMakeFiles/ColladaConv.dir/daeMain.o" \
"CMakeFiles/ColladaConv.dir/main.o" \
"CMakeFiles/ColladaConv.dir/optimizer.o" \
"CMakeFiles/ColladaConv.dir/utils.o"

# External object files for target ColladaConv
ColladaConv_EXTERNAL_OBJECTS =

Horde3D/Binaries/Darwin/ColladaConv: Horde3D/Source/ColladaConverter/CMakeFiles/ColladaConv.dir/converter.o
Horde3D/Binaries/Darwin/ColladaConv: Horde3D/Source/ColladaConverter/CMakeFiles/ColladaConv.dir/daeMain.o
Horde3D/Binaries/Darwin/ColladaConv: Horde3D/Source/ColladaConverter/CMakeFiles/ColladaConv.dir/main.o
Horde3D/Binaries/Darwin/ColladaConv: Horde3D/Source/ColladaConverter/CMakeFiles/ColladaConv.dir/optimizer.o
Horde3D/Binaries/Darwin/ColladaConv: Horde3D/Source/ColladaConverter/CMakeFiles/ColladaConv.dir/utils.o
Horde3D/Binaries/Darwin/ColladaConv: Horde3D/Source/ColladaConverter/CMakeFiles/ColladaConv.dir/build.make
Horde3D/Binaries/Darwin/ColladaConv: Horde3D/Source/ColladaConverter/CMakeFiles/ColladaConv.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable ../../Binaries/Darwin/ColladaConv"
	cd /Users/kleinerm/projects/Horde3D/Horde3D/Source/ColladaConverter && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/ColladaConv.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
Horde3D/Source/ColladaConverter/CMakeFiles/ColladaConv.dir/build: Horde3D/Binaries/Darwin/ColladaConv
.PHONY : Horde3D/Source/ColladaConverter/CMakeFiles/ColladaConv.dir/build

Horde3D/Source/ColladaConverter/CMakeFiles/ColladaConv.dir/requires: Horde3D/Source/ColladaConverter/CMakeFiles/ColladaConv.dir/converter.o.requires
Horde3D/Source/ColladaConverter/CMakeFiles/ColladaConv.dir/requires: Horde3D/Source/ColladaConverter/CMakeFiles/ColladaConv.dir/daeMain.o.requires
Horde3D/Source/ColladaConverter/CMakeFiles/ColladaConv.dir/requires: Horde3D/Source/ColladaConverter/CMakeFiles/ColladaConv.dir/main.o.requires
Horde3D/Source/ColladaConverter/CMakeFiles/ColladaConv.dir/requires: Horde3D/Source/ColladaConverter/CMakeFiles/ColladaConv.dir/optimizer.o.requires
Horde3D/Source/ColladaConverter/CMakeFiles/ColladaConv.dir/requires: Horde3D/Source/ColladaConverter/CMakeFiles/ColladaConv.dir/utils.o.requires
.PHONY : Horde3D/Source/ColladaConverter/CMakeFiles/ColladaConv.dir/requires

Horde3D/Source/ColladaConverter/CMakeFiles/ColladaConv.dir/clean:
	cd /Users/kleinerm/projects/Horde3D/Horde3D/Source/ColladaConverter && $(CMAKE_COMMAND) -P CMakeFiles/ColladaConv.dir/cmake_clean.cmake
.PHONY : Horde3D/Source/ColladaConverter/CMakeFiles/ColladaConv.dir/clean

Horde3D/Source/ColladaConverter/CMakeFiles/ColladaConv.dir/depend:
	cd /Users/kleinerm/projects/Horde3D && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/kleinerm/projects/Horde3D /Users/kleinerm/projects/Horde3D/Horde3D/Source/ColladaConverter /Users/kleinerm/projects/Horde3D /Users/kleinerm/projects/Horde3D/Horde3D/Source/ColladaConverter /Users/kleinerm/projects/Horde3D/Horde3D/Source/ColladaConverter/CMakeFiles/ColladaConv.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : Horde3D/Source/ColladaConverter/CMakeFiles/ColladaConv.dir/depend

