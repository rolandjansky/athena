# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( AthenaPoolMultiTest )

# External dependencies:
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )

# Component(s) in the package:
atlas_add_component( AthenaPoolMultiTest
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CORAL_LIBRARIES} AthenaBaseComps AthenaPoolExampleData AthenaPoolTestData AthenaPoolUtilities DBDataModel GaudiKernel GoodRunsListsLib PersistentDataModel SGTools StoreGateLib xAODEventInfo )

# Install files from the package:
atlas_install_joboptions( share/*.py )

function (athenapoolmultitest_run_test testName jo postScript)
  cmake_parse_arguments( ARG "" "DEPENDS" "" ${ARGN} )

  configure_file( ${CMAKE_CURRENT_SOURCE_DIR}/test/athenapoolmultitest_test.sh.in
                  ${CMAKE_CURRENT_BINARY_DIR}/athenapoolmultitest_${testName}.sh
                  @ONLY )
  atlas_add_test( ${testName}
                  SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/athenapoolmultitest_${testName}.sh
                  POST_EXEC_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/test/${postScript}.sh ${testName} "
                  PROPERTIES TIMEOUT 600
                   )
  if( ARG_DEPENDS )
    set_tests_properties( AthenaPoolMultiTest_${testName}_ctest
                          PROPERTIES DEPENDS AthenaPoolMultiTest_${ARG_DEPENDS}_ctest )
  endif()
endfunction (athenapoolmultitest_run_test)

athenapoolmultitest_run_test( AthenaPoolMultiTestBuildInput SplittableData_jo
                              post_check_bi )
athenapoolmultitest_run_test( AthenaPoolMultiTestEventSplit EventSplit_jo
                              post_check_es
                              DEPENDS AthenaPoolMultiTestBuildInput )
athenapoolmultitest_run_test( AthenaPoolMultiTestEventSplitDecision EventSplitDecision_jo
                              post_check_es
                              DEPENDS AthenaPoolMultiTestBuildInput )
athenapoolmultitest_run_test( AthenaPoolMultiTestEventNoSplit EventNoSplit_jo
                              post_check_es
                              DEPENDS AthenaPoolMultiTestBuildInput )
athenapoolmultitest_run_test( AthenaPoolMultiTestPropagate DecisionPropagation_jo
                              post_check_p
                              DEPENDS AthenaPoolMultiTestEventSplitDecision )
athenapoolmultitest_run_test( AthenaPoolMultiTestStreamSelect CheckStreamSelector_jo
                              post_check_ss
                              DEPENDS AthenaPoolMultiTestEventSplitDecision )
athenapoolmultitest_run_test( AthenaPoolMultiTestPropagateSelect CheckStreamPropagate_jo
                              post_check_ss
                              DEPENDS AthenaPoolMultiTestPropagate )
#athenapoolmultitest_run_test( AthenaPoolMultiTestCheckNull CheckNull_jo
#                              post_check_cn
#                              DEPENDS AthenaPoolMultiTestEventSplit )
#athenapoolmultitest_run_test( AthenaPoolMultiTestCheckOutput CheckImplicit_jo
#                              post_check_co
#                              DEPENDS AthenaPoolMultiTestEventSplit )
