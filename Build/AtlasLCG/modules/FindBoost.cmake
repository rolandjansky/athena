# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: FindBoost.cmake 772148 2016-09-07 15:04:09Z krasznaa $
#
# This file is here to intercept find_package(Boost) calls, and
# massage the paths produced by the system module, to make them relocatable.
#

# The LCG include(s):
include( LCGFunctions )

# If Boost was already found, then silence the module:
if( Boost_FOUND )
   set( Boost_FIND_QUIETLY TRUE )
endif()

# Make sure that Boost is not searched for using CMAKE_PREFIX_PATH, looking
# for BoostConfig.cmake. As that would take precedence over the setting made
# using BOOST_ROOT.
if( BOOST_ROOT )
   set( Boost_NO_BOOST_CMAKE TRUE )
   set( Boost_NO_SYSTEM_PATHS TRUE )
   set( CMAKE_SYSTEM_IGNORE_PATH /usr/include /usr/bin /usr/lib /usr/lib32
      /usr/lib64 /lib /lib32 /lib64 )
endif()

# Temporarily clean out CMAKE_MODULE_PATH, so that we could pick up
# FindBoost.cmake from CMake:
set( _modulePathBackup ${CMAKE_MODULE_PATH} )
set( CMAKE_MODULE_PATH )

# Call CMake's own FindBoost.cmake. Note that the arguments created
# for this script by CMake pass through to the official script. So we don't
# need to give any extra arguments to this call.
find_package( Boost )

# Restore CMAKE_MODULE_PATH:
set( CMAKE_MODULE_PATH ${_modulePathBackup} )
unset( _modulePathBackup )
set( CMAKE_SYSTEM_IGNORE_PATH )

# Massage the paths found by the system module:
if( Boost_FOUND AND NOT GAUDI_ATLAS )

   # Update the include directories:
   set( _newIncludes )
   foreach( _inc ${Boost_INCLUDE_DIRS} )
      set( _relocatableDir ${_inc} )
      _lcg_make_paths_relocatable( _relocatableDir )
      list( APPEND _newIncludes
         $<BUILD_INTERFACE:${_inc}>
         $<INSTALL_INTERFACE:${_relocatableDir}> )
      unset( _relocatableDir )
   endforeach()
   set( Boost_INCLUDE_DIRS ${_newIncludes} )
   unset( _newIncludes )

   # Update the libraries:
   set( _newLibs )
   foreach( _lib ${Boost_LIBRARIES} )
      set( _relocatableLib ${_lib} )
      _lcg_make_paths_relocatable( _relocatableLib )
      list( APPEND _newLibs
         $<BUILD_INTERFACE:${_lib}>
         $<INSTALL_INTERFACE:${_relocatableLib}> )
      unset( _relocatableLib )
   endforeach()
   set( Boost_LIBRARIES ${_newLibs} )
   unset( _newLibs )

   # Update the library directories:
   set( _newLibDirs )
   foreach( _libDir ${Boost_LIBRARY_DIRS} )
      set( _relocatableDir ${_libDir} )
      _lcg_make_paths_relocatable( _relocatableDir )
      list( APPEND _newLibDirs
         $<BUILD_INTERFACE:${_libDir}>
         $<INSTALL_INTERFACE:${_relocatableDir}> )
      unset( _relocatableDir )
   endforeach()
   set( Boost_LIBRARY_DIRS ${_newLibDirs} )
   unset( _newLibDirs )

   # Update the component variables:
   foreach( _component ${Boost_FIND_COMPONENTS} )
      if( Boost_${_component}_FOUND )
         set( _relocatableLib ${Boost_${_component}_LIBRARY} )
         _lcg_make_paths_relocatable( _relocatableLib )
         set( Boost_${_component}_LIBRARY
            $<BUILD_INTERFACE:${Boost_${_component}_LIBRARY}>
            $<INSTALL_INTERFACE:${_relocatableLib}> )
         unset( _relocatableLib )
      endif()
   endforeach()

endif()

# Add the appropriate thread library to the library list.
# As many ATLAS packages depend on Boost code that internally
# depends on pthread.
find_package( Threads )
if( CMAKE_THREAD_LIBS_INIT )
   list( APPEND Boost_LIBRARIES ${CMAKE_THREAD_LIBS_INIT} )
endif()

# Adding additional link options for Boost libraries cf ATLINFR-1150
lcg_os_id( _os _isValid )
if( _isValid AND "${_os}" STREQUAL "centos7" )
   list( APPEND Boost_LIBRARIES "-Wl,--copy-dt-needed-entries" )
endif()
unset( _os )
unset( _isValid )

# Set up the RPM dependency:
lcg_need_rpm( Boost )
