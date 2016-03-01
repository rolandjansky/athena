# $Id: FindGMock.cmake 727050 2016-03-01 14:21:44Z krasznaa $
#
# A simple module for finding the GoogleMock headers and libraries. It
# tries to mimic the behaviour of CMake's own FindGTest.cmake module,
# so have a look at that module for the definition of the different
# variables that you should expect from this module.
#

# This module will only work if AtlasCMake is being used...
include( AtlasInternals )

# Declare the module:
atlas_external_module( NAME GMock
   INCLUDE_SUFFIXES include INCLUDE_NAMES gmock/gmock.h
   LIBRARY_SUFFIXES lib lib64
   DEFAULT_COMPONENTS gmock )

# Handle the standard find_package arguments:
include( FindPackageHandleStandardArgs )
find_package_handle_standard_args( GMock DEFAULT_MSG GMOCK_INCLUDE_DIR
   GMOCK_LIBRARIES )
mark_as_advanced( GMOCK_FOUND GMOCK_INCLUDE_DIR GMOCK_INCLUDE_DIRS
   GMOCK_LIBRARIES GMOCK_LIBRARY_DIRS )

# If the main GMock library was found, set up variables mimicking those
# configured by CMake's own FindGTest.cmake:
if( GMOCK_FOUND )
   find_library( GMOCK_MAIN_LIBRARIES
      NAMES gmock_main PATH_SUFFIXES lib lib64
      PATHS ${GMOCK_ROOT} )
endif()
