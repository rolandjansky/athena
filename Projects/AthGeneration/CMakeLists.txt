# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Set up the project.
cmake_minimum_required( VERSION 3.6 )
file( READ ${CMAKE_SOURCE_DIR}/version.txt _version )
string( STRIP ${_version} _version )
project( AthGeneration VERSION ${_version} LANGUAGES C CXX Fortran )
unset( _version )

# Configure flake8:
set( ATLAS_FLAKE8 flake8_atlas --select ATL,F,E101,E7,E9,W6
                               --ignore ATL238,ATL9,E701,E702,E704,E741
                               --enable-extensions ATL902
   CACHE STRING "Default flake8 command" )
set( ATLAS_PYTHON_CHECKER ${ATLAS_FLAKE8} --filterFiles AthenaConfiguration
   CACHE STRING "Python checker command to run during Python module compilation" )

# Find the ATLAS CMake code:
find_package( AtlasCMake QUIET )

# Find the base project(s):
find_package( AthGenerationExternals REQUIRED )
find_package( Gaudi REQUIRED )

# Additional externals needed for the build/runtime:
find_package( Xrootd )
find_package( GSL )
find_package( Davix )
find_package( PNG )
find_package( BLAS )
find_package( AIDA )
find_package( VDT )

# Set the project into "GENERATIONBASE mode".
set( GENERATIONBASE TRUE CACHE BOOL
   "Flag specifying that this is a generation release build" )
if( GENERATIONBASE )
   add_definitions( -DGENERATIONBASE )
endif()

# Set up where to find the AthenaPoolUtilitiesTest CMake code.
set( AthenaPoolUtilitiesTest_DIR
   "${CMAKE_SOURCE_DIR}/../../Database/AthenaPOOL/AthenaPoolUtilities/cmake"
   CACHE PATH "Directory holding the AthenaPoolUtilititesTest module" )

# Set up where to find the xAODUtilities CMake code.
set( xAODUtilities_DIR
   "${CMAKE_SOURCE_DIR}/../../Event/xAOD/xAODCore/cmake"
   CACHE PATH "Directory holding the xAODUtilities module" )

# Disable the usage of the --no-undefined linker flag:
atlas_disable_no_undefined()

# Set up CTest:
atlas_ctest_setup()

# Set up the "ATLAS project".
atlas_project( USE AthGenerationExternals ${AthGenerationExternals_VERSION}
   PROJECT_ROOT ${CMAKE_SOURCE_DIR}/../../ )

# Generate the environment setup for the externals, to be used during the build:
lcg_generate_env( SH_FILE ${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM}/env_setup.sh )

# Generate replacement rules for the installed paths:
set( _replacements )
if( NOT "$ENV{NICOS_PROJECT_HOME}" STREQUAL "" )
   get_filename_component( _buildDir $ENV{NICOS_PROJECT_HOME} PATH )
   list( APPEND _replacements ${_buildDir} "\${AthGeneration_DIR}/../../../.." )
endif()
if( NOT "$ENV{TDAQ_RELEASE_BASE}" STREQUAL "" )
   list( APPEND _replacements $ENV{TDAQ_RELEASE_BASE}
      "\${TDAQ_RELEASE_BASE}" )
endif()

# Now generate and install the installed setup files:
lcg_generate_env( SH_FILE ${CMAKE_BINARY_DIR}/env_setup_install.sh
   REPLACE ${_replacements} )
install( FILES ${CMAKE_BINARY_DIR}/env_setup_install.sh
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

# Install graphviz output if available:
install( FILES ${CMAKE_BINARY_DIR}/packages.dot
   DESTINATION . OPTIONAL )

# Package up the release using CPack:
atlas_cpack_setup()
