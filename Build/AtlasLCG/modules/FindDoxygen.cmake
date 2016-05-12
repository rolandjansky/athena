# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: FindDoxygen.cmake 719980 2016-01-27 08:38:07Z krasznaa $
#
# This file is here to intercept find_package(Doxygen) calls, and extend the
# environment setup file of the project with the correct Doxygen paths.
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

# Call CMake's own FindDoxygen.cmake. Note that the arguments created for this
# script by CMake pass through to the official script. So we don't need to
# give any extra arguments to this call.
find_package( Doxygen )

# Restore CMAKE_MODULE_PATH:
set( CMAKE_MODULE_PATH ${_modulePathBackup} )
unset( _modulePathBackup )
set( CMAKE_SYSTEM_IGNORE_PATH )

# Set some extra variable(s), to make the environment configuration easier:
if( DOXYGEN_EXECUTABLE )
   get_filename_component( DOXYGEN_BINARY_PATH ${DOXYGEN_EXECUTABLE} PATH )
endif()

# Set up the RPM dependency:
lcg_need_rpm( doxygen )
