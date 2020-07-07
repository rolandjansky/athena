# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
# This is the main CMakeLists.txt file for building the VP1Light
# software release.
#

# Set up the project.
cmake_minimum_required( VERSION 3.6 )
file( READ ${CMAKE_SOURCE_DIR}/version.txt _version )
string( STRIP ${_version} _version )
project( VP1Light VERSION ${_version} LANGUAGES C CXX )
unset( _version )

# This project is built on top of VP1LightExternals:
find_package( VP1LightExternals REQUIRED )

# Find Python. This is needed because VP1LightExternals sets up
# a wrong value for PYTHONHOME. And nothing in VP1Light builds
# against Python to correct it.
find_package( Python COMPONENTS Interpreter )

# Build options to customize the compilation of ATLAS packages
option( BUILDVP1LIGHT "Build ATLAS packages for VP1 Light" ON )

# Add the project's modules directory to CMAKE_MODULE_PATH:
list( INSERT CMAKE_MODULE_PATH 0 ${CMAKE_SOURCE_DIR}/modules )
list( REMOVE_DUPLICATES CMAKE_MODULE_PATH )

# Install the files from modules/:
install( DIRECTORY ${CMAKE_SOURCE_DIR}/modules/
   DESTINATION cmake/modules
   USE_SOURCE_PERMISSIONS
   PATTERN ".svn" EXCLUDE
   PATTERN "*~" EXCLUDE )

# Set up the build/runtime environment:
set( VP1LightReleaseEnvironment_DIR ${CMAKE_SOURCE_DIR} )
find_package( VP1LightReleaseEnvironment REQUIRED )

# Set up where to find the xAODUtilities CMake code.
set( xAODUtilities_DIR
   "${CMAKE_SOURCE_DIR}/../../Event/xAOD/xAODCore/cmake"
   CACHE PATH "Directory holding the xAODUtilities module" )

# Set up CTest:
atlas_ctest_setup()

# Declare project name and version
atlas_project( USE VP1LightExternals ${VP1LightExternals_VERSION}
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
   list( APPEND _replacements ${_buildDir} "\${VP1Light_DIR}/../../../.." )
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
