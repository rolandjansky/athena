# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# $Id: FindJavaSDK.cmake 711151 2015-11-27 12:00:55Z krasznaa $
#
# - Locate JavaSDK
# Defines:
#
#  JAVASDK_FOUND
#  JAVASDK_INCLUDE_DIR
#  JAVASDK_INCLUDE_DIRS (not cached)
#  JAVASDK_LIBRARIES
#
# Can be steered using JAVASDK_ROOT.
#

# Include the helper code:
include( AtlasInternals )

# Declare the module:
atlas_external_module( NAME JavaSDK
   INCLUDE_SUFFIXES include INCLUDE_NAMES jni.h )
if( JAVASDK_INCLUDE_DIR )
   list( APPEND JAVASDK_INCLUDE_DIRS ${JAVASDK_INCLUDE_DIR}/linux )
endif()

# Handle the standard find_package arguments:
include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( JavaSDK DEFAULT_MSG JAVASDK_INCLUDE_DIR )
mark_as_advanced( JAVASDK_FOUND JAVASDK_INCLUDE_DIR JAVASDK_INCLUDE_DIRS )
