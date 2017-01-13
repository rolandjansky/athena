
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
set( PROJECT_NAME "AnalysisBase" )
set( PROJECT_DEPS "AtlasExternals" "0.0.1" )
set( PROJECT_VERSION "2.6.0" )

# Find the ATLAS CMake code:
find_package( AtlasCMake QUIET )

# Find the base project(s):
find_package( AtlasExternals REQUIRED )

# Set it up as a "standalone build":
set( XAOD_STANDALONE TRUE CACHE BOOL
   "Flag specifying that this is a standalone build" )
if( XAOD_STANDALONE )
   add_definitions( -DXAOD_STANDALONE )
endif()
set( XAOD_ANALYSIS TRUE CACHE BOOL
   "Flag specifying that this is an analysis build" )
if( XAOD_ANALYSIS )
   add_definitions( -DXAOD_ANALYSIS )
endif()

# Set up CTest:
atlas_ctest_setup()

# Declare project name and version
atlas_project( ${PROJECT_NAME} ${PROJECT_VERSION}
   USE ${PROJECT_DEPS}
   PROJECT_ROOT ${CMAKE_SOURCE_DIR}/../../ )

# Generate the environment setup for the externals, to be used during the build:
lcg_generate_env( SH_FILE ${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM}/env_setup.sh )

# Generate replacement rules for the installed paths:
set( _replacements )
if( NOT "$ENV{NICOS_PROJECT_HOME}" STREQUAL "" )
   get_filename_component( _buildDir $ENV{NICOS_PROJECT_HOME} PATH )
   list( APPEND _replacements ${_buildDir} "\${AnalysisBase_DIR}/../../../.." )
endif()
if( NOT "$ENV{NICOS_PROJECT_RELNAME}" STREQUAL "" )
   list( APPEND _replacements $ENV{NICOS_PROJECT_RELNAME}
      "\${AnalysisBase_VERSION}" )
endif()

# Now generate and install the installed setup files:
lcg_generate_env( SH_FILE ${CMAKE_BINARY_DIR}/env_setup_install.sh
   REPLACE ${_replacements} )
install( FILES ${CMAKE_BINARY_DIR}/env_setup_install.sh
   DESTINATION . RENAME env_setup.sh )

# Package up the release using CPack:
atlas_cpack_setup()
