# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( AsgExampleTools )

if (SIMULATIONBASE)
  set (extra_lib )
else()
  set (extra_lib xAODMuon)
endif()

# External dependencies:
find_package( GTest )
find_package( GMock )

# Libraries in the package:
atlas_add_root_dictionary( AsgExampleToolsLib AsgExampleToolsCintDict
   ROOT_HEADERS AsgExampleTools/UnitTestTool1.h Root/LinkDef.h
   EXTERNAL_PACKAGES ROOT )

atlas_add_library( AsgExampleToolsLib
   AsgExampleTools/*.h Root/*.cxx ${AsgExampleToolsCintDict}
   PUBLIC_HEADERS AsgExampleTools
   LINK_LIBRARIES AsgTools AsgDataHandlesLib AsgTestingLib ${extra_lib} )

if( NOT XAOD_STANDALONE )
   atlas_add_component( AsgExampleTools
      src/*.h src/*.cxx src/components/*.cxx
      LINK_LIBRARIES AthenaBaseComps GaudiKernel AsgExampleToolsLib )
endif()

atlas_add_dictionary( AsgExampleToolsDict
   AsgExampleTools/AsgExampleToolsDict.h
   AsgExampleTools/selection.xml
   LINK_LIBRARIES AsgExampleToolsLib )

# Executable(s) in the package:
atlas_add_executable( AsgExampleTools_hello
   util/hello.cxx
   LINK_LIBRARIES AsgExampleToolsLib )

# Test(s) in the package:
set( extra_libs )
if( XAOD_STANDALONE )
   set( extra_libs xAODRootAccess )
endif()
atlas_add_test( gt_ToolHandle_test
   SOURCES test/gt_ToolHandle_test.cxx
   INCLUDE_DIRS ${GTEST_INCLUDE_DIRS}
   LINK_LIBRARIES ${GTEST_LIBRARIES} AsgTools AsgExampleToolsLib AsgTestingLib ${extra_libs} )
set_tests_properties (AsgExampleTools_gt_ToolHandle_test_ctest PROPERTIES LABELS "AsgTools;AsgExampleTools" )

atlas_add_test( gt_AnaToolHandle_test
   SOURCES test/gt_AnaToolHandle_test.cxx
   INCLUDE_DIRS ${GTEST_INCLUDE_DIRS}
   LINK_LIBRARIES ${GTEST_LIBRARIES} AsgTools AsgExampleToolsLib AsgTestingLib )
set_tests_properties (AsgExampleTools_gt_AnaToolHandle_test_ctest PROPERTIES LABELS "AsgTools;AsgExampleTools" )

atlas_add_test( gt_AsgToolConfig
   SOURCES test/gt_AsgToolConfig.cxx
   INCLUDE_DIRS ${GTEST_INCLUDE_DIRS}
   LINK_LIBRARIES ${GTEST_LIBRARIES} AsgTools AsgExampleToolsLib AsgTestingLib )
set_tests_properties (AsgExampleTools_gt_AsgToolConfig_ctest PROPERTIES LABELS "AsgTools;AsgExampleTools" )


if( XAOD_STANDALONE )

   atlas_add_test( gt_AsgTool
      SOURCES test/gt_AsgTool.cxx
      INCLUDE_DIRS ${GTEST_INCLUDE_DIRS}
      LINK_LIBRARIES ${GMOCK_LIBRARIES} AsgTools AsgExampleToolsLib AsgTestingLib )
   set_tests_properties (AsgExampleTools_gt_AsgTool_ctest PROPERTIES LABELS "AsgTools;AsgExampleTools" )

   atlas_add_test( gt_MessageCheck
      SOURCES test/gt_MessageCheck.cxx
      INCLUDE_DIRS ${GTEST_INCLUDE_DIRS}
      LINK_LIBRARIES ${GMOCK_LIBRARIES} AsgTools AsgExampleToolsLib AsgTestingLib )
   set_tests_properties (AsgExampleTools_gt_MessageCheck_ctest PROPERTIES LABELS "AsgTools;AsgExampleTools" )

   atlas_add_test( gt_TProperty
      SOURCES test/gt_TProperty.cxx
      INCLUDE_DIRS ${GTEST_INCLUDE_DIRS}
      LINK_LIBRARIES ${GTEST_LIBRARIES} AsgTools AsgExampleToolsLib AsgTestingLib )
   set_tests_properties (AsgExampleTools_gt_TProperty_ctest PROPERTIES LABELS "AsgTools;AsgExampleTools" )

   atlas_add_test( gt_asgtools_toolhandle_test
      SOURCES test/gt_asgtools_toolhandle_test.cxx
      INCLUDE_DIRS ${GTEST_INCLUDE_DIRS}
      LINK_LIBRARIES ${GTEST_LIBRARIES} AsgTools AsgExampleToolsLib AsgTestingLib )
   set_tests_properties (AsgExampleTools_gt_asgtools_toolhandle_test_ctest PROPERTIES LABELS "AsgTools;AsgExampleTools" )

endif()

if (NOT SIMULATIONBASE)
   if (NOT XAOD_STANDALONE)
     set (extra_libs POOLRootAccessLib)
   else()
     set (extra_libs )
   endif()

   atlas_add_test( gt_DataHandlesTest
      SOURCES test/gt_DataHandlesTest.cxx
      INCLUDE_DIRS ${GTEST_INCLUDE_DIRS}
      LINK_LIBRARIES ${GTEST_LIBRARIES} AsgTools AsgExampleToolsLib AsgTestingLib ${extra_libs})
   set_tests_properties (AsgExampleTools_gt_DataHandlesTest_ctest PROPERTIES LABELS "AsgDataHandles;AsgExampleTools" )
endif()

atlas_add_test( gt_AnaCheck
   SOURCES test/gt_AnaCheck.cxx
   INCLUDE_DIRS ${GTEST_INCLUDE_DIRS}
   LINK_LIBRARIES ${GMOCK_LIBRARIES} AsgTools AsgExampleToolsLib AsgTestingLib )
set_tests_properties (AsgExampleTools_gt_AnaCheck_ctest PROPERTIES LABELS "AsgTools;AsgExampleTools" )

atlas_add_test( gt_asgtools_toolstore
   SOURCES test/gt_asgtools_toolstore.cxx
   INCLUDE_DIRS ${GTEST_INCLUDE_DIRS}
   LINK_LIBRARIES ${GMOCK_LIBRARIES} AsgTools AsgExampleToolsLib AsgTestingLib )
set_tests_properties (AsgExampleTools_gt_asgtools_toolstore_ctest PROPERTIES LABELS "AsgTools;AsgExampleTools" )
