# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: FindPythonInterp.cmake 719980 2016-01-27 08:38:07Z krasznaa $
#
# This file is here to intercept find_package(PythonInterp) calls, and extend
# the environment setup file of the project with the correct Python paths.
#

# The LCG include(s):
include( LCGFunctions )

# Temporarily clean out CMAKE_MODULE_PATH, so that we could pick up
# FindPythonInterp.cmake from CMake:
set( _modulePathBackup ${CMAKE_MODULE_PATH} )
set( CMAKE_MODULE_PATH )

# Make the code ignore the system path(s):
set( CMAKE_SYSTEM_IGNORE_PATH /usr/include /usr/bin /usr/lib /usr/lib32
   /usr/lib64 )

# Call CMake's own FindPythonInterp.cmake. Note that the arguments created
# for this script by CMake pass through to the official script. So we don't
# need to give any extra arguments to this call.
find_package( PythonInterp )

# Restore CMAKE_MODULE_PATH:
set( CMAKE_MODULE_PATH ${_modulePathBackup} )
unset( _modulePathBackup )
set( CMAKE_SYSTEM_IGNORE_PATH )

# Set some extra variable(s), to make the environment configuration easier:
if( PYTHON_EXECUTABLE )
   get_filename_component( PythonInterp_BINARY_PATH ${PYTHON_EXECUTABLE} PATH )
endif()

# Set up the RPM dependency:
lcg_need_rpm( Python FOUND_NAME PYTHONINTERP VERSION_NAME PYTHON )
