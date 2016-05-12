# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: FindCoin3D.cmake 724137 2016-02-16 09:01:57Z krasznaa $
#
# This file is here to intercept find_package(Coin3D) calls, and
# massage the paths produced by the system module, to make them relocatable.
#

# The LCG include(s):
include( LCGFunctions )

# Silence the module if the external has already been found:
if( COIN3D_FOUND )
   set( Coin3D_FIND_QUIETLY TRUE )
endif()

# Temporarily clean out CMAKE_MODULE_PATH, so that we could pick up
# FindCoin3D.cmake from CMake:
set( _modulePathBackup ${CMAKE_MODULE_PATH} )
set( CMAKE_MODULE_PATH )

# Make the code ignore the system path(s):
set( CMAKE_SYSTEM_IGNORE_PATH /usr/include /usr/bin /usr/lib /usr/lib32
   /usr/lib64 )

# Call CMake's own FindCoin3D.cmake. Note that the arguments created
# for this script by CMake pass through to the official script. So we don't
# need to give any extra arguments to this call.
find_package( Coin3D )

# Restore CMAKE_MODULE_PATH:
set( CMAKE_MODULE_PATH ${_modulePathBackup} )
unset( _modulePathBackup )
set( CMAKE_SYSTEM_IGNORE_PATH )

# Massage the paths found by the system module:
if( COIN3D_FOUND AND NOT GAUDI_ATLAS )

   # Update the include directories:
   set( _newIncludes )
   foreach( _inc ${COIN3D_INCLUDE_DIRS} )
      set( _relocatableDir ${_inc} )
      _lcg_make_paths_relocatable( _relocatableDir )
      list( APPEND _newIncludes
         $<BUILD_INTERFACE:${_inc}>
         $<INSTALL_INTERFACE:${_relocatableDir}> )
      unset( _relocatableDir )
   endforeach()
   set( COIN3D_INCLUDE_DIRS ${_newIncludes} )
   unset( _newIncludes )

   # Update the libraries:
   set( _newLibs )
   set( COIN3D_LIBRARY_DIRS )
   foreach( _lib ${COIN3D_LIBRARIES} )
      set( _relocatableLib ${_lib} )
      _lcg_make_paths_relocatable( _relocatableLib )
      list( APPEND _newLibs
         $<BUILD_INTERFACE:${_lib}>
         $<INSTALL_INTERFACE:${_relocatableLib}> )
      unset( _relocatableLib )
      get_filename_component( _dir ${_lib} PATH )
      set( _relocatableDir ${_dir} )
      _lcg_make_paths_relocatable( _relocatableDir )
      list( APPEND COIN3D_LIBRARY_DIRS
         $<BUILD_INTERFACE:${_dir}>
         $<INSTALL_INTERFACE:${_relocatableDir}> )
      unset( _relocatableDir )
      unset( _dir )
   endforeach()
   set( COIN3D_LIBRARIES ${_newLibs} )
   unset( _newLibs )
   list( REMOVE_DUPLICATES COIN3D_LIBRARY_DIRS )

endif()

# Set up the RPM dependency:
lcg_need_rpm( coin3d )
