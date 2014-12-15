cmake_minimum_required (VERSION 2.6)
SET(CMAKE_COLOR_MAKEFILE ON)

project (MEngine)

SET( PROJECT_SOURCE_ROOT ${CMAKE_CURRENT_SOURCE_DIR} )
SET( PROJECT_BINARY_ROOT ${CMAKE_CURRENT_BINARY_DIR} )
SET( VERSION_MAJOR 0 )
SET( VERSION_MINOR 1 )
SET( VERSION_PATCH 1 )
SET( VERSION_STRING "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}" )

SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
OPTION( BUILD_EXAMPLES "If on, build executable examples." ON )

# Put executables in bin/
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")

# Include our own directories.
INCLUDE_DIRECTORIES( "${CMAKE_CURRENT_SOURCE_DIR}/include" )

find_package(MATLABLIB REQUIRED)
MESSAGE( STATUS "MATLAB LIBRARIES -> " ${MATLABLIB_LIBRARY} )

# Set MATLAB include dir
include_directories(/usr/local/MATLAB/R2013a/extern/include/)

# Set MATLAB link dir
#link_directories(/usr/local/MATLAB/R2013a/bin/glnxa64/)

FIND_PACKAGE(LCM REQUIRED)

# Try to find Doxygen
FIND_PROGRAM( DOXYGEN_CMD doxygen )
include(cmake/pods.cmake)
include(cmake/lcmtypes.cmake)
lcmtypes_build(CPP_AGGREGATE_HEADER lcmmsgs.hpp)

# Descend into subdirectories
ADD_SUBDIRECTORY( includes )
IF( DOXYGEN_CMD )
   ADD_SUBDIRECTORY( doc )
ENDIF()
ADD_SUBDIRECTORY( src )

