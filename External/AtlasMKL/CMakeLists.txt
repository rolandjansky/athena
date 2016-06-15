# $Id: CMakeLists.txt 754999 2016-06-15 12:35:14Z krasznaa $
#
# File describing how to acquire the Intel math libraries for the
# build of an ATLAS project.
#

# Declare the name of the package:
atlas_subdir( AtlasMKL )

# The libraries are only available for 64-bit linux. On everything
# else just bail here.
if( NOT "${CMAKE_SYSTEM_NAME}" STREQUAL "Linux" )
   return()
endif()
if( NOT "${CMAKE_SIZEOF_VOID_P}" EQUAL "8" )
   return()
endif()

# Find the MKL libraries:
if( NOT ATLAS_EXTERNAL )
   if( NOT "$ENV{ATLAS_EXTERNAL}" STREQUAL "" )
      set( ATLAS_EXTERNAL $ENV{ATLAS_EXTERNAL} )
   else()
      set( ATLAS_EXTERNAL "/afs/cern.ch/atlas/offline/external" )
   endif()
endif()
set( _directory "${ATLAS_EXTERNAL}/MKL/IntelSoftware/linux/x86_64/xe2016" )
set( _directory "${_directory}/compilers_and_libraries/linux/lib/intel64" )

# Check if the directory is available:
if( NOT IS_DIRECTORY "${_directory}" )
   message( WARNING "Can't access directory: ${_directory}"
      "Intel math libraries are not set up" )
   return()
endif()

# Set up the installation of the libraries:
atlas_install_generic( ${_directory}/*
   DESTINATION lib
   EXECUTABLE )

# Configure the environment setup module:
configure_file(
   ${CMAKE_CURRENT_SOURCE_DIR}/AtlasMKLEnvironmentConfig.cmake.in
   ${CMAKE_CURRENT_BINARY_DIR}/AtlasMKLEnvironmentConfig.cmake
   @ONLY )

# Now set up the environment of the package:
set( AtlasMKLEnvironment_DIR ${CMAKE_CURRENT_BINARY_DIR}
   CACHE PATH "Location of AtlasMKLEnvironmentConfig.cmake" )
find_package( AtlasMKLEnvironment )
