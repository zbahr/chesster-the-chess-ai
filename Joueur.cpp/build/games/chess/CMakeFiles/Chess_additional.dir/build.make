# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.11

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
CMAKE_COMMAND = /usr/local/bin/cmake

# The command to remove a file.
RM = /usr/local/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = "/Users/zachary/Desktop/CS 5400/Git Repositories - Game Series/2018-sp-a-game-4-zjb998/Joueur.cpp"

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = "/Users/zachary/Desktop/CS 5400/Git Repositories - Game Series/2018-sp-a-game-4-zjb998/Joueur.cpp/build"

# Utility rule file for Chess_additional.

# Include the progress variables for this target.
include games/chess/CMakeFiles/Chess_additional.dir/progress.make

games/chess/CMakeFiles/Chess_additional: ../games/chess/additional_files.txt
	cd "/Users/zachary/Desktop/CS 5400/Git Repositories - Game Series/2018-sp-a-game-4-zjb998/Joueur.cpp/build/games/chess" && /usr/local/bin/cmake -E touch /Users/zachary/Desktop/CS\ 5400/Git\ Repositories\ -\ Game\ Series/2018-sp-a-game-4-zjb998/Joueur.cpp/games/chess/CMakeLists.txt

Chess_additional: games/chess/CMakeFiles/Chess_additional
Chess_additional: games/chess/CMakeFiles/Chess_additional.dir/build.make

.PHONY : Chess_additional

# Rule to build all files generated by this target.
games/chess/CMakeFiles/Chess_additional.dir/build: Chess_additional

.PHONY : games/chess/CMakeFiles/Chess_additional.dir/build

games/chess/CMakeFiles/Chess_additional.dir/clean:
	cd "/Users/zachary/Desktop/CS 5400/Git Repositories - Game Series/2018-sp-a-game-4-zjb998/Joueur.cpp/build/games/chess" && $(CMAKE_COMMAND) -P CMakeFiles/Chess_additional.dir/cmake_clean.cmake
.PHONY : games/chess/CMakeFiles/Chess_additional.dir/clean

games/chess/CMakeFiles/Chess_additional.dir/depend:
	cd "/Users/zachary/Desktop/CS 5400/Git Repositories - Game Series/2018-sp-a-game-4-zjb998/Joueur.cpp/build" && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" "/Users/zachary/Desktop/CS 5400/Git Repositories - Game Series/2018-sp-a-game-4-zjb998/Joueur.cpp" "/Users/zachary/Desktop/CS 5400/Git Repositories - Game Series/2018-sp-a-game-4-zjb998/Joueur.cpp/games/chess" "/Users/zachary/Desktop/CS 5400/Git Repositories - Game Series/2018-sp-a-game-4-zjb998/Joueur.cpp/build" "/Users/zachary/Desktop/CS 5400/Git Repositories - Game Series/2018-sp-a-game-4-zjb998/Joueur.cpp/build/games/chess" "/Users/zachary/Desktop/CS 5400/Git Repositories - Game Series/2018-sp-a-game-4-zjb998/Joueur.cpp/build/games/chess/CMakeFiles/Chess_additional.dir/DependInfo.cmake" --color=$(COLOR)
.PHONY : games/chess/CMakeFiles/Chess_additional.dir/depend

