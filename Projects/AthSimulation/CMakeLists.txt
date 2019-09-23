
# The minimum required CMake version:
cmake_minimum_required( VERSION 3.6 )

# Read in the project's version from a file called version.txt. But let it be
# overridden from the command line if necessary.
file( READ ${CMAKE_SOURCE_DIR}/version.txt _version )
string( STRIP ${_version} _version )
set( ATHSIMULATION_PROJECT_VERSION ${_version}
   CACHE STRING "Version of the AthSimulation project to build" )
unset( _version )

# Find the ATLAS CMake code:
find_package( AtlasCMake QUIET )

# Find the base project(s):
find_package( AthSimulationExternals REQUIRED )
find_package( Gaudi REQUIRED )

# External(s) needed at build and runtime:
find_package( Frontier_Client )
find_package( PNG )
find_package( VDT )
find_package( TIFF )

# Set the project into "SIMULATIONBASE mode".
set( SIMULATIONBASE TRUE CACHE BOOL
   "Flag specifying that this is a simulation release build" )
if( SIMULATIONBASE )
   add_definitions( -DSIMULATIONBASE )
endif()

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

# Make the local CMake files visible to AtlasCMake.
list( INSERT CMAKE_MODULE_PATH 0 ${CMAKE_SOURCE_DIR}/cmake )

# Set up CTest:
atlas_ctest_setup()

# Declare project name and version
atlas_project( AthSimulation ${ATHSIMULATION_PROJECT_VERSION}
   USE AthSimulationExternals ${AthSimulationExternals_VERSION}
   PROJECT_ROOT ${CMAKE_SOURCE_DIR}/../../
   LANGUAGES C CXX Fortran )

# Install the external configurations:
install( DIRECTORY ${CMAKE_SOURCE_DIR}/externals
   DESTINATION ${CMAKE_INSTALL_CMAKEDIR} USE_SOURCE_PERMISSIONS )

# Generate the environment setup for the externals, to be used during the build:
lcg_generate_env( SH_FILE ${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM}/env_setup.sh )

# Generate replacement rules for the installed paths:
set( _replacements )
if( NOT "$ENV{NICOS_PROJECT_HOME}" STREQUAL "" )
   get_filename_component( _buildDir $ENV{NICOS_PROJECT_HOME} PATH )
   list( APPEND _replacements ${_buildDir} "\${AthSimulation_DIR}/../../../.." )
endif()
if( NOT "$ENV{NICOS_PROJECT_RELNAME}" STREQUAL "" )
   list( APPEND _replacements $ENV{NICOS_PROJECT_RELNAME}
      "\${AthSimulation_VERSION}" )
endif()

# Now generate and install the installed setup files:
lcg_generate_env(
   SH_FILE ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/env_setup_install.sh
   REPLACE ${_replacements} )
install( FILES ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/env_setup_install.sh
   DESTINATION . RENAME env_setup.sh )

# Configure and install the post-configuration file:
configure_file( ${CMAKE_SOURCE_DIR}/cmake/PostConfig.cmake.in
   ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/PostConfig.cmake @ONLY )
install( FILES ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/PostConfig.cmake
   DESTINATION ${CMAKE_INSTALL_CMAKEDIR} )

# Package up the release using CPack:
atlas_cpack_setup()
