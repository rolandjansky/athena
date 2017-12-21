#
# Project file for building a selected set of packages against an
# installed ATLAS release/nightly.
#

# Set the minimum required CMake version:
cmake_minimum_required( VERSION 3.2 FATAL_ERROR )

# Let the user pick up updated AtlasCMake/AtlasLCG versions for testing.
# Remember that it's not a problem if AtlasCMake is not found, that's why
# we use the QUIET keyword.
find_package( AtlasCMake QUIET )

# Try to figure out what project is our parent. Just using a hard-coded list
# of possible project names. Basically the names of all the other
# sub-directories inside the Projects/ directory in the repository.
set( _parentProjectNames Athena AthenaP1 AnalysisBase AthAnalysisBase
   AthSimulation AthDerivation)
set( _defaultParentProject Athena )
set( _defaultUseFortran TRUE )
foreach( _pp ${_parentProjectNames} )
   if( NOT "$ENV{${_pp}_DIR}" STREQUAL "" )
      set( _defaultParentProject ${_pp} )
      # Only turn off Fortran support for the AnalysisBase project for now:
      if( "${_pp}" STREQUAL "AnalysisBase" )
         set( _defaultUseFortran FALSE )
      endif()
      break()
   endif()
endforeach()

# Set the parent project name based on the previous findings:
set( ATLAS_PROJECT ${_defaultParentProject}
   CACHE STRING "The name of the parent project to build against" )
# Set whether to use Fortran, based on the previous simple logic:
option( ATLAS_USE_FORTRAN
   "Whether the WorkDir project should provide support for Fortran"
   ${_defaultUseFortran} )

# Clean up:
unset( _parentProjectNames )
unset( _defaultParentProject )
unset( _defaultUseFortran )

# Find the project that we depend on:
find_package( ${ATLAS_PROJECT} REQUIRED )

# Set up CTest:
atlas_ctest_setup()

# Set up a work directory project:
set( _useFortran )
if( ATLAS_USE_FORTRAN )
   set( _useFortran FORTRAN )
endif()
atlas_project( WorkDir ${${ATLAS_PROJECT}_VERSION}
   USE ${ATLAS_PROJECT} ${${ATLAS_PROJECT}_VERSION}
   PROJECT_ROOT ${CMAKE_SOURCE_DIR}/../..
   ${_useFortran} )
unset( _useFortran )

# Set up the runtime environment setup script(s):
lcg_generate_env( SH_FILE ${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM}/env_setup.sh )
install( FILES ${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM}/env_setup.sh
   DESTINATION . )

# Set up CPack:
atlas_cpack_setup()
