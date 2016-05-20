# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: FindGeant4.cmake 744789 2016-05-04 12:17:00Z krasznaa $
#
# Locate Geant4.
# Defines:
#  - GEANT4_FOUND
#  - GEANT4_INCLUDE_DIR
#  - GEANT4_INCLUDE_DIRS
#  - GEANT4_<component>_FOUND
#  - GEANT4_<component>_LIBRARY
#  - GEANT4_LIBRARIES
#  - GEANT4_LIBRARY_DIRS
#
# Can be steered by GEANT4_HOME_DIR + GEANT4_VERSION.
#

# Geant 4 is installed in a quite particular way. So the atlas_external_module
# helper code is of little use unfortunately. :-(

# Find the include path to Geant:
find_path( GEANT4_INCLUDE_DIR NAMES G4Version.hh
   PATH_SUFFIXES share/${GEANT4_VERSION}/include include/Geant4
   PATHS ${GEANT4_HOME_DIR} )
mark_as_advanced( GEANT4_INCLUDE_DIR )
set( GEANT4_INCLUDE_DIRS ${GEANT4_INCLUDE_DIR} )

# Try to extract the version of the found G4 instance:
if( GEANT4_INCLUDE_DIR )
   file( READ "${GEANT4_INCLUDE_DIR}/G4Version.hh" _g4version_h
      LIMIT 20000 )
   set( _version_regex
      ".*#define G4VERSION_TAG \".Name. ([^\"]*) .\".*" )
   if( "${_g4version_h}" MATCHES ${_version_regex} )
      set( G4_VERSION ${CMAKE_MATCH_1} CACHE INTERNAL
         "Detected version of Geant4" )
   else()
      set( G4_VERSION "${GEANT4_VERSION}" CACHE INTERNAL
         "Specified version of Geant4" )
   endif()
else()
   set( G4_VERSION "${GEANT4_VERSION}" CACHE INTERNAL
      "Specified version of Geant4" )
endif()

# Select which libraries to use:
list( APPEND Geant4_FIND_COMPONENTS
   G4global G4event G4track G4particles G4tracking G4geometry G4digits_hits
   G4graphics_reps G4intercoms G4materials G4run G4Tree G4visHepRep
   G4readout G4processes G4physicslists G4interfaces G4vis_management G4modeling
   G4persistency )
list( REMOVE_DUPLICATES Geant4_FIND_COMPONENTS )

# Find all the libraries:
set( GEANT4_LIBRARIES )
set( GEANT4_LIBRARY_DIRS )
foreach( _component ${Geant4_FIND_COMPONENTS} )
   # Search for the requested library:
   find_library( GEANT4_${_component}_LIBRARY NAMES ${_component}
      PATH_SUFFIXES specific/${ATLAS_PLATFORM}/${GEANT4_VERSION}/lib/Linux-g++
                    lib
      PATHS ${GEANT4_HOME_DIR} )
   # Deal with the search results:
   if( GEANT4_${_component}_LIBRARY )
      set( GEANT4_${_component}_FOUND TRUE )
      list( APPEND GEANT4_FOUND_COMPONENTS ${_component} )
      list( APPEND GEANT4_LIBRARIES ${GEANT4_${_component}_LIBRARY} )
      get_filename_component( _libdir ${GEANT4_${_component}_LIBRARY} PATH )
      list( APPEND GEANT4_LIBRARY_DIRS ${_libdir} )
      unset( _libdir )
   else()
      set( GEANT4_${_component}_FOUND FALSE )
   endif()
   # Mark the component location as an advanced property:
   mark_as_advanced( GEANT4_${_component}_LIBRARY
      GEANT4_${_component}_FOUND )
endforeach()
if( GEANT4_LIBRARY_DIRS )
   list( REMOVE_DUPLICATES GEANT4_LIBRARY_DIRS )
   list( GET GEANT4_LIBRARY_DIRS 0 GEANT4_LIBRARY_DIR )
endif()

# Handle the standard find_package arguments:
include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( Geant4 REQUIRED_VARS GEANT4_INCLUDE_DIR
   GEANT4_LIBRARIES VERSION_VAR G4_VERSION )
mark_as_advanced( GEANT4_FOUND )

# Look for the geant4-config script:
find_file( _g4_config NAMES geant4-config
   PATH_SUFFIXES specific/${ATLAS_PLATFORM}/${GEANT4_VERSION}/bin
   bin
   PATHS ${GEANT4_HOME_DIR} )
mark_as_advanced( _g4_config )
if( _g4_config )
   # Not finding it is not a problem. We just get extra information
   # out of it when we find it. If we found it, ask it for all the
   # compiler flags needed to use the G4 headers:
   execute_process( COMMAND ${_g4_config} --cflags
      TIMEOUT 30
      OUTPUT_VARIABLE _cflags )
   # Filter out just the macro definitions from it:
   string( REGEX MATCHALL "-D[^ ]+"
      _definitions "${_cflags}" )
   # And now apply these:
   if( NOT "${_definitions}" STREQUAL "" )
      add_definitions( ${_definitions} )
      set( GEANT4_DEFINITIONS ${_definitions} )
   endif()
   # Now clean up:
   unset( _cflags )
   unset( _definitions )
endif()
unset( _g4_config )

# If Geant4 was found, set up some environment variables for it:
if( GEANT4_FOUND )

   # Default location of the data files. To be overridden by AtlasSetup:
   set( _g4home "/afs/cern.ch/atlas/offline/external/geant4/releases" )
   # Main directory of the headers/libraries:
   get_filename_component( _helper "${GEANT4_INCLUDE_DIR}" PATH )
   get_filename_component( _geant4 "${_helper}" PATH )
   # Location of the data files:
   set( _g4data "\${G4PATH}/share/\${G4VERS}/data" )
   # The G4 platform/system:
   if( APPLE )
      set( _g4system "Darwin-g++" )
   else()
      set( _g4system "Linux-g++" )
   endif()

   # Set the environment:
   set( GEANT4_ENVIRONMENT
      SET G4VERS "${G4_VERSION}"
      SET G4PATH "${_g4home}"
      SET GEANT4 "${_geant4}"
      SET Geant4_data "${_g4data}"
      SET G4INSTALL "${_geant4}"
      SET G4INCLUDE "${GEANT4_INCLUDE_DIR}"
      SET G4LIB "${GEANT4_LIBRARY_DIR}"
      SET G4SYSTEM "${_g4system}"
      SET G4LEVELGAMMADATA "${_g4data}/PhotonEvaporation"
      SET G4NEUTRONHPDATA  "${_g4data}/G4NDL"
      SET G4LEDATA         "${_g4data}/G4EMLOW"
      SET G4NEUTRONXSDATA  "${_g4data}/G4NEUTRONXS"
      SET G4SAIDXSDATA     "${_g4data}/G4SAIDDATA" )

   # Set up a dependency on the G4 data RPM:
   set_property( GLOBAL APPEND PROPERTY ATLAS_EXTERNAL_RPMS
      "ATLAS_Geant4Data_${G4_VERSION}" )

   # Clean up:
   unset( _g4system )
   unset( _geant4 )
   unset( _g4data )
   unset( _helper )
   unset( _g4home )

endif()
