# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: FindGSL.cmake 724137 2016-02-16 09:01:57Z krasznaa $
#
# This file is here to intercept find_package(GSL) calls, and massage
# the paths produced by the system module, to make them relocatable.
#

# The LCG function(s):
include( LCGFunctions )

# Ignore the system paths:
set( CMAKE_SYSTEM_IGNORE_PATH /usr/include /usr/bin /usr/lib /usr/lib32
   /usr/lib64 )

# Temporarily clean out CMAKE_MODULE_PATH, so that we could pick up
# FindGSL.cmake from CMake:
set( _modulePathBackup ${CMAKE_MODULE_PATH} )
set( CMAKE_MODULE_PATH )

# Call CMake's own FindGSL.cmake. Note that the arguments created
# for this script by CMake pass through to the official script. So we don't
# need to give any extra arguments to this call.
find_package( GSL )

# Restore the paths:
set( CMAKE_MODULE_PATH ${_modulePathBackup} )
unset( _modulePathBackup )
set( CMAKE_SYSTEM_IGNORE_PATH )

# Massage the paths found by the system module:
if( GSL_FOUND AND NOT GAUDI_ATLAS )

   # Update the include directories:
   set( _newIncludes )
   foreach( _inc ${GSL_INCLUDE_DIRS} )
      set( _relocatableDir ${_inc} )
      _lcg_make_paths_relocatable( _relocatableDir )
      list( APPEND _newIncludes
         $<BUILD_INTERFACE:${_inc}>
         $<INSTALL_INTERFACE:${_relocatableDir}> )
      unset( _relocatableDir )
   endforeach()
   set( GSL_INCLUDE_DIRS ${_newIncludes} )
   unset( _newIncludes )

   # Update the libraries:
   set( _newLibs )
   set( _newLibDirs )
   foreach( _lib ${GSL_LIBRARIES} )
      set( _relocatableLib ${_lib} )
      _lcg_make_paths_relocatable( _relocatableLib )
      list( APPEND _newLibs
         $<BUILD_INTERFACE:${_lib}>
         $<INSTALL_INTERFACE:${_relocatableLib}> )
      unset( _relocatableLib )
      get_filename_component( _libDir ${_lib} PATH )
      set( _relocatableDir ${_libDir} )
      _lcg_make_paths_relocatable( _relocatableDir )
      list( APPEND _newLibDirs
         $<BUILD_INTERFACE:${_libDir}>
         $<INSTALL_INTERFACE:${_relocatableDir}> )
      unset( _relocatableDir )
   endforeach()
   set( GSL_LIBRARIES ${_newLibs} )
   unset( _newLibs )
   set( GSL_LIBRARY_DIRS ${_newLibDirs} )
   unset( _newLibDirs )

endif()

# Set up the RPM dependency:
lcg_need_rpm( GSL )
