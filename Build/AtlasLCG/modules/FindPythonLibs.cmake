# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: FindPythonLibs.cmake 734923 2016-04-08 15:32:19Z krasznaa $
#
# This file is here to intercept find_package(PythonLibs) calls, and
# massage the paths produced by the system module, to make them relocatable.
#

# The LCG include(s):
include( LCGFunctions )

# Temporarily clean out CMAKE_MODULE_PATH, so that we could pick up
# FindPythonLibs.cmake from CMake:
set( _modulePathBackup ${CMAKE_MODULE_PATH} )
set( CMAKE_MODULE_PATH )

# Make the code ignore the system path(s):
set( CMAKE_SYSTEM_IGNORE_PATH /usr/include /usr/bin /usr/lib /usr/lib32
   /usr/lib64 )

# Call CMake's own FindPythonLibs.cmake. Note that the arguments created
# for this script by CMake pass through to the official script. So we don't
# need to give any extra arguments to this call.
find_package( PythonLibs )

# Restore CMAKE_MODULE_PATH:
set( CMAKE_MODULE_PATH ${_modulePathBackup} )
unset( _modulePathBackup )
set( CMAKE_SYSTEM_IGNORE_PATH )

# Massage the paths found by the system module:
if( PYTHONLIBS_FOUND AND NOT GAUDI_ATLAS )

   # Set the PYTHONLIBS_LIBRARY_DIRS variable, used by the
   # lcg_generate_env function later on:
   list( GET PYTHON_LIBRARIES 0 _library )
   get_filename_component( PYTHONLIBS_LIBRARY_DIRS ${_library} PATH )
   unset( _library )

   # Update the include directories:
   set( _newIncludes )
   foreach( _inc ${PYTHON_INCLUDE_DIRS} )
      set( _relocatableDir ${_inc} )
      _lcg_make_paths_relocatable( _relocatableDir )
      list( APPEND _newIncludes
         $<BUILD_INTERFACE:${_inc}>
         $<INSTALL_INTERFACE:${_relocatableDir}> )
      unset( _relocatableDir )
   endforeach()
   set( PYTHON_INCLUDE_DIRS ${_newIncludes} )
   unset( _newIncludes )

   # Update the libraries:
   set( _newLibs )
   foreach( _lib ${PYTHON_LIBRARIES} )
      set( _relocatableLib ${_lib} )
      _lcg_make_paths_relocatable( _relocatableLib )
      list( APPEND _newLibs
         $<BUILD_INTERFACE:${_lib}>
         $<INSTALL_INTERFACE:${_relocatableLib}> )
      unset( _relocatableLib )
   endforeach()
   set( PYTHON_LIBRARIES ${_newLibs} )
   unset( _newLibs )

   # Set the PYTHONLIBS_INCLIDE_DIRS variable, used by the
   # lcg_generate_env function later on:
   set( PYTHONLIBS_INCLUDE_DIRS ${PYTHON_INCLUDE_DIRS} )

endif()

# Set up the RPM dependency:
lcg_need_rpm( Python FOUND_NAME PYTHONLIBS VERSION_NAME PYTHON )
