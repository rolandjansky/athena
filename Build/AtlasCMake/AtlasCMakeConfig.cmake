# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: AtlasCMakeConfig.cmake 746543 2016-05-12 11:31:22Z krasznaa $
#
# This file is used to make it possible to pull in the code of this package into
# a top level CMake file using:
#
#   find_package( AtlasCMake )
#
# Then, to make CMake find this file correctly, one has to use something like
# the following on the command line:
#
#   CMAKE_PREFIX_PATH=../source/AtlasCMake cmake ../source
#
# Or:
#
#   cmake -DAtlasCMake_DIR=../source/AtlasCMake ../source
#
# The meat of the code is in the other files of the package, this file just
# provides the interface to the find_package(...) function.

# Print a status message that the file was found:
if( NOT AtlasCMake_FIND_QUIETLY )
   message( STATUS "AtlasCMake found with version: $Revision: 746543 $" )
endif()

# Get the current directory:
get_filename_component( _thisdir "${CMAKE_CURRENT_LIST_FILE}" PATH )
if( NOT AtlasCMake_FIND_QUIETLY )
   message( STATUS "Including ATLAS CMake code from directory: ${_thisdir}" )
endif()
set( ATLASCMAKE_MODULEDIR ${_thisdir}/modules CACHE STRING
   "Directory holding the ATLAS CMake files" FORCE )

# Add it to the CMake module path:
list( INSERT CMAKE_MODULE_PATH 0 ${_thisdir}/modules )
list( REMOVE_DUPLICATES CMAKE_MODULE_PATH )

# Make sure that since this project is being included directly, the built
# projects don't shadow the modules of this package with possibly older
# versions of the files.
set( ATLAS_DONT_PREPEND_PROJECT_MODULES TRUE
   CACHE BOOL "Prevent built projects from prepending to CMAKE_MODULE_PATH" )

# Install the helper script(s) from the package:
install( PROGRAMS ${_thisdir}/scripts/acmake.py DESTINATION bin )
install( DIRECTORY ${_thisdir}/cmt2cmake DESTINATION .
   USE_SOURCE_PERMISSIONS
   PATTERN ".*" EXCLUDE
   PATTERN "*.pyc" EXCLUDE )

# If CTEST_USE_LAUNCHERS is turned on, create a dummy log file already
# during configuration, which NICOS can pick up, and conclude that the
# package's "build" was successful.
if( CTEST_USE_LAUNCHERS )
   file( MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/BuildLogs )
   file( WRITE ${CMAKE_BINARY_DIR}/BuildLogs/AtlasCMake.log
      "Dummy build log\n"
      "AtlasCMake: Package build succeeded" )
endif()

# Include the AtlasFunctions file:
include( AtlasFunctions )

# For now include the Athena adaptor functions:
include( AthenaFunctions )

# Enable/Configure the use of CTest in the project:
atlas_ctest_setup()

# Environment variable(s) needed for the build:
set( ATLASCMAKE_ENVIRONMENT
   APPEND PATH \${${CMAKE_PROJECT_NAME}_DIR}/cmt2cmake
   SET GFORTRAN_UNBUFFERED_ALL "y" )

# If there's a version file available, declare the version of this package
# to the project:
if( EXISTS ${_thisdir}/version.cmake )
   file( READ ${_thisdir}/version.cmake _version LIMIT 10000 )
   set_property( GLOBAL APPEND PROPERTY ATLAS_SPECIAL_PACKAGES
      "Build/AtlasCMake" "${_version}" )
elseif( EXISTS ${_thisdir}/cmt/version.cmt )
   file( READ ${_thisdir}/cmt/version.cmt _version LIMIT 10000 )
   set_property( GLOBAL APPEND PROPERTY ATLAS_SPECIAL_PACKAGES
      "Build/AtlasCMake" "${_version}" )
endif()

# Clean up:
unset( _thisdir )
