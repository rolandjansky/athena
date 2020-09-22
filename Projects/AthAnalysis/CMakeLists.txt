# The minimum required CMake version:
cmake_minimum_required( VERSION 3.2 FATAL_ERROR )
project( AthAnalysis )

# Read in the project's version from a file called version.txt. But let it be
# overridden from the command line if necessary.
file( READ ${CMAKE_SOURCE_DIR}/version.txt _version )
string( STRIP ${_version} _version )
set( ATHANALYSIS_PROJECT_VERSION ${_version}
   CACHE STRING "Version of the AthAnalysis project to build" )
unset( _version )

# Find the ATLAS CMake code:
find_package( AtlasCMake QUIET )

# Find the base project(s):
find_package( AthAnalysisExternals REQUIRED )
find_package( Gaudi REQUIRED )

# Additional externals needed for the build/runtime:
find_package( Xrootd )
find_package( GSL )
find_package( Davix )
find_package( PNG )
find_package( BLAS )
find_package( VDT )
find_package( requests )

# AIDA dependency, because Gaudi needs it
# Need to ensure AIDA is in the ROOT_INCLUDE_PATH for python dictionaries
find_package( AIDA )

# Package(s) needed only by the analysers, not the analysis release itself.
find_package( nlohmann_json )

# Conditional compile flags:
add_definitions( -DXAOD_ANALYSIS )
add_definitions( -DROOTCORE_RELEASE_SERIES=25 )

# And the same variables for CMake as well:
set( XAOD_ANALYSIS TRUE CACHE BOOL
   "Flag specifying that this is an analysis release" )

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

# Set up CTest:
atlas_ctest_setup()

# Declare project name and version
atlas_project( AthAnalysis ${ATHANALYSIS_PROJECT_VERSION}
   USE AthAnalysisExternals 0.0.1
   PROJECT_ROOT ${CMAKE_SOURCE_DIR}/../../
   FORTRAN )

# Generate the environment setup for the externals, to be used during the build:
lcg_generate_env( SH_FILE ${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM}/env_setup.sh )

# Generate replacement rules for the installed paths:
set( _replacements )
if( NOT "$ENV{NICOS_PROJECT_HOME}" STREQUAL "" )
   get_filename_component( _buildDir $ENV{NICOS_PROJECT_HOME} PATH )
   list( APPEND _replacements ${_buildDir} "\${AthAnalysis_DIR}/../../../.." )
endif()
if( NOT "$ENV{NICOS_PROJECT_RELNAME}" STREQUAL "" )
   list( APPEND _replacements $ENV{NICOS_PROJECT_RELNAME}
      "\${AthAnalysis_VERSION}" )
endif()


# Now generate and install the installed setup files:
lcg_generate_env( SH_FILE ${CMAKE_BINARY_DIR}/env_setup_install.sh
   REPLACE ${_replacements} )
install( FILES ${CMAKE_BINARY_DIR}/env_setup_install.sh
   DESTINATION . RENAME env_setup.sh )

# Configure and install the post-configuration file:
set( GAUDI_VERSION $ENV{GAUDI_VERSION} )
string( REPLACE "$ENV{TDAQ_RELEASE_BASE}" "\$ENV{TDAQ_RELEASE_BASE}"
   TDAQ-COMMON_ROOT "${TDAQ-COMMON_ROOT}" )
string( REPLACE "${TDAQ-COMMON_VERSION}" "\${TDAQ-COMMON_VERSION}"
   TDAQ-COMMON_ROOT "${TDAQ-COMMON_ROOT}" )
configure_file( ${CMAKE_SOURCE_DIR}/PostConfig.cmake.in
   ${CMAKE_BINARY_DIR}/PostConfig.cmake @ONLY )
install( FILES ${CMAKE_BINARY_DIR}/PostConfig.cmake
   DESTINATION ${CMAKE_INSTALL_CMAKEDIR} )

# Package up the release using CPack:
atlas_cpack_setup()
