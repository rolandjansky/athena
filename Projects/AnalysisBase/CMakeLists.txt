#
# This is the main CMakeLists.txt file for building the AnalysisBase
# software release.
#

# The minimum required CMake version:
cmake_minimum_required( VERSION 3.6 FATAL_ERROR )

# Read in the project's version from a file called version.txt. But let it be
# overridden from the command line if necessary.
file( READ ${CMAKE_SOURCE_DIR}/version.txt _version )
string( STRIP ${_version} _version )
set( ANALYSISBASE_PROJECT_VERSION ${_version}
   CACHE STRING "Version of the AnalysisBase project to build" )
unset( _version )

# This project is built on top of AnalysisBaseExternals:
find_package( AnalysisBaseExternals REQUIRED )

# Find Python. This is needed because AnalysisBaseExternals sets up
# a wrong value for PYTHONHOME. And nothing in AnalysisBase builds
# against Python to correct it.
find_package( PythonInterp )

# Set up the build/runtime environment:
set( AnalysisBaseReleaseEnvironment_DIR ${CMAKE_SOURCE_DIR} )
find_package( AnalysisBaseReleaseEnvironment REQUIRED )

# Add the directory to the global include path, where the project
# will create the RootCore/Packages.h header:
include_directories( ${CMAKE_BINARY_DIR}/RootCore/include )

# Set up CTest:
atlas_ctest_setup()

# Declare project name and version
atlas_project( AnalysisBase ${ANALYSISBASE_PROJECT_VERSION}
   USE AnalysisBaseExternals ${AnalysisBaseExternals_VERSION}
   PROJECT_ROOT ${CMAKE_SOURCE_DIR}/../../ )

# Configure and install the post-configuration file:
configure_file( ${CMAKE_SOURCE_DIR}/PostConfig.cmake.in
   ${CMAKE_BINARY_DIR}/PostConfig.cmake @ONLY )
install( FILES ${CMAKE_BINARY_DIR}/PostConfig.cmake
   DESTINATION ${CMAKE_INSTALL_CMAKEDIR} )

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

# Generate the environment configuration file(s):
lcg_generate_env(
   SH_FILE ${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM}/env_setup.sh )
lcg_generate_env(
   SH_FILE ${CMAKE_BINARY_DIR}/env_setup_install.sh
   REPLACE ${_replacements} )
install( FILES ${CMAKE_BINARY_DIR}/env_setup_install.sh
   DESTINATION . RENAME env_setup.sh )

# Set up the release packaging:
atlas_cpack_setup()
