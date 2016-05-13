# Declare the name of the package:
atlas_subdir( TwissFiles )

# If we are in release rebuilding mode, stop here:
if( ATLAS_RELEASE_MODE )
   return()
endif()

# Find the ATLAS TwissFiles area:
if( NOT SITEROOT )
   if( ENV{SITEROOT} )
      set( SITEROOT $ENV{SITEROOT} )
   else()
      set( SITEROOT "/afs/cern.ch" )
   endif()
endif()
if( NOT ATLAS_TWISSFILES )
   if( ENV{ATLAS_TWISSFILES} )
      set( ATLAS_TWISSFILES $ENV{ATLAS_TWISSFILES} )
   else()
      set( ATLAS_TWISSFILES "${SITEROOT}/atlas/offline/external/TwissFiles" )
   endif()
endif()

# The current release data version:
set( TwissFiles_native_version "v003" )

# Set up a dependency on the separately built RPM file holding this
# data:
set_property( GLOBAL APPEND PROPERTY ATLAS_EXTERNAL_RPMS
   "ATLAS_TwissFiles_${TwissFiles_native_version}" )

# Configure the environment setup module:
configure_file(
   ${CMAKE_CURRENT_SOURCE_DIR}/TwissFilesEnvironmentConfig.cmake.in
   ${CMAKE_CURRENT_BINARY_DIR}/TwissFilesEnvironmentConfig.cmake
   @ONLY )
# And now "find" it:
set( TwissFilesEnvironment_DIR ${CMAKE_CURRENT_BINARY_DIR}
   CACHE PATH "Location of TwissFilesEnvironmentConfig.cmake" )
find_package( TwissFilesEnvironment )
