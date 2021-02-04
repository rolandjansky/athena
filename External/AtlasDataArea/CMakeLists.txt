# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Declare the name of the package:
atlas_subdir( AtlasDataArea )

# Find the ATLAS ReleaseData area:
if( NOT SITEROOT )
   if( NOT "$ENV{SITEROOT}" STREQUAL "" )
      set( SITEROOT "$ENV{SITEROOT}" )
   else()
      set( SITEROOT "/afs/cern.ch" )
   endif()
endif()
if( NOT ATLAS_RELEASEDATA )
   if( NOT "$ENV{ATLAS_RELEASEDATA}" STREQUAL "" )
      set( ATLAS_RELEASEDATA "$ENV{ATLAS_RELEASEDATA}" )
   else()
      set( ATLAS_RELEASEDATA "${SITEROOT}/atlas/offline/ReleaseData" )
   endif()
endif()

# The current release data version:
set( AtlasDataArea_native_version "v20" )

# Set up a dependency on the separately built RPM file holding this
# data:
set_property( GLOBAL APPEND PROPERTY ATLAS_EXTERNAL_RPMS
   "ATLAS_ReleaseData_${AtlasDataArea_native_version}" )

# Configure the environment setup module:
configure_file(
   ${CMAKE_CURRENT_SOURCE_DIR}/AtlasDataAreaEnvironmentConfig.cmake.in
   ${CMAKE_CURRENT_BINARY_DIR}/AtlasDataAreaEnvironmentConfig.cmake
   @ONLY )
# And now "find" it:
set( AtlasDataAreaEnvironment_DIR ${CMAKE_CURRENT_BINARY_DIR}
   CACHE PATH "Location of AtlasDataAreaEnvironmentConfig.cmake" )
find_package( AtlasDataAreaEnvironment )
