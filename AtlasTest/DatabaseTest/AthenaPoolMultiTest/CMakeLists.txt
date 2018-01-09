################################################################################
# Package: AthenaPoolMultiTest
################################################################################

# Declare the package name:
atlas_subdir( AthenaPoolMultiTest )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/MinimalRunTime
                          Database/AthenaPOOL/AthenaPoolExample/AthenaPoolExampleAlgorithms
                          PRIVATE
                          AtlasTest/DatabaseTest/AthenaPoolTestData
                          Control/AthenaBaseComps
                          Control/SGTools
                          Control/StoreGate
                          DataQuality/GoodRunsLists
                          Database/APR/CollectionBase
                          Database/APR/CollectionUtilities
                          Database/APR/POOLCore
                          Database/AthenaPOOL/AthenaPoolExample/AthenaPoolExampleData
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Database/PersistentDataModel
                          Database/AthenaPOOL/DBDataModel
                          Event/ByteStreamCnvSvc
                          Event/ByteStreamData
                          Event/EventInfo
                          Event/xAOD/xAODEventInfo
                          GaudiKernel
                          TestPolicy )

# External dependencies:
find_package( CORAL COMPONENTS CoralBase CoralKernel RelationalAccess )

# Component(s) in the package:
atlas_add_library( AthenaPoolMultiTestLib
                   src/*.cxx
                   PUBLIC_HEADERS AthenaPoolMultiTest
                   PRIVATE_INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
                   LINK_LIBRARIES StoreGateLib SGtests GoodRunsListsLib ByteStreamData_test
                   PRIVATE_LINK_LIBRARIES ${CORAL_LIBRARIES} AthenaPoolTestData AthenaBaseComps SGTools CollectionBase CollectionUtilities POOLCore AthenaPoolExampleData AthenaPoolUtilities PersistentDataModel DBDataModel ByteStreamData EventInfo xAODEventInfo GaudiKernel )

atlas_add_component( AthenaPoolMultiTest
                     src/components/*.cxx
                     INCLUDE_DIRS ${CORAL_INCLUDE_DIRS}
                     LINK_LIBRARIES ${CORAL_LIBRARIES} AthenaPoolTestData AthenaBaseComps SGTools StoreGateLib SGtests GoodRunsListsLib CollectionBase CollectionUtilities POOLCore AthenaPoolExampleData AthenaPoolUtilities DBDataModel ByteStreamData ByteStreamData_test EventInfo xAODEventInfo GaudiKernel AthenaPoolMultiTestLib )

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
athenapoolmultitest_run_test( AthenaPoolMultiTestCollectInput TestSimpleCollection
                              post_check_ci
                              DEPENDS AthenaPoolMultiTestBuildInput )
athenapoolmultitest_run_test( AthenaPoolMultiTestEventSplit EventSplit_jo
                              post_check_es
                              DEPENDS AthenaPoolMultiTestBuildInput )
athenapoolmultitest_run_test( AthenaPoolMultiTestCheckCollections CheckExplicit_jo
                              post_check_co
                              DEPENDS AthenaPoolMultiTestEventSplit )
athenapoolmultitest_run_test( AthenaPoolMultiTestCheckNull CheckNull_jo
                              post_check_cn
                              DEPENDS AthenaPoolMultiTestEventSplit )
athenapoolmultitest_run_test( AthenaPoolMultiTestCheckOutput CheckImplicit_jo
                              post_check_co
                              DEPENDS AthenaPoolMultiTestEventSplit )

#athenapoolmultitest_run_test( AthenaPoolMultiTestBSMetaWrite BSMetaWrite
#                              post_check_bs )
#athenapoolmultitest_run_test( AthenaPoolMultiTestBSMetaWriteNone BSMetaWriteNone
#                              post_check_bs
#                              DEPENDS AthenaPoolMultiTestBSMetaWrite )
