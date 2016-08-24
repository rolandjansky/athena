# $Id: CMakeLists.txt 769864 2016-08-24 13:16:38Z krasznaa $
#
# Package building the FFTW library as part of the offline software build.
#

# Set the name of the package:
atlas_subdir( AtlasFFTW )

# In release recompilation mode stop here:
if( ATLAS_RELEASE_MODE )
   return()
endif()

# Decide whether to request debug symbols from the build:
set( _fftwExtraConfig )
if( "${CMAKE_BUILD_TYPE}" STREQUAL "Debug" )
   set( _fftwExtraConfig --enable-debug )
endif()

# Set up the build of FFTW for the build area:
ExternalProject_Add( FFTW
   PREFIX ${CMAKE_BINARY_DIR}
   URL ${CMAKE_CURRENT_SOURCE_DIR}/src/fftw-3.3.3.tar.gz
   URL_MD5 74b2a000d80646d3c0210efbeb9836ba
   INSTALL_DIR ${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM}
   CONFIGURE_COMMAND ./configure --prefix=${CMAKE_CURRENT_BINARY_DIR}/build
   --enable-shared --enable-float ${_fftwExtraConfig}
   BUILD_IN_SOURCE 1
   BUILD_COMMAND make
   COMMAND make install
   INSTALL_COMMAND ${CMAKE_COMMAND} -E copy_directory
   ${CMAKE_CURRENT_BINARY_DIR}/build/ <INSTALL_DIR>
   )
add_dependencies( Package_AtlasFFTW FFTW )

# Set up its installation:
install( DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/build/
   DESTINATION . USE_SOURCE_PERMISSIONS OPTIONAL )
