# $Id: CMakeLists.txt 744427 2016-05-03 11:50:05Z krasznaa $
#
# CMake configuration file for the PathResolver package.
#

# Declare the package name:
atlas_subdir( PathResolver )

# Extra dependencies based on which environment we are in:
if( NOT XAOD_STANDALONE )
   set( extra_deps GaudiKernel )
   set( extra_libs GaudiKernel )
endif()

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthToolSupport/AsgTools
   Control/CxxUtils
   PRIVATE
   ${extra_deps} )

# External dependencies:
find_package( Boost COMPONENTS filesystem )
find_package( ROOT COMPONENTS RIO Core )

# Component(s) in the package:
atlas_add_library( PathResolver
   Root/*.cxx
   PUBLIC_HEADERS PathResolver
   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES AsgTools
   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} ${extra_libs} )

atlas_add_dictionary( PathResolverDict
   PathResolver/PathResolverDict.h PathResolver/selection.xml
   LINK_LIBRARIES PathResolver )

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_scripts( python/showReleaseNotes.py )

# Set up a helper environment variable for the test(s):
set( _dpath "${CMAKE_CURRENT_SOURCE_DIR}/test" )
set( _dpath "${CMAKE_CURRENT_SOURCE_DIR}/test/B:${_dpath}" )
set( _dpath "${CMAKE_CURRENT_SOURCE_DIR}/test/A:${_dpath}" )

# Test(s) in the package:
atlas_add_test( PathResolver_test
   SOURCES test/PathResolver_test.cxx
   LINK_LIBRARIES PathResolver
   ENVIRONMENT "DATAPATH=${_dpath}:$ENV{DATAPATH}" )

# Skip the runtime environment setup in release rebuilding mode:
if( ATLAS_RELEASE_MODE )
   return()
endif()

# Set up the general runtime environment:
set( PathResolverEnvironment_DIR ${CMAKE_CURRENT_SOURCE_DIR}
   CACHE PATH "Location of PathResolverEnvironment.cmake" )
find_package( PathResolverEnvironment )
