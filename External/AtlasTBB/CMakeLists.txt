# $Id: CMakeLists.txt 725898 2016-02-24 13:10:11Z krasznaa $
#
# CMake configuration for the build of TBB.
#

# Set the package's name:
atlas_subdir( AtlasTBB )

# In release recompilation mode finish here:
if( ATLAS_RELEASE_MODE )
   return()
endif()

# Configure two versions of the InstallTBB script. One for installing
# TBB in the build area, and one for installing it into the installation
# area.
set( SOURCE_DIR ${CMAKE_BINARY_DIR}/src/AtlasTBB )
set( INSTALL_DIR ${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM} )
configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/cmake/InstallTBB.cmake.in
   ${CMAKE_CURRENT_BINARY_DIR}/InstallTBB.build.cmake @ONLY )
set( INSTALL_DIR ${CMAKE_INSTALL_PREFIX} )
configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/cmake/InstallTBB.cmake.in
   ${CMAKE_CURRENT_BINARY_DIR}/InstallTBB.install.cmake @ONLY )

# Build TBB for the build area:
ExternalProject_Add( AtlasTBB
   PREFIX ${CMAKE_BINARY_DIR}
   URL ${CMAKE_CURRENT_SOURCE_DIR}/src/tbb42_20131003oss_src.tar.gz
   CONFIGURE_COMMAND true
   BUILD_IN_SOURCE 1
   COMMAND make
   INSTALL_COMMAND ${CMAKE_COMMAND}
   -Dinstall_dir=${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM}
   -Dsource_dir=<SOURCE_DIR>
   -P ${CMAKE_CURRENT_BINARY_DIR}/InstallTBB.build.cmake
   )
add_dependencies( Package_AtlasTBB AtlasTBB )

# Install TBB:
install( SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/InstallTBB.install.cmake )
