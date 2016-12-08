# $Id: CMakeLists.txt 788623 2016-12-08 14:09:55Z elmsheus $
#
# Package building FastJet as part of the offline software build.
#

# Set the package name:
atlas_subdir( AtlasFastJet )

# In release recompilation mode stop here:
if( ATLAS_RELEASE_MODE )
   return()
endif()

# Decide whether to request debug symbols from the build:
if( "${CMAKE_BUILD_TYPE}" STREQUAL "Release" OR
      "${CMAKE_BUILD_TYPE}" STREQUAL "MinSizeRel" OR
      "${CMAKE_BUILD_TYPE}" STREQUAL "" )
   set( _fastJetExtraConfig "--disable-debug" )
else()
   set( _fastJetExtraConfig "--enable-debug" )
endif()

# Set up the build of FastJet for the build area:
ExternalProject_Add( FastJet
   PREFIX ${CMAKE_BINARY_DIR}
   URL http://fastjet.fr/repo/fastjet-3.2.1.tar.gz
   INSTALL_DIR ${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM}
   CONFIGURE_COMMAND ./configure
   --prefix=${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/build
   --enable-shared --disable-static --enable-allcxxplugins
   ${_fastJetExtraConfig}
   BUILD_IN_SOURCE 1
   BUILD_COMMAND make
   COMMAND make install
   INSTALL_COMMAND ${CMAKE_COMMAND} -E copy_directory
   ${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/build/ <INSTALL_DIR> )
add_dependencies( Package_AtlasFastJet FastJet )

# Set up its installation:
install( DIRECTORY
   ${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/build/
   DESTINATION . USE_SOURCE_PERMISSIONS OPTIONAL )
