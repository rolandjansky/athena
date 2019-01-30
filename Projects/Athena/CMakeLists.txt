
# The minimum required CMake version:
cmake_minimum_required( VERSION 3.6 )

# Read in the project's version from a file called version.txt. But let it be
# overridden from the command line if necessary.
file( READ ${CMAKE_SOURCE_DIR}/version.txt _version )
string( STRIP ${_version} _version )
set( ATHENA_PROJECT_VERSION ${_version}
   CACHE STRING "Version of the Athena project to build" )
unset( _version )

# Set the versions of the TDAQ externals to pick up for the build:
if( NOT LCG_NIGHTLY )
   if( NOT "$ENV{LCG_NIGHTLY}" STREQUAL "" )
       set( LCG_NIGHTLY $ENV{LCG_NIGHTLY} CACHE STRING "LCG nightly build flavor" )
       message( STATUS "Using LCG_NIGHTLY: ${LCG_NIGHTLY}" )
   endif()
endif()

# Set the versions of the TDAQ externals to pick up for the build:
if( LCG_NIGHTLY )
    # TDAQ_RELEASE_BASE should be set to a NIGHTLY TDAQ build!
    set( TDAQ-COMMON_VERSION "99-00-00" CACHE STRING
       "The version of tdaq-common to use for the build" )
    set( DQM-COMMON_VERSION "99-00-00" CACHE STRING
       "The version of dqm-common to use for the build" )
    set( TDAQ_VERSION "99-00-00" CACHE STRING
       "The version of tdaq to use for the build" )
else()
    set( TDAQ-COMMON_VERSION "02-09-00" CACHE STRING
       "The version of tdaq-common to use for the build" )
    set( DQM-COMMON_VERSION "01-09-00" CACHE STRING
       "The version of dqm-common to use for the build" )
    set( TDAQ_VERSION "08-01-01" CACHE STRING
       "The version of tdaq to use for the build" )
endif()

set( TDAQ-COMMON_ATROOT
   "$ENV{TDAQ_RELEASE_BASE}/tdaq-common/tdaq-common-${TDAQ-COMMON_VERSION}"
   CACHE PATH "The directory to pick up tdaq-common from" )
set( DQM-COMMON_ATROOT
   "$ENV{TDAQ_RELEASE_BASE}/dqm-common/dqm-common-${DQM-COMMON_VERSION}"
   CACHE PATH "The directory to pick up dqm-common from" )
set( TDAQ_PROJECT_NAME "tdaq" CACHE STRING "The name of the tdaq project" )
set( TDAQ_ATROOT
   "$ENV{TDAQ_RELEASE_BASE}/${TDAQ_PROJECT_NAME}/${TDAQ_PROJECT_NAME}-${TDAQ_VERSION}"
   CACHE PATH "The directory to pick up tdaq from" )
mark_as_advanced( TDAQ-COMMON_ATROOT DQM-COMMON_ATROOT TDAQ_PROJECT_NAME
   TDAQ_ATROOT )

# Find the ATLAS CMake code:
find_package( AtlasCMake QUIET )

# Find the base project(s):
find_package( AthenaExternals REQUIRED )
find_package( Gaudi REQUIRED )

# Find some auxiliary packages:
find_package( Doxygen )
find_package( PNG )

# Load all the files from the externals/ subdirectory:
file( GLOB _externals "${CMAKE_CURRENT_SOURCE_DIR}/externals/*.cmake" )
foreach( _external ${_externals} )
   include( ${_external} )
   get_filename_component( _extName ${_external} NAME_WE )
   string( TOUPPER ${_extName} _extNameUpper )
   message( STATUS "Taking ${_extName} from: ${${_extNameUpper}_LCGROOT}" )
   unset( _extName )
   unset( _extNameUpper )
endforeach()
unset( _external )
unset( _externals )

# Set up CTest:
atlas_ctest_setup()

# Declare project name and version
atlas_project( Athena ${ATHENA_PROJECT_VERSION}
   USE AthenaExternals ${AthenaExternals_VERSION}
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
string( REPLACE "$ENV{TDAQ_RELEASE_BASE}" "\$ENV{TDAQ_RELEASE_BASE}"
   DQM-COMMON_ATROOT "${DQM-COMMON_ATROOT}" )
string( REPLACE "${DQM-COMMON_VERSION}" "\${DQM-COMMON_VERSION}"
   DQM-COMMON_ATROOT "${DQM-COMMON_ATROOT}" )

# Temporarily add tdaq dependency to Athena build:
string( REPLACE "$ENV{TDAQ_RELEASE_BASE}" "\$ENV{TDAQ_RELEASE_BASE}"
   TDAQ_ATROOT "${TDAQ_ATROOT}" )
string( REPLACE "${TDAQ_VERSION}" "\${TDAQ_VERSION}"
   TDAQ_ATROOT "${TDAQ_ATROOT}" )

configure_file( ${CMAKE_SOURCE_DIR}/PostConfig.cmake.in
   ${CMAKE_BINARY_DIR}/PostConfig.cmake @ONLY )
install( FILES ${CMAKE_BINARY_DIR}/PostConfig.cmake
   DESTINATION ${CMAKE_INSTALL_CMAKEDIR} )

# Package up the release using CPack:
atlas_cpack_setup()
