# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: FindQt4.cmake 728694 2016-03-09 15:42:56Z krasznaa $
#
# This file is here to intercept find_package(Qt4) calls, and
# massage the paths produced by the system module, to make them relocatable.
#

# The LCG include(s):
include( LCGFunctions )

# Temporarily clean out CMAKE_MODULE_PATH, so that we could pick up
# FindQt4.cmake from CMake:
set( _modulePathBackup ${CMAKE_MODULE_PATH} )
set( CMAKE_MODULE_PATH )

# Make the code ignore the system path(s):
set( CMAKE_SYSTEM_IGNORE_PATH /usr/include /usr/bin /usr/lib /usr/lib32
   /usr/lib64 )

# Call CMake's own FindQt4.cmake. Note that the arguments created
# for this script by CMake pass through to the official script. So we don't
# need to give any extra arguments to this call.
find_package( Qt4 )

# Restore CMAKE_MODULE_PATH:
set( CMAKE_MODULE_PATH ${_modulePathBackup} )
unset( _modulePathBackup )
set( CMAKE_SYSTEM_IGNORE_PATH )

# Massage the paths found by the system module:
if( Qt4_FOUND AND NOT GAUDI_ATLAS )

   # Set up the include path(s) for Qt4:
   set( _relocatableInclude ${QT_INCLUDE_DIR} )
   _lcg_make_paths_relocatable( _relocatableInclude )
   set( QT4_INCLUDE_DIR
      $<BUILD_INTERFACE:${QT_INCLUDE_DIR}>
      $<INSTALL_INTERFACE:${_relocatableInclude}> )
   unset( _relocatableInclude )

   set( _newIncludes )
   foreach( _inc ${QT_INCLUDES} )
      set( _relocatableDir ${_inc} )
      _lcg_make_paths_relocatable( _relocatableDir )
      list( APPEND _newIncludes
         $<BUILD_INTERFACE:${_inc}>
         $<INSTALL_INTERFACE:${_relocatableDir}> )
      unset( _relocatableDir )
   endforeach()
   set( QT4_INCLUDE_DIRS ${_newIncludes} )
   unset( _newIncludes )

   # Set up the libraries for Qt4:
   set( _relocatableLibDir ${QT_LIBRARY_DIR} )
   _lcg_make_paths_relocatable( _relocatableLibDir )
   set( QT4_LIBRARY_DIRS
      $<BUILD_INTERFACE:${QT_LIBRARY_DIR}>
      $<INSTALL_INTERFACE:${_relocatableLibDir}> )
   unset( _relocatableLibDir )

   set( QT4_LIBRARIES )
   foreach( _component ${Qt4_FIND_COMPONENTS} )
      # Turn the component name into all-caps:
      string( TOUPPER "${_component}" _upperComponent )
      # If the library was found, set it up:
      if( QT_${_upperComponent}_FOUND )
         set( QT4_${_upperComponent}_FOUND TRUE )
         set( QT4_${_upperComponent}_LIBRARY ${QT_${_upperComponent}_LIBRARY} )
         list( APPEND QT4_LIBRARIES ${QT_${_upperComponent}_LIBRARIES} )
      endif()
   endforeach()

endif()

# Set up the RPM dependency:
lcg_need_rpm( Qt FOUND_NAME Qt4 VERSION_NAME QT )
