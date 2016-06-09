# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
# $Id: AthenaPoolUtilitiesTestConfig.cmake 753926 2016-06-09 14:22:05Z ssnyder $
# @file AthenaPoolUtilities/share/AthenaPoolUtilitiesTestConfig.make
# @author sss
# @date May, 2016
# @brief cmake function for running tpcnv regression tests.
#
# Example:
#
#   run_tpcnv_test( TrackParticleTPCnv_17.2.8   AOD-17.2.8-full )
#

function( run_tpcnv_test testName refName )

   # The package name is needed for the configuration of the test script:
   atlas_get_package_name( pkgName )

   # Find the test runner script:
   find_file( _testSkel run_tpcnv_test.sh.in
      PATH_SUFFIXES scripts share
      PATHS ${CMAKE_BINARY_DIR}/${ATLAS_PLATFORM}/cmake/modules ${CMAKE_MODULE_PATH} )
   if( NOT _testSkel )
      message( WARNING "Couldn't find run_tpcnv_test.sh.in" )
      return()
   endif()

   # Create the script that will be run:
   configure_file( ${_testSkel}
      ${CMAKE_CURRENT_BINARY_DIR}/tpcnv_${testName}.sh
      @ONLY )

   # Create a test with it:
   atlas_add_test( ${testName}
      SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/tpcnv_${testName}.sh
      PROPERTIES TIMEOUT 600
      POST_EXEC_SCRIPT "post_tpcnvtest.sh ${testName}" )

endfunction( run_tpcnv_test )

# Allow the user to look for the conventional variable for checking whether
# the package was found:
set( ATHENAPOOLUTILITIESTEST_FOUND TRUE )
