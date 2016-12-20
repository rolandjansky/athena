# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: FindJava.cmake 790613 2016-12-20 14:56:58Z krasznaa $
#
# This file is here to intercept find_package(Java) calls, and
# massage the paths produced by the system module, to make them relocatable.
#

# The LCG include(s):
include( LCGFunctions )

# Temporarily clean out CMAKE_MODULE_PATH, so that we could pick up
# FindJava.cmake from CMake:
set( _modulePathBackup ${CMAKE_MODULE_PATH} )
set( CMAKE_MODULE_PATH )

# Make the code ignore the system path(s):
if( JAVA_ROOT )
   set( CMAKE_SYSTEM_IGNORE_PATH /usr/include /usr/bin /usr/lib /usr/lib32
      /usr/lib64 )
endif()

# Call CMake's own FindJava.cmake. Note that the arguments created
# for this script by CMake pass through to the official script. So we don't
# need to give any extra arguments to this call.
find_package( Java )

# Restore CMAKE_MODULE_PATH:
set( CMAKE_MODULE_PATH ${_modulePathBackup} )
unset( _modulePathBackup )
set( CMAKE_SYSTEM_IGNORE_PATH )

# Massage the paths found by the system module:
if( Java_FOUND AND NOT GAUDI_ATLAS )

   # Set up the include path(s) for Java:
   set( _newIncludes )
   foreach( _inc ${Java_INCLUDE_DIRS} )
      set( _relocatableDir ${_inc} )
      _lcg_make_paths_relocatable( _relocatableDir )
      list( APPEND _newIncludes
         $<BUILD_INTERFACE:${_inc}>
         $<INSTALL_INTERFACE:${_relocatableDir}> )
      unset( _relocatableDir )
   endforeach()
   set( Java_INCLUDE_DIRS ${_newIncludes} )
   unset( _newIncludes )

   # Set up the libraries for Java:
   set( Java_LIBRARY_DIRS )
   set( _newLibraries )
   foreach( _lib ${Java_LIBRARIES} )
      get_filename_component( _libDir "${_lib}" PATH )
      set( _relocatableDir ${_libDir} )
      _lcg_make_paths_relocatable( _relocatableDir )
      list( APPEND Java_LIBRARY_DIRS
         $<BUILD_INTERFACE:${_libDir}>
         $<INSTALL_INTERFACE:${_relocatableDir}> )
      unset( _relocatableDir )
      set( _relocatableLib ${_lib} )
      _lcg_make_paths_relocatable( _relocatableLib )
      list( APPEND _newLibraries
         $<BUILD_INTERFACE:${_lib}>
         $<INSTALL_INTERFACE:${_relocatableLib}> )
      unset( _relocatableLib )
   endforeach()
   set( Java_LIBRARIES ${_newLibraries} )
   unset( _newLibraries )

endif()

# Set up the RPM dependency:
lcg_need_rpm( java FOUND_NAME Java VERSION_NAME JAVA )
