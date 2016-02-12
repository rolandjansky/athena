# $Id: CMakeLists.txt 723562 2016-02-12 11:10:57Z krasznaa $
#
# CMake configuration file describing the build and installation of the
# Coin3D-simage library.
#

# The name of the package:
atlas_subdir( AtlasSimage )

# In release recompilation mode finish here:
if( ATLAS_RELEASE_MODE )
   return()
endif()

# Build the library for the build area:
ExternalProject_Add( AtlasSimage
   PREFIX ${CMAKE_BINARY_DIR}
   URL ${CMAKE_CURRENT_SOURCE_DIR}/src/Coin3D-simage-2c958a61ea8b.zip
   CONFIGURE_COMMAND ./configure
   --prefix=${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM}
   BUILD_IN_SOURCE 1
   BUILD_COMMAND make
   INSTALL_COMMAND make install
   )
add_dependencies( Package_AtlasSimage AtlasSimage )

# Install the package:
install( DIRECTORY ${CMAKE_INCLUDE_OUTPUT_DIRECTORY}/
   DESTINATION include
   USE_SOURCE_PERMISSIONS
   FILES_MATCHING PATTERN "*simage*" )
install( DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/
   DESTINATION ${CMAKE_INSTALL_LIBDIR}
   USE_SOURCE_PERMISSIONS
   FILES_MATCHING PATTERN "*simage*" )
