# $Id: CMakeLists.txt 725984 2016-02-24 17:55:24Z krasznaa $
#
# CMake configuration file building the LAPACK library.
#

# Declare the name of the package:
atlas_subdir( AtlasLapack )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC AtlasBlas )

# In release recompilation mode finish here:
if( ATLAS_RELEASE_MODE )
   return()
endif()

# Extend the CMAKE_Fortran_FLAGS variable with the build type specific
# flags, as we only use that variable in setting up the make.inc file.
if( NOT "${CMAKE_BUILD_TYPE}" STREQUAL "" )
   string( TOUPPER ${CMAKE_BUILD_TYPE} _type )
   set( CMAKE_Fortran_FLAGS
      "${CMAKE_Fortran_FLAGS} ${CMAKE_Fortran_FLAGS_${_type}}" )
   unset( _type )
endif()

# Configure LAPACK's makefile:
configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/src/cmake-make.inc.in
   ${CMAKE_CURRENT_BINARY_DIR}/make.inc @ONLY )

# Build LAPACK for the build area:
ExternalProject_Add( AtlasLapack
   PREFIX ${CMAKE_BINARY_DIR}
   URL ${CMAKE_CURRENT_SOURCE_DIR}/src/lapack-3.1.1.tar.gz
   INSTALL_COMMAND ${CMAKE_COMMAND} -E make_directory
   ${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM}/lib
   COMMAND ${CMAKE_COMMAND} -E copy liblapack.a
   ${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM}/lib/
   COMMAND ${CMAKE_COMMAND} -E copy libtmglib.a
   ${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM}/lib/
   CONFIGURE_COMMAND ${CMAKE_COMMAND} -E copy
   ${CMAKE_CURRENT_BINARY_DIR}/make.inc make.inc
   BUILD_COMMAND make lapack_install lib
   BUILD_IN_SOURCE 1
   )
add_dependencies( AtlasLapack AtlasBlas )
add_dependencies( Package_AtlasLapack AtlasLapack )

# Install LAPACK:
install( FILES ${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM}/lib/liblapack.a
   ${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM}/lib/libtmglib.a
   DESTINATION ${CMAKE_INSTALL_LIBDIR} OPTIONAL )
