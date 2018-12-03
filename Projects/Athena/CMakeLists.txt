
# The minimum required CMake version:
cmake_minimum_required( VERSION 3.2 FATAL_ERROR )

# Read in the project's version from a file called version.txt. But let it be
# overridden from the command line if necessary.
file( READ ${CMAKE_SOURCE_DIR}/version.txt _version )
string( STRIP ${_version} _version )
set( ATHENA_PROJECT_VERSION ${_version}
   CACHE STRING "Version of the Athena project to build" )
unset( _version )

# Set the versions of the TDAQ externals to pick up for the build:
set( TDAQ-COMMON_VERSION "02-03-00" )
set( TDAQ-COMMON_ROOT
   "$ENV{TDAQ_RELEASE_BASE}/tdaq-common/tdaq-common-${TDAQ-COMMON_VERSION}" )

set( DQM-COMMON_VERSION "01-03-00" )
set( DQM-COMMON_ROOT
   "$ENV{TDAQ_RELEASE_BASE}/dqm-common/dqm-common-${DQM-COMMON_VERSION}" )

set( TDAQ_PROJECT_NAME "tdaq")
set( TDAQ_VERSION "07-01-00" )
set( TDAQ_ROOT "$ENV{TDAQ_RELEASE_BASE}/${TDAQ_PROJECT_NAME}/${TDAQ_PROJECT_NAME}-${TDAQ_VERSION}" )

# Find the ATLAS CMake code:
find_package( AtlasCMake QUIET )

# Find the base project(s):
find_package( AthenaExternals REQUIRED )
find_package( Gaudi REQUIRED )

# Load all the files from the externals/ subdirectory:
file( GLOB _externals "${CMAKE_CURRENT_SOURCE_DIR}/externals/*.cmake" )
foreach( _external ${_externals} )
   include( ${_external} )
   get_filename_component( _extName ${_external} NAME_WE )
   string( TOUPPER ${_extName} _extNameUpper )
   message( STATUS "Taking ${_extName} from: ${${_extNameUpper}_ROOT}" )
   unset( _extName )
   unset( _extNameUpper )
endforeach()
unset( _external )
unset( _externals )

# Disable the usage of the --no-undefined linker flag:
atlas_disable_no_undefined()

# Set up CTest:
atlas_ctest_setup()

# Declare project name and version
atlas_project( Athena ${ATHENA_PROJECT_VERSION}
   USE AthenaExternals 0.0.1
   PROJECT_ROOT ${CMAKE_SOURCE_DIR}/../../
   FORTRAN )

# Install the external configurations:
install( DIRECTORY ${CMAKE_SOURCE_DIR}/externals
   DESTINATION ${CMAKE_INSTALL_CMAKEDIR} USE_SOURCE_PERMISSIONS )

# Generate the environment setup for the externals, to be used during the build:
lcg_generate_env( SH_FILE ${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM}/env_setup.sh )

# Generate replacement rules for the installed paths:
set( _replacements )
if( NOT "$ENV{NICOS_PROJECT_HOME}" STREQUAL "" )
   get_filename_component( _buildDir $ENV{NICOS_PROJECT_HOME} PATH )
   list( APPEND _replacements ${_buildDir} "\${Athena_DIR}/../../../.." )
endif()
if( NOT "$ENV{NICOS_PROJECT_RELNAME}" STREQUAL "" )
   list( APPEND _replacements $ENV{NICOS_PROJECT_RELNAME}
      "\${Athena_VERSION}" )
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

# Set tdaq project paths:
string( REPLACE "$ENV{TDAQ_RELEASE_BASE}" "\$ENV{TDAQ_RELEASE_BASE}"
   TDAQ_ROOT "${TDAQ_ROOT}" )
string( REPLACE "${TDAQ_VERSION}" "\${TDAQ_VERSION}"
   TDAQ_ROOT "${TDAQ_ROOT}" )
string( REPLACE "$ENV{TDAQ_RELEASE_BASE}" "\$ENV{TDAQ_RELEASE_BASE}"
   TDAQ-COMMON_ROOT "${TDAQ-COMMON_ROOT}" )
string( REPLACE "${TDAQ-COMMON_VERSION}" "\${TDAQ-COMMON_VERSION}"
   TDAQ-COMMON_ROOT "${TDAQ-COMMON_ROOT}" )
string( REPLACE "$ENV{TDAQ_RELEASE_BASE}" "\$ENV{TDAQ_RELEASE_BASE}"
   DQM-COMMON_ROOT "${DQM-COMMON_ROOT}" )
string( REPLACE "${DQM-COMMON_VERSION}" "\${DQM-COMMON_VERSION}"
   DQM-COMMON_ROOT "${DQM-COMMON_ROOT}" )

# Configure and install the post-configuration file:
configure_file( ${CMAKE_SOURCE_DIR}/PostConfig.cmake.in
   ${CMAKE_BINARY_DIR}/PostConfig.cmake @ONLY )
install( FILES ${CMAKE_BINARY_DIR}/PostConfig.cmake
   DESTINATION ${CMAKE_INSTALL_CMAKEDIR} )

# Package up the release using CPack:
atlas_cpack_setup()
