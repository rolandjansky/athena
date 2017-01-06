# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: Findtdaq.cmake 790478 2016-12-19 13:20:23Z krasznaa $
#
# Try to find TDAQ
# Defines:
#  - TDAQ_FOUND
#  - TDAQ_INCLUDE_DIR
#  - TDAQ_INCLUDE_DIRS
#  - TDAQ_<component>_FOUND
#  - TDAQ_<component>_LIBRARY
#  - TDAQ_LIBRARIES
#  - TDAQ_LIBRARY_DIRS
#  - TDAQ_PYTHON_PATH
#  - TDAQ_BINARY_PATH
#  - TDAQ_DB_PATH
#
# Can be steered by TDAQ_ROOT.
#

# Include the helper code:
include( AtlasInternals )

# Declare the module:
atlas_external_module( NAME tdaq
   INCLUDE_SUFFIXES installed/include INCLUDE_NAMES RunControl/RunControl.h
   LIBRARY_SUFFIXES installed/$ENV{CMTCONFIG}/lib
   installed/${ATLAS_PLATFORM}/lib
   COMPULSORY_COMPONENTS ipc )

# Add the platform specific header directory, if the platform agnostic directory
# was found:
if( TDAQ_INCLUDE_DIRS )
   list( APPEND TDAQ_INCLUDE_DIRS
      ${TDAQ_ROOT}/installed/${ATLAS_PLATFORM}/include )
endif()

# Handle the standard find_package arguments:
include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( tdaq DEFAULT_MSG TDAQ_INCLUDE_DIR
   TDAQ_LIBRARIES )
mark_as_advanced( TDAQ_FOUND TDAQ_INCLUDE_DIR TDAQ_INCLUDE_DIRS TDAQ_LIBRARIES
   TDAQ_LIBRARY_DIRS )

# Set TDAQ specific environment:
if( TDAQ_FOUND )
   set( TDAQ_PYTHON_PATH ${TDAQ_ROOT}/installed/share/lib/python
      ${TDAQ_LIBRARY_DIRS} )
   if( "$ENV{CMTCONFIG}" STREQUAL "" )
      set( TDAQ_BINARY_PATH ${TDAQ_ROOT}/installed/${ATLAS_PLATFORM}/bin
         ${TDAQ_ROOT}/installed/share/bin )
   else()
      set( TDAQ_BINARY_PATH ${TDAQ_ROOT}/installed/$ENV{CMTCONFIG}/bin
         ${TDAQ_ROOT}/installed/share/bin )
   endif()
   set( TDAQ_ENVIRONMENT
      SET    TDAQ_DB_PATH ${TDAQ_ROOT}/installed/share/data
      APPEND TDAQ_DB_PATH ${TDAQ_ROOT}/installed/databases
      APPEND TDAQ_DB_PATH ${TDAQ_ROOT}/databases )

endif()

# When using tdaq, some macro definitions are made in the tdaq headers,
# and should not be set by the build system itself.
remove_definitions( -DHAVE_BOOL -DHAVE_DYNAMIC_CAST -DHAVE_NAMESPACES )

# Add the RPM dependencies:
if( TDAQ_FOUND )
   # Set up a dependency on the main tdaq RPM package:
   if ( TDAQ_PROJECT_NAME )
      set_property( GLOBAL APPEND PROPERTY ATLAS_EXTERNAL_RPMS
                   "${TDAQ_PROJECT_NAME}-${TDAQ_VERSION}_${ATLAS_PLATFORM}" )
   else()
      set_property( GLOBAL APPEND PROPERTY ATLAS_EXTERNAL_RPMS
      		    "tdaq-${TDAQ_VERSION}_${ATLAS_PLATFORM}" )
   endif()

#   foreach( comp ${tdaq_FIND_COMPONENTS} )
#      if( TDAQ_${comp}_FOUND )
#         set_property( GLOBAL APPEND PROPERTY ATLAS_EXTERNAL_RPMS
#            "tdaq-${TDAQ_VERSION}_${comp}_${ATLAS_PLATFORM}" )
#      endif()
#   endforeach()
endif()
