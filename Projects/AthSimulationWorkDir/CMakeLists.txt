#
# Project file for building a selected set of packages against an
# installed AthSimulation release/nightly.
#

# Set the minimum required CMake version:
cmake_minimum_required( VERSION 3.2 FATAL_ERROR )

# Let the user pick up updated AtlasCMake/AtlasLCG versions for testing:
find_package( AtlasCMake QUIET )

# Find the project that we depend on:
find_package( AthSimulation REQUIRED )

# Set up CTest:
atlas_ctest_setup()

# Set up a work directory project:
atlas_project( AthSimulationWorkDir ${AthSimulation_VERSION}
   USE AthSimulation ${AthSimulation_VERSION}
   PROJECT_ROOT ${CMAKE_SOURCE_DIR}/../..
   FORTRAN )

# Set up the runtime environment setup script(s):
lcg_generate_env( SH_FILE ${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM}/env_setup.sh )
install( FILES ${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM}/env_setup.sh
   DESTINATION . )

# Set up CPack:
atlas_cpack_setup()
