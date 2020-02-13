#
# This is the main CMakeLists.txt file for building the VP1Light
# software release.
#

# The minimum required CMake version:
cmake_minimum_required( VERSION 3.2 FATAL_ERROR )

# Read in the project's version from a file called version.txt. But let it be
# overridden from the command line if necessary.
file( READ ${CMAKE_SOURCE_DIR}/version.txt _version )
string( STRIP ${_version} _version )
set( VP1LIGHT_PROJECT_VERSION ${_version}
   CACHE STRING "Version of the VP1Light project to build" )
unset( _version )

# This project is built on top of VP1LightExternals:
find_package( VP1LightExternals REQUIRED )

# Find Python. This is needed because VP1LightExternals sets up
# a wrong value for PYTHONHOME. And nothing in VP1Light builds
# against Python to correct it.
find_package( Python COMPONENTS Interpreter )

find_package( ROOT COMPONENTS Physics Core Tree MathCore Hist RIO pthread
                              GenVector REQUIRED )


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

# Include the VP1Light specific function(s), to include RootCore:
include( VP1LightFunctions ) # TODO: Do we need RootCore for VP1Light/xAOD???

# Set up the build/runtime environment:
set( VP1LightReleaseEnvironment_DIR ${CMAKE_SOURCE_DIR} )
find_package( VP1LightReleaseEnvironment REQUIRED )

# Add the directory to the global include path, where the project
# will create the RootCore/Packages.h header:
include_directories( ${CMAKE_BINARY_DIR}/RootCore/include )

# Set up CTest:
atlas_ctest_setup()

# Custom definitions needed for this package
# set( CMAKE_CXX_STANDARD 14 ) # this must be declared BEFORE the 'atlas_project' command


message("Using VP1LightExternals version: $ENV{VP1LightExternals_VERSION}" )

# Declare project name and version
atlas_project( VP1Light ${VP1LIGHT_PROJECT_VERSION}
   #USE VP1LightExternals $ENV{VP1LightExternals_VERSION} # This fails on macOS
   PROJECT_ROOT ${CMAKE_SOURCE_DIR}/../../ )

# Generate the RootCore/Packages.h header:
vp1light_generate_release_header()

# Set up the load_packages.C script:
configure_file( ${CMAKE_SOURCE_DIR}/scripts/load_packages.C.in
   ${CMAKE_SCRIPT_OUTPUT_DIRECTORY}/load_packages.C @ONLY )
install( FILES ${CMAKE_SCRIPT_OUTPUT_DIRECTORY}/load_packages.C
   DESTINATION ${CMAKE_INSTALL_SCRIPTDIR} )

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
if( NOT "$ENV{NICOS_PROJECT_RELNAME}" STREQUAL "" )
   list( APPEND _replacements $ENV{NICOS_PROJECT_RELNAME}
      "\${VP1Light_VERSION}" )
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
