# $Id: CMakeLists.txt 725779 2016-02-23 17:09:29Z krasznaa $
#
# Package building the dSFMT libraries as part of the ATLAS offline software
# build.
#

# The name of the package:
atlas_subdir( AtlasDSFMT )

# In release recompilation mode stop here:
if( ATLAS_RELEASE_MODE )
   return()
endif()

# Flags for the build:
set( _flags CMTCONFIG=${ATLAS_PLATFORM} CC=${CMAKE_C_COMPILER}
   AR=${CMAKE_AR} OUTDIR=${CMAKE_CURRENT_BINARY_DIR}/build )

# Build the package for the build area:
ExternalProject_Add( dSFMT
   PREFIX ${CMAKE_BINARY_DIR}
   URL ${CMAKE_CURRENT_SOURCE_DIR}/src/dSFMT-src-2.1.tar.gz
   INSTALL_DIR ${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM}
   PATCH_COMMAND ${CMAKE_COMMAND} -E copy
   ${CMAKE_CURRENT_SOURCE_DIR}/src/Makefile.atlas
   <SOURCE_DIR>/Makefile
   CONFIGURE_COMMAND ${CMAKE_COMMAND} -E echo
   "Configuring the build of dSFMT"
   BUILD_COMMAND make std-check ${_flags}
   COMMAND make sse2-check ${_flags}
   COMMAND make all ${_flags}
   INSTALL_COMMAND ${CMAKE_COMMAND} -E make_directory
   ${CMAKE_CURRENT_BINARY_DIR}/build/include
   COMMAND ${CMAKE_COMMAND} -E make_directory
   ${CMAKE_CURRENT_BINARY_DIR}/build/lib
   COMMAND make install ${_flags}
   COMMAND ${CMAKE_COMMAND} -E copy_directory
   ${CMAKE_CURRENT_BINARY_DIR}/build/ <INSTALL_DIR>
   BUILD_IN_SOURCE 1 )
add_dependencies( Package_AtlasDSFMT dSFMT )

# Install it:
install( DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/build/
   DESTINATION . USE_SOURCE_PERMISSIONS )
