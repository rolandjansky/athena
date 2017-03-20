#
# This project can be used to build just a selected few packages on top
# of a full AthenaP1 release. Use it like the following:
#   - Set up the AthenaP1 release in your runtime environment.
#   - Make a copy of this directory's package_filters.txt file into a directory
#     outside of the repository. Edit it to select just the packages that you
#     want to build.
#   - Configure your build like:
#
#       cmake -DATLAS_PACKAGE_FILTER_FILE=/your/package_filters.txt \
#          ../athena/Projects/AthenaP1WorkDir
#

# Set the minimum required CMake version:
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

# Pick up a local version of the AtlasCMake code if it exists:
find_package( AtlasCMake QUIET )

# Find the project that we depend on:
find_package( AthenaP1 )

# Set up CTest:
atlas_ctest_setup()

# Set up a work directory project:
atlas_project( AthenaP1WorkDir 21.0.9
   USE AthenaP1 0.0.1
   PROJECT_ROOT ${CMAKE_SOURCE_DIR}/../..
   FORTRAN )

# Set up the runtime environment setup script(s):
lcg_generate_env( SH_FILE ${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM}/env_setup.sh )
install( FILES ${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM}/env_setup.sh
   DESTINATION . )

# Turn off the RPM compression. This is just for a test in this project. It's
# main purpose is in the AthenaP1 project.
set( CPACK_RPM_SPEC_MORE_DEFINE
   "%define _source_payload w0.gzdio\n%define _binary_payload w0.gzdio" )

# Set up CPack:
atlas_cpack_setup()
