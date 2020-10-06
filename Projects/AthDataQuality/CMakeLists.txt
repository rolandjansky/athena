# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Set up the project.
cmake_minimum_required( VERSION 3.6 )
file( READ ${CMAKE_SOURCE_DIR}/version.txt _version )
string( STRIP ${_version} _version )
project( AthDataQuality VERSION ${_version} LANGUAGES CXX )
unset( _version )

# Set the version of tdaq-common to use for the build:
set( TDAQ-COMMON_VERSION "04-02-01" CACHE STRING
   "The version of tdaq-common to use for the build" )
set( TDAQ-COMMON_ATROOT
   "$ENV{TDAQ_RELEASE_BASE}/tdaq-common/tdaq-common-${TDAQ-COMMON_VERSION}"
   CACHE PATH "The directory to pick up tdaq-common from" )

# Pick up the AtlasCMake code:
find_package( AtlasCMake REQUIRED )

# Build the project against LCG:
set( LCG_VERSION_POSTFIX "python3"
   CACHE STRING "Version postfix for the LCG release to use" )
set( LCG_VERSION_NUMBER 98
   CACHE STRING "Version number for the LCG release to use" )
find_package( LCG ${LCG_VERSION_NUMBER} EXACT REQUIRED )

# External(s) needed at build/runtime:
find_package( Xrootd )
find_package( GSL )
find_package( PNG )
find_package( TBB )
find_package( TIFF )

# Make CMake find the project specific code we have here:
list( INSERT CMAKE_MODULE_PATH 0 ${CMAKE_SOURCE_DIR}/cmake )

# Set up CTest:
atlas_ctest_setup()

# Set up a work directory project:
atlas_project( PROJECT_ROOT ${CMAKE_SOURCE_DIR}/../../ )

# Generate the environment setup for the externals, to be used during the build:
lcg_generate_env( SH_FILE ${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM}/env_setup.sh )

# Generate replacement rules for the installed paths:
set( _replacements )
if( NOT "$ENV{NICOS_PROJECT_HOME}" STREQUAL "" )
   get_filename_component( _buildDir $ENV{NICOS_PROJECT_HOME} PATH )
   list( APPEND _replacements ${_buildDir}
      "\${AthDataQuality_DIR}/../../../.." )
endif()
if( NOT "$ENV{TDAQ_RELEASE_BASE}" STREQUAL "" )
   list( APPEND _replacements $ENV{TDAQ_RELEASE_BASE}
      "\${TDAQ_RELEASE_BASE}" )
endif()

# Now generate and install the installed setup files:
lcg_generate_env( SH_FILE
   ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/env_setup_install.sh
   REPLACE ${_replacements} )
install( FILES ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/env_setup_install.sh
   DESTINATION . RENAME env_setup.sh )

# Configure and install the post-configuration file:
string( REPLACE "$ENV{TDAQ_RELEASE_BASE}" "\$ENV{TDAQ_RELEASE_BASE}"
   TDAQ-COMMON_ATROOT "${TDAQ-COMMON_ATROOT}" )
string( REPLACE "${TDAQ-COMMON_VERSION}" "\${TDAQ-COMMON_VERSION}"
   TDAQ-COMMON_ATROOT "${TDAQ-COMMON_ATROOT}" )
configure_file( ${CMAKE_SOURCE_DIR}/cmake/PreConfig.cmake.in
   ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/PreConfig.cmake @ONLY )
configure_file( ${CMAKE_SOURCE_DIR}/cmake/PostConfig.cmake.in
   ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/PostConfig.cmake @ONLY )
install( FILES
   ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/PreConfig.cmake
   ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/PostConfig.cmake
   DESTINATION ${CMAKE_INSTALL_CMAKEDIR} )

# Set up CPack:
atlas_cpack_setup()
