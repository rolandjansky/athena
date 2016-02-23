# $Id#
#
# Configuration for building gperftools as part of the offline software build.
#

# Set the name of the package:
atlas_subdir( AtlasGPerfTools )

# In release rebuild mode stop here:
if( ATLAS_RELEASE_MODE )
   return()
endif()

# Set up the build of gperftools for the build area:
ExternalProject_Add( GPerfTools
   PREFIX ${CMAKE_BINARY_DIR}
   URL /afs/cern.ch/atlas/offline/external/TARs/gperftools-2.4.tar.gz
   INSTALL_DIR ${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM}
   BUILD_IN_SOURCE 1
   CONFIGURE_COMMAND ./configure --enable-frame-pointers
   --prefix=${CMAKE_CURRENT_BINARY_DIR}/build
   LDFLAGS=-L${libunwind_ROOT}/lib
   CXXFLAGS=-I${libunwind_ROOT}/include
   BUILD_COMMAND make
   COMMAND make install
   INSTALL_COMMAND ${CMAKE_COMMAND} -E copy_directory
   ${CMAKE_CURRENT_BINARY_DIR}/build/ <INSTALL_DIR>
   )
add_dependencies( Package_AtlasGPerfTools GPerfTools )

# Set up its installation:
install( DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/build/
   DESTINATION . USE_SOURCE_PERMISSIONS )

# Install the package's scripts:
atlas_install_scripts( bin/atl-gpt-* )

# Configure the environment setup module:
configure_file(
   ${CMAKE_CURRENT_SOURCE_DIR}/AtlasGPerfToolsEnvironmentConfig.cmake.in
   ${CMAKE_CURRENT_BINARY_DIR}/AtlasGPerfToolsEnvironmentConfig.cmake
   @ONLY )

# Set up the environment variable needed by the package:
set( AtlasGPerfToolsEnvironment_DIR ${CMAKE_CURRENT_BINARY_DIR}
   CACHE PATH "Location of AtlasGPergToolsEnvironmentConfig.cmake" )
find_package( AtlasGPerfToolsEnvironment )
