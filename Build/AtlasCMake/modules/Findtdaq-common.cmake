# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: Findtdaq-common.cmake 732986 2016-03-30 16:25:54Z krasznaa $
#
# Try to find TDAQ-COMMON.
# Defines:
#  - TDAQ-COMMON_FOUND
#  - TDAQ-COMMON_INCLUDE_DIR
#  - TDAQ-COMMON_INCLUDE_DIRS
#  - TDAQ-COMMON_<component>_FOUND
#  - TDAQ-COMMON_<component>_LIBRARY
#  - TDAQ-COMMON_LIBRARIES
#  - TDAQ-COMMON_LIBRARY_DIRS
#  - TDAQ-COMMON_PYTHON_PATH
#  - TDAQ-COMMON_BINARY_PATH
#
# Can be steered by TDAQ-COMMON_ROOT.
#

# Include the helper code:
include( AtlasInternals )

# Declare the module:
atlas_external_module( NAME tdaq-common
   INCLUDE_SUFFIXES installed/include INCLUDE_NAMES eformat/eformat.h
   LIBRARY_SUFFIXES installed/${ATLAS_PLATFORM}/lib
   COMPULSORY_COMPONENTS eformat ers )

# Handle the standard find_package arguments:
include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( tdaq-common DEFAULT_MSG
   TDAQ-COMMON_INCLUDE_DIR TDAQ-COMMON_LIBRARIES )
mark_as_advanced( TDAQ-COMMON_FOUND TDAQ-COMMON_INCLUDE_DIR
   TDAQ-COMMON_INCLUDE_DIRS TDAQ-COMMON_LIBRARIES TDAQ-COMMON_LIBRARY_DIRS )

# Set TDAQ specific environment:
if( TDAQ-COMMON_FOUND )
   set( TDAQ-COMMON_PYTHON_PATH ${TDAQ-COMMON_ROOT}/installed/share/lib/python
      ${TDAQ-COMMON_LIBRARY_DIRS} )
   set( TDAQ-COMMON_BINARY_PATH
      ${TDAQ-COMMON_ROOT}/installed/${ATLAS_PLATFORM}/bin
      ${TDAQ-COMMON_ROOT}/installed/share/bin )    
endif()

# Add the RPM dependencies:
if( TDAQ-COMMON_FOUND )
   # Set up a dependency on the main tdaq-common RPM package:
   set_property( GLOBAL APPEND PROPERTY ATLAS_EXTERNAL_RPMS
      "tdaq-common-${TDAQ-COMMON_VERSION}_TDAQCRelease_${ATLAS_PLATFORM}" )

#   foreach( comp ${tdaq-common_FIND_COMPONENTS} )
#      if( TDAQ-COMMON_${comp}_FOUND )
#         set_property( GLOBAL APPEND PROPERTY ATLAS_EXTERNAL_RPMS
#            "tdaq-common-${TDAQ-COMMON_VERSION}_${comp}_${ATLAS_PLATFORM}" )
#      endif()
#   endforeach()
endif()
