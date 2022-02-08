# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#
# Project file for building a selected set of packages against an
# installed ATLAS release/nightly.
#

# Set up the project.
cmake_minimum_required( VERSION 3.10 )
# Note that there is a second `project` call further below to set
# the version and languages.
project( WorkDir )

# Force the compilation of CUDA code for only compute capability 5.2 for now.
# To get rid of warnings from 11.0 about not supporting older compute
# capabilities anymore.
set( CMAKE_CUDA_ARCHITECTURES "52" CACHE STRING
   "CUDA architectures to build code for" )

# Enable the CI tests in the CITest package in case the user wants to
# run them locally:
set( ATLAS_ENABLE_CI_TESTS TRUE CACHE BOOL
   "Set up tests in CITest package" )

# Let the user pick up updated AtlasCMake/AtlasLCG versions for testing.
# Remember that it's not a problem if AtlasCMake is not found, that's why
# we use the QUIET keyword.
find_package( AtlasCMake QUIET )

# Try to figure out what project is our parent. Just using a hard-coded list
# of possible project names. Basically the names of all the other
# sub-directories inside the Projects/ directory in the repository.
set( _parentProjectNames Athena AthenaP1 AnalysisBase AthAnalysis
  AthSimulation AthDerivation AthDataQuality AthGeneration DetCommon )
set( _defaultParentProject Athena )
foreach( _pp ${_parentProjectNames} )
   if( NOT "$ENV{${_pp}_DIR}" STREQUAL "" )
      set( _defaultParentProject ${_pp} )
      break()
   endif()
endforeach()

# Set the parent project name based on the previous findings:
set( ATLAS_PROJECT ${_defaultParentProject}
   CACHE STRING "The name of the parent project to build against" )

# Clean up:
unset( _parentProjectNames )
unset( _defaultParentProject )

# Find the project that we depend on:
find_package( ${ATLAS_PROJECT} REQUIRED )

# Set up the project with its version and baseline languages.
project( WorkDir VERSION ${${ATLAS_PROJECT}_VERSION} LANGUAGES C CXX )

# Set up any additional languages that may be available.
include( CheckLanguage )
check_language( Fortran )
if( CMAKE_Fortran_COMPILER )
   enable_language( Fortran )
endif()
check_language( CUDA )
if( CMAKE_CUDA_COMPILER )
   enable_language( CUDA )
endif()
find_package( AtlasHIP QUIET )
if( AtlasHIP_FOUND )
   include( AtlasCheckLanguage )
   atlas_check_language( HIP )
   if( CMAKE_HIP_COMPILER )
      enable_language( HIP )
   endif()
endif()

# Set up where to find the AthenaPoolUtilitiesTest CMake code.
if( IS_DIRECTORY
   "${CMAKE_SOURCE_DIR}/../../Database/AthenaPOOL/AthenaPoolUtilities" )
   set( AthenaPoolUtilitiesTest_DIR
      "${CMAKE_SOURCE_DIR}/../../Database/AthenaPOOL/AthenaPoolUtilities/cmake"
      CACHE PATH "Directory holding the AthenaPoolUtilititesTest module" )
endif()

# Set up where to find the xAODUtilities CMake code.
if( IS_DIRECTORY
   "${CMAKE_SOURCE_DIR}/../../Event/xAOD/xAODCore/cmake" )
   set( xAODUtilities_DIR
      "${CMAKE_SOURCE_DIR}/../../Event/xAOD/xAODCore/cmake"
      CACHE PATH "Directory holding the xAODUtilities module" )
endif()

# Set up CTest:
atlas_ctest_setup()

# Generate a compile_commands.json file, which VS Code can use to interpret
# our code correctly.
set( CMAKE_EXPORT_COMPILE_COMMANDS TRUE CACHE BOOL
   "Create compile_commands.json" FORCE )

# Re-check all of the wildcarded expressions on every re-build in this
# project.
set( ATLAS_ALWAYS_CHECK_WILDCARDS TRUE CACHE BOOL
   "(Re-)Check wildcards in file specifications on every build" )

# Set up CppCheck.
find_program( ATLAS_CPPCHECK "cppcheck"
   PATHS ENV PATH
   NO_DEFAULT_PATH )
mark_as_advanced( ATLAS_CPPCHECK )
if( ATLAS_CPPCHECK )
   # Set up the default cppcheck command to use.
   set( CMAKE_CPPCHECK_DEFAULT "${ATLAS_CPPCHECK}" "--quiet"
      "--enable=warning,portability" "--inline-suppr" "--template=gcc" )
   foreach( ignored_path "${LCG_RELEASE_BASE}" "${TDAQ-COMMON_INCLUDE_DIR}"
                         "${TDAQ_INCLUDE_DIR}" "${AthenaExternals_INCLUDE_DIR}"
                         "${AnalysisBaseExternals_INCLUDE_DIR}"
                         "${AthAnalysisExternals_INCLUDE_DIR}"
                         "${AthDerivationExternals_INCLUDE_DIR}"
                         "${AthSimulationExternals_INCLUDE_DIR}"
                         "${AthGenerationExternals_INCLUDE_DIR}" )
      if( ignored_path )
         list( APPEND CMAKE_CPPCHECK_DEFAULT "--suppress=*:${ignored_path}/*" )
      endif()
   endforeach()
   list( APPEND CMAKE_CPPCHECK_DEFAULT "--suppress=*:*Dict.cxx" )
   # Set the cache variable(s) that CMake uses to decide how to use cppcheck.
   set( CMAKE_C_CPPCHECK "${CMAKE_CPPCHECK_DEFAULT}"
      CACHE STRING "CppCheck command to use for the C source files" )
   set( CMAKE_CXX_CPPCHECK "${CMAKE_CPPCHECK_DEFAULT}"
      CACHE STRING "CppCheck command to use for the C++ source files" )
   unset( CMAKE_CPPCHECK_DEFAULT )
   # Let the user know what happened.
   if( CMAKE_C_CPPCHECK )
      message( STATUS "Checking C files with: ${CMAKE_C_CPPCHECK}" )
   endif()
   if( CMAKE_CXX_CPPCHECK )
      message( STATUS "Checking C++ files with: ${CMAKE_CXX_CPPCHECK}" )
   endif()
endif()

# Set up a work directory project with the same version as our parent:
atlas_project( USE ${ATLAS_PROJECT} ${${ATLAS_PROJECT}_VERSION}
   PROJECT_ROOT ${CMAKE_SOURCE_DIR}/../../ )

# Set up the runtime environment setup script(s):
lcg_generate_env( SH_FILE ${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM}/env_setup.sh )
install( FILES ${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM}/env_setup.sh
   DESTINATION . )

# Dump some environment variables for IDE use:
set( CMAKE_DUMP_ENV_FILE ${CMAKE_BINARY_DIR}/env.txt CACHE FILEPATH
   "File containing dump of environment variables" )

if( CMAKE_DUMP_ENV_FILE )
   execute_process(
      COMMAND ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/atlas_build_run.sh printenv
      COMMAND grep -w ^PYTHONPATH
      OUTPUT_FILE "${CMAKE_DUMP_ENV_FILE}" )
endif()

# Set up CPack:
atlas_cpack_setup()

# Remind to set up the environment
message( STATUS "")
message( STATUS "            In order to test your updates, please don't forget to" )
message( STATUS "            set up the environment with e.g.:" )
message( STATUS "            --->   source ${ATLAS_PLATFORM}/setup.sh" )
message( STATUS "")
