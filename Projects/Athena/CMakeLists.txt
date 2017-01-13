
# The minimum required CMake version:
cmake_minimum_required( VERSION 3.2 FATAL_ERROR )

# If there's a directory called AtlasCMake in the project,
# and the user didn't specify AtlasCMake_DIR yet, then let's
# give it a default value.
if( IS_DIRECTORY ${CMAKE_SOURCE_DIR}/../../Build/AtlasCMake AND
      NOT AtlasCMake_DIR AND NOT ENV{AtlasCMake_DIR} )
   set( AtlasCMake_DIR ${CMAKE_SOURCE_DIR}/../../Build/AtlasCMake )
endif()

# If there's a directory called AtlasLCG in the project,
# and the user didn't specify LCG_DIR yet, then let's
# give it a default value.
if( IS_DIRECTORY ${CMAKE_SOURCE_DIR}/../../Build/AtlasLCG AND
      NOT LCG_DIR AND NOT ENV{LCG_DIR} )
   set( LCG_DIR ${CMAKE_SOURCE_DIR}/../../Build/AtlasLCG )
endif()

# Set up the project:
message( STATUS "Using default project configuration" )
set( PROJECT_NAME "Athena" )
set( PROJECT_DEPS "AtlasExternals" "0.0.1" )
set( PROJECT_VERSION "21.0.14" )

set( TDAQ-COMMON_VERSION "02-01-00" )
set( TDAQ-COMMON_ROOT
   "$ENV{TDAQ_RELEASE_BASE}/tdaq-common/tdaq-common-${TDAQ-COMMON_VERSION}" )

set( DQM-COMMON_VERSION "01-01-00" )
set( DQM-COMMON_ROOT
   "$ENV{TDAQ_RELEASE_BASE}/dqm-common/dqm-common-${DQM-COMMON_VERSION}" )

set( TDAQ_PROJECT_NAME "tdaq-nightly")
set( TDAQ_VERSION "07-10-16" )
set( TDAQ_ROOT "$ENV{TDAQ_RELEASE_BASE}/${TDAQ_PROJECT_NAME}/${TDAQ_PROJECT_NAME}-${TDAQ_VERSION}" )


# Find the ATLAS CMake code:
find_package( AtlasCMake QUIET )

# Find the base project(s):
find_package( AtlasExternals REQUIRED )
find_package( Gaudi REQUIRED )

# Set up CTest:
atlas_ctest_setup()

# Declare project name and version
atlas_project( ${PROJECT_NAME} ${PROJECT_VERSION}
   USE ${PROJECT_DEPS}
   PROJECT_ROOT ${CMAKE_SOURCE_DIR}/../../
   FORTRAN )

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

# Configure and install the post-configuration file:
string( REPLACE "$ENV{TDAQ_RELEASE_BASE}" "\$ENV{TDAQ_RELEASE_BASE}"
   TDAQ-COMMON_ROOT "${TDAQ-COMMON_ROOT}" )
string( REPLACE "${TDAQ-COMMON_VERSION}" "\${TDAQ-COMMON_VERSION}"
   TDAQ-COMMON_ROOT "${TDAQ-COMMON_ROOT}" )
string( REPLACE "$ENV{TDAQ_RELEASE_BASE}" "\$ENV{TDAQ_RELEASE_BASE}"
   DQM-COMMON_ROOT "${DQM-COMMON_ROOT}" )
string( REPLACE "${DQM-COMMON_VERSION}" "\${DQM-COMMON_VERSION}"
   DQM-COMMON_ROOT "${DQM-COMMON_ROOT}" )
configure_file( ${CMAKE_SOURCE_DIR}/PostConfig.cmake.in
   ${CMAKE_BINARY_DIR}/PostConfig.cmake @ONLY )
install( FILES ${CMAKE_BINARY_DIR}/PostConfig.cmake
   DESTINATION ${CMAKE_INSTALL_CMAKEDIR} )

# Turn off the compression of the created RPM. This makes the RPM a lot
# bigger, but should push the RPM creation time down by a lot.
set( CPACK_RPM_SPEC_MORE_DEFINE
   "%define _source_payload w0.gzdio\n%define _binary_payload w0.gzdio" )

# Package up the release using CPack:
atlas_cpack_setup()
