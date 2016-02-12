# $Id: CMakeLists.txt 723544 2016-02-12 10:12:55Z krasznaa $
#
# CMake configuration file building the BLAS library as part of the project
# build.
#

# The name of the package:
atlas_subdir( AtlasBlas )

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

# Configure BLAS's makefile:
configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/src/cmake-make.inc.in
   ${CMAKE_CURRENT_BINARY_DIR}/make.inc @ONLY )

# Build BLAS for the build area:
ExternalProject_Add( AtlasBlas
   PREFIX ${CMAKE_BINARY_DIR}
   URL ${CMAKE_CURRENT_SOURCE_DIR}/src/blas-20070405.tar.gz
   CONFIGURE_COMMAND ${CMAKE_COMMAND} -E copy
   ${CMAKE_CURRENT_BINARY_DIR}/make.inc make.inc
   BUILD_IN_SOURCE 1
   BUILD_COMMAND make
   INSTALL_COMMAND ${CMAKE_COMMAND} -E make_directory
   ${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM}/lib
   COMMAND ${CMAKE_COMMAND} -E copy libblas.a
   ${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM}/lib/
   )
add_dependencies( Package_AtlasBlas AtlasBlas )

# Install BLAS:
install( FILES ${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM}/lib/libblas.a
   DESTINATION ${CMAKE_INSTALL_LIBDIR} OPTIONAL )
