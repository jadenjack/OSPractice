# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.9

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


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
CMAKE_COMMAND = /home/yongjun/Desktop/clion-2017.3.3/bin/cmake/bin/cmake

# The command to remove a file.
RM = /home/yongjun/Desktop/clion-2017.3.3/bin/cmake/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/yongjun/CLionProjects/HW2_OS

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/yongjun/CLionProjects/HW2_OS/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/HW2_OS.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/HW2_OS.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/HW2_OS.dir/flags.make

CMakeFiles/HW2_OS.dir/main.c.o: CMakeFiles/HW2_OS.dir/flags.make
CMakeFiles/HW2_OS.dir/main.c.o: ../main.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/yongjun/CLionProjects/HW2_OS/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/HW2_OS.dir/main.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/HW2_OS.dir/main.c.o   -c /home/yongjun/CLionProjects/HW2_OS/main.c

CMakeFiles/HW2_OS.dir/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/HW2_OS.dir/main.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/yongjun/CLionProjects/HW2_OS/main.c > CMakeFiles/HW2_OS.dir/main.c.i

CMakeFiles/HW2_OS.dir/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/HW2_OS.dir/main.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/yongjun/CLionProjects/HW2_OS/main.c -o CMakeFiles/HW2_OS.dir/main.c.s

CMakeFiles/HW2_OS.dir/main.c.o.requires:

.PHONY : CMakeFiles/HW2_OS.dir/main.c.o.requires

CMakeFiles/HW2_OS.dir/main.c.o.provides: CMakeFiles/HW2_OS.dir/main.c.o.requires
	$(MAKE) -f CMakeFiles/HW2_OS.dir/build.make CMakeFiles/HW2_OS.dir/main.c.o.provides.build
.PHONY : CMakeFiles/HW2_OS.dir/main.c.o.provides

CMakeFiles/HW2_OS.dir/main.c.o.provides.build: CMakeFiles/HW2_OS.dir/main.c.o


CMakeFiles/HW2_OS.dir/disk.c.o: CMakeFiles/HW2_OS.dir/flags.make
CMakeFiles/HW2_OS.dir/disk.c.o: ../disk.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/yongjun/CLionProjects/HW2_OS/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object CMakeFiles/HW2_OS.dir/disk.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/HW2_OS.dir/disk.c.o   -c /home/yongjun/CLionProjects/HW2_OS/disk.c

CMakeFiles/HW2_OS.dir/disk.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/HW2_OS.dir/disk.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/yongjun/CLionProjects/HW2_OS/disk.c > CMakeFiles/HW2_OS.dir/disk.c.i

CMakeFiles/HW2_OS.dir/disk.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/HW2_OS.dir/disk.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/yongjun/CLionProjects/HW2_OS/disk.c -o CMakeFiles/HW2_OS.dir/disk.c.s

CMakeFiles/HW2_OS.dir/disk.c.o.requires:

.PHONY : CMakeFiles/HW2_OS.dir/disk.c.o.requires

CMakeFiles/HW2_OS.dir/disk.c.o.provides: CMakeFiles/HW2_OS.dir/disk.c.o.requires
	$(MAKE) -f CMakeFiles/HW2_OS.dir/build.make CMakeFiles/HW2_OS.dir/disk.c.o.provides.build
.PHONY : CMakeFiles/HW2_OS.dir/disk.c.o.provides

CMakeFiles/HW2_OS.dir/disk.c.o.provides.build: CMakeFiles/HW2_OS.dir/disk.c.o


CMakeFiles/HW2_OS.dir/fs.c.o: CMakeFiles/HW2_OS.dir/flags.make
CMakeFiles/HW2_OS.dir/fs.c.o: ../fs.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/yongjun/CLionProjects/HW2_OS/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building C object CMakeFiles/HW2_OS.dir/fs.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/HW2_OS.dir/fs.c.o   -c /home/yongjun/CLionProjects/HW2_OS/fs.c

CMakeFiles/HW2_OS.dir/fs.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/HW2_OS.dir/fs.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/yongjun/CLionProjects/HW2_OS/fs.c > CMakeFiles/HW2_OS.dir/fs.c.i

CMakeFiles/HW2_OS.dir/fs.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/HW2_OS.dir/fs.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/yongjun/CLionProjects/HW2_OS/fs.c -o CMakeFiles/HW2_OS.dir/fs.c.s

CMakeFiles/HW2_OS.dir/fs.c.o.requires:

.PHONY : CMakeFiles/HW2_OS.dir/fs.c.o.requires

CMakeFiles/HW2_OS.dir/fs.c.o.provides: CMakeFiles/HW2_OS.dir/fs.c.o.requires
	$(MAKE) -f CMakeFiles/HW2_OS.dir/build.make CMakeFiles/HW2_OS.dir/fs.c.o.provides.build
.PHONY : CMakeFiles/HW2_OS.dir/fs.c.o.provides

CMakeFiles/HW2_OS.dir/fs.c.o.provides.build: CMakeFiles/HW2_OS.dir/fs.c.o


CMakeFiles/HW2_OS.dir/mount.c.o: CMakeFiles/HW2_OS.dir/flags.make
CMakeFiles/HW2_OS.dir/mount.c.o: ../mount.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/yongjun/CLionProjects/HW2_OS/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building C object CMakeFiles/HW2_OS.dir/mount.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/HW2_OS.dir/mount.c.o   -c /home/yongjun/CLionProjects/HW2_OS/mount.c

CMakeFiles/HW2_OS.dir/mount.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/HW2_OS.dir/mount.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/yongjun/CLionProjects/HW2_OS/mount.c > CMakeFiles/HW2_OS.dir/mount.c.i

CMakeFiles/HW2_OS.dir/mount.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/HW2_OS.dir/mount.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/yongjun/CLionProjects/HW2_OS/mount.c -o CMakeFiles/HW2_OS.dir/mount.c.s

CMakeFiles/HW2_OS.dir/mount.c.o.requires:

.PHONY : CMakeFiles/HW2_OS.dir/mount.c.o.requires

CMakeFiles/HW2_OS.dir/mount.c.o.provides: CMakeFiles/HW2_OS.dir/mount.c.o.requires
	$(MAKE) -f CMakeFiles/HW2_OS.dir/build.make CMakeFiles/HW2_OS.dir/mount.c.o.provides.build
.PHONY : CMakeFiles/HW2_OS.dir/mount.c.o.provides

CMakeFiles/HW2_OS.dir/mount.c.o.provides.build: CMakeFiles/HW2_OS.dir/mount.c.o


# Object files for target HW2_OS
HW2_OS_OBJECTS = \
"CMakeFiles/HW2_OS.dir/main.c.o" \
"CMakeFiles/HW2_OS.dir/disk.c.o" \
"CMakeFiles/HW2_OS.dir/fs.c.o" \
"CMakeFiles/HW2_OS.dir/mount.c.o"

# External object files for target HW2_OS
HW2_OS_EXTERNAL_OBJECTS =

HW2_OS: CMakeFiles/HW2_OS.dir/main.c.o
HW2_OS: CMakeFiles/HW2_OS.dir/disk.c.o
HW2_OS: CMakeFiles/HW2_OS.dir/fs.c.o
HW2_OS: CMakeFiles/HW2_OS.dir/mount.c.o
HW2_OS: CMakeFiles/HW2_OS.dir/build.make
HW2_OS: CMakeFiles/HW2_OS.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/yongjun/CLionProjects/HW2_OS/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Linking C executable HW2_OS"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/HW2_OS.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/HW2_OS.dir/build: HW2_OS

.PHONY : CMakeFiles/HW2_OS.dir/build

CMakeFiles/HW2_OS.dir/requires: CMakeFiles/HW2_OS.dir/main.c.o.requires
CMakeFiles/HW2_OS.dir/requires: CMakeFiles/HW2_OS.dir/disk.c.o.requires
CMakeFiles/HW2_OS.dir/requires: CMakeFiles/HW2_OS.dir/fs.c.o.requires
CMakeFiles/HW2_OS.dir/requires: CMakeFiles/HW2_OS.dir/mount.c.o.requires

.PHONY : CMakeFiles/HW2_OS.dir/requires

CMakeFiles/HW2_OS.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/HW2_OS.dir/cmake_clean.cmake
.PHONY : CMakeFiles/HW2_OS.dir/clean

CMakeFiles/HW2_OS.dir/depend:
	cd /home/yongjun/CLionProjects/HW2_OS/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/yongjun/CLionProjects/HW2_OS /home/yongjun/CLionProjects/HW2_OS /home/yongjun/CLionProjects/HW2_OS/cmake-build-debug /home/yongjun/CLionProjects/HW2_OS/cmake-build-debug /home/yongjun/CLionProjects/HW2_OS/cmake-build-debug/CMakeFiles/HW2_OS.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/HW2_OS.dir/depend
