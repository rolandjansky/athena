# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

#
# @file AthenaPoolUtilities/share/AthenaPoolUtilitiesTestConfig.make
# @author sss
# @date May, 2016
# @brief cmake function for running tpcnv regression tests.
#
# Example:
#
#   run_tpcnv_legacy_test( TrackParticleTPCnv_17.2.8   AOD-17.2.8-full )
#

function( run_tpcnv_legacy_test testName refName )

   cmake_parse_arguments( ARG "" "REFERENCE_TAG" "REQUIRED_LIBRARIES" ${ARGN} )

   # The package name is needed for the configuration of the test script:
   atlas_get_package_name( pkgName )

   # Find the test runner script:
   find_file( ATLAS_TPCNV_LEGACY_TEST_RUNNER "run_tpcnv_legacy_test.sh.in"
      PATH_SUFFIXES "share" "scripts"
      PATHS "${AthenaPoolUtilitiesTest_DIR}/.."
      "${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM}/cmake/modules"
      ${CMAKE_MODULE_PATH} )
   if( NOT ATLAS_TPCNV_LEGACY_TEST_RUNNER )
      message( WARNING "Couldn't find run_tpcnv_legacy_test.sh.in" )
      return()
   endif()
   mark_as_advanced( ATLAS_TPCNV_LEGACY_TEST_RUNNER )

   # Create the script that will be run:
   configure_file( "${ATLAS_TPCNV_LEGACY_TEST_RUNNER}"
      "${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/tpcnv_legacy_${testName}.sh"
      @ONLY )

    set( _env )
    if( ARG_REFERENCE_TAG )
      set( _env ENVIRONMENT "ATLAS_REFERENCE_TAG=${pkgName}/${ARG_REFERENCE_TAG}" )
    endif()

   # Create a test with it:
   atlas_add_test( ${testName}
      SCRIPT
      "${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/tpcnv_legacy_${testName}.sh"
      PROPERTIES TIMEOUT 600
      POST_EXEC_SCRIPT "post_tpcnvtest.sh ${testName}" ${_env} )

endfunction( run_tpcnv_legacy_test )

function( run_tpcnv_test testName refName )

   cmake_parse_arguments( ARG "" "REFERENCE_TAG" "REQUIRED_LIBRARIES" ${ARGN} )

   # The package name is needed for the configuration of the test script:
   atlas_get_package_name( pkgName )

   # Find the test runner script:
   find_file( ATLAS_TPCNV_TEST_RUNNER "run_tpcnv_test.sh.in"
      PATH_SUFFIXES "share" "scripts"
      PATHS "${AthenaPoolUtilitiesTest_DIR}/.."
      "${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM}/cmake/modules"
      ${CMAKE_MODULE_PATH} )
   if( NOT ATLAS_TPCNV_TEST_RUNNER )
      message( WARNING "Couldn't find run_tpcnv_test.sh.in" )
      return()
   endif()
   mark_as_advanced( ATLAS_TPCNV_TEST_RUNNER )

   # Create the script that will be run:
   configure_file( "${ATLAS_TPCNV_TEST_RUNNER}"
      "${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/tpcnv_${testName}.sh"
      @ONLY )

    set( _env )
    if( ARG_REFERENCE_TAG )
      set( _env ENVIRONMENT "ATLAS_REFERENCE_TAG=${pkgName}/${ARG_REFERENCE_TAG}" )
    endif()

   # Create a test with it:
   atlas_add_test( ${testName}
      SCRIPT
      "${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/tpcnv_${testName}.sh"
      PROPERTIES TIMEOUT 600
      POST_EXEC_SCRIPT "post_tpcnvtest.sh ${testName}" ${_env} )

endfunction( run_tpcnv_test )

# Allow the user to look for the conventional variable for checking whether
# the package was found:
set( ATHENAPOOLUTILITIESTEST_FOUND TRUE )
