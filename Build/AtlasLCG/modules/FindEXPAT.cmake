# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: FindEXPAT.cmake 719980 2016-01-27 08:38:07Z krasznaa $
#
# File intercepting find_package(EXPAT) calls, and making the created
# paths relocatable.
#

# The LCG include(s):
include( LCGFunctions )

# Temporarily clean out CMAKE_MODULE_PATH, so that we could pick up
# FindEXPAT.cmake from CMake:
set( _modulePathBackup ${CMAKE_MODULE_PATH} )
set( CMAKE_MODULE_PATH )

# Make the code ignore the system path(s):
set( CMAKE_SYSTEM_IGNORE_PATH /usr/include /usr/bin /usr/lib /usr/lib32
   /usr/lib64 )

# Call CMake's own FindEXPAT.cmake. Note that the arguments created
# for this script by CMake pass through to the official script. So we don't
# need to give any extra arguments to this call.
find_package( EXPAT )

# Restore CMAKE_MODULE_PATH:
set( CMAKE_MODULE_PATH ${_modulePathBackup} )
unset( _modulePathBackup )
set( CMAKE_SYSTEM_IGNORE_PATH )

# Massage the paths found by the system module:
if( EXPAT_FOUND AND NOT GAUDI_ATLAS )

   # Update the include directories:
   set( _newIncludes )
   foreach( _inc ${EXPAT_INCLUDE_DIRS} )
      set( _relocatableDir ${_inc} )
      _lcg_make_paths_relocatable( _relocatableDir )
      list( APPEND _newIncludes
         $<BUILD_INTERFACE:${_inc}>
         $<INSTALL_INTERFACE:${_relocatableDir}> )
      unset( _relocatableDir )
   endforeach()
   set( EXPAT_INCLUDE_DIRS ${_newIncludes} )
   unset( _newIncludes )

   # Update the libraries:
   set( _newLibs )
   set( EXPAT_LIBRARY_DIRS )
   foreach( _lib ${EXPAT_LIBRARIES} )
      set( _relocatableLib ${_lib} )
      _lcg_make_paths_relocatable( _relocatableLib )
      list( APPEND _newLibs
         $<BUILD_INTERFACE:${_lib}>
         $<INSTALL_INTERFACE:${_relocatableLib}> )
      unset( _relocatableLib )
      get_filename_component( _dir ${_lib} PATH )
      set( _relocatableDir ${_dir} )
      _lcg_make_paths_relocatable( _relocatableDir )
      list( APPEND EXPAT_LIBRARY_DIRS
         $<BUILD_INTERFACE:${_dir}>
         $<INSTALL_INTERFACE:${_relocatableDir}> )
      unset( _relocatableDir )
      unset( _dir )
   endforeach()
   set( EXPAT_LIBRARIES ${_newLibs} )
   unset( _newLibs )
   list( REMOVE_DUPLICATES EXPAT_LIBRARY_DIRS )

endif()

# Set up the RPM dependency:
lcg_need_rpm( expat )
