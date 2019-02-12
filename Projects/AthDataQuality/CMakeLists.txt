
# Set the minimum required CMake version:
cmake_minimum_required( VERSION 3.6 )

# Read in the project's version from a file called version.txt. But let it be
# overridden from the command line if necessary.
file( READ ${CMAKE_SOURCE_DIR}/version.txt _version )
string( STRIP ${_version} _version )
set( ATHDATAQUALITY_PROJECT_VERSION ${_version}
   CACHE STRING "Version of the AthDataQuality project to build" )
unset( _version )

# Set the version of tdaq-common to use for the build:
set( TDAQ-COMMON_VERSION "03-00-00" CACHE STRING
   "The version of tdaq-common to use for the build" )
set( TDAQ-COMMON_ATROOT
   "$ENV{TDAQ_RELEASE_BASE}/tdaq-common/tdaq-common-${TDAQ-COMMON_VERSION}"
   CACHE PATH "The directory to pick up tdaq-common from" )

# Pick up the AtlasCMake code:
find_package( AtlasCMake REQUIRED )

# Build the project against LCG:
set( LCG_VERSION_POSTFIX "a"
   CACHE STRING "Version postfix for the LCG release to use" )
set( LCG_VERSION_NUMBER 94
   CACHE STRING "Version number for the LCG release to use" )
find_package( LCG ${LCG_VERSION_NUMBER} EXACT REQUIRED )

# External(s) needed at runtime:
find_package( Xrootd )
find_package( GSL )
find_package( PNG )

# Make CMake find the project specific code we have here:
list( INSERT CMAKE_MODULE_PATH 0 ${CMAKE_SOURCE_DIR}/modules )

# Set up CTest:
atlas_ctest_setup()

# Set up a work directory project:
atlas_project( AthDataQuality ${ATHDATAQUALITY_PROJECT_VERSION}
   PROJECT_ROOT ${CMAKE_SOURCE_DIR}/../../ )

# Generate the environment setup for the externals, to be used during the build:
lcg_generate_env( SH_FILE ${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM}/env_setup.sh )

# Generate replacement rules for the installed paths:
set( _replacements )
if( NOT "$ENV{NICOS_PROJECT_HOME}" STREQUAL "" )
   get_filename_component( _buildDir $ENV{NICOS_PROJECT_HOME} PATH )
   list( APPEND _replacements ${_buildDir}
      "\${AthDataQuality_DIR}/../../../.." )
endif()
if( NOT "$ENV{NICOS_PROJECT_RELNAME}" STREQUAL "" )
   list( APPEND _replacements $ENV{NICOS_PROJECT_RELNAME}
      "\${AthDataQuality_VERSION}" )
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
configure_file( ${CMAKE_SOURCE_DIR}/PostConfig.cmake.in
   ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/PostConfig.cmake @ONLY )
install( FILES ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/PostConfig.cmake
   DESTINATION ${CMAKE_INSTALL_CMAKEDIR} )

# Set up CPack:
atlas_cpack_setup()
