# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: FindCASTOR.cmake 718732 2016-01-20 12:30:12Z krasznaa $
#
# - Locate CASTOR libraries
# Defines:
#
#  CASTOR_FOUND
#  CASTOR_INCLUDE_DIR
#  CASTOR_INCLUDE_DIRS
#  CASTOR_<component>_LIBRARY
#  CASTOR_<component>_FOUND
#  CASTOR_LIBRARIES
#  CASTOR_LIBRARY_DIRS
#
# Can be steered by CASTOR_ROOT.
#

# The LCG include(s):
include( LCGFunctions )

# Declare the external module:
lcg_external_module( NAME CASTOR
   INCLUDE_SUFFIXES usr/include INCLUDE_NAMES shift.h
   EXTRA_OPTIONS NO_SYSTEM_ENVIRONMENT_PATH NO_CMAKE_SYSTEM_PATH )

# Reset the library lists:
set( CASTOR_LIBRARIES )
set( CASTOR_LIBRARY_DIRS )

# Always look at least for libshift:
if( NOT CASTOR_FIND_COMPONENTS )
   set( CASTOR_FIND_COMPONENTS shift )
endif()

# Find the requested component(s):
foreach( component ${CASTOR_FIND_COMPONENTS} )

   # libshift.so is the only one without the prefix 'castor'
   if( component STREQUAL shift )
      set( name ${component} )
   else()
      set( name castor${component} )
   endif()

   # The the requested library:
   find_library( _CASTOR_${component}_library NAMES ${name}
      PATH_SUFFIXES usr/lib64 PATHS ${CASTOR_ROOT}
      EXTRA_OPTIONS NO_SYSTEM_ENVIRONMENT_PATH NO_CMAKE_SYSTEM_PATH )
   if( _CASTOR_${component}_library )
      set( CASTOR_${component}_FOUND TRUE )
      set( _relocatableLib ${_CASTOR_${component}_library} )
      _lcg_make_paths_relocatable( _relocatableLib )
      string( SUBSTRING ${_relocatableLib} 1 -1 _relocatableLib )
      if( GAUDI_ATLAS )
         set( CASTOR_${component}_LIBRARY
            ${_CASTOR_${component}_library} )
      else()
         set( CASTOR_${component}_LIBRARY
            $<BUILD_INTERFACE:${_CASTOR_${component}_library}>
            $<INSTALL_INTERFACE:/${_relocatableLib}> )
      endif()
      list( APPEND CASTOR_LIBRARIES ${CASTOR_${component}_LIBRARY} )
      get_filename_component( _libdir ${_CASTOR_${component}_library} PATH )
      list( APPEND CASTOR_LIBRARY_DIRS ${_libdir} )
      unset( _relocatableLib )
      unset( _libdir )
   else()
      set( CASTOR_${component}_FOUND FALSE )
   endif()
   unset( _CASTOR_${component}_library )

   # Mark the component location as an advanced property:
   mark_as_advanced( CASTOR_${component}_LIBRARY
      CASTOR_${component}_FOUND )
endforeach()

# Remove duplicates from the directories:
if( CASTOR_LIBRARY_DIRS )
   list( REMOVE_DUPLICATES CASTOR_LIBRARY_DIRS )
endif()

# Handle the standard find_package arguments:
include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( CASTOR DEFAULT_MSG CASTOR_INCLUDE_DIR
   CASTOR_LIBRARIES )
mark_as_advanced( CASTOR_FOUND CASTOR_INCLUDE_DIR CASTOR_INCLUDE_DIRS
   CASTOR_LIBRARIES CASTOR_LIBRARY_DIRS )

# Set up the RPM dependency:
lcg_need_rpm( CASTOR )
