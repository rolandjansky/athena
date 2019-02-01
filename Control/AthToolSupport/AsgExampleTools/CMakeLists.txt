# $Id: CMakeLists.txt 779405 2016-10-20 13:44:57Z krasznaa $
################################################################################
# Package: AsgExampleTools
################################################################################

# Declare the package name:
atlas_subdir( AsgExampleTools )

# Extra dependencies based on the build environment:
set( extra_deps )
if( XAOD_STANDALONE )
   set( extra_deps Control/xAODRootAccess )
else()
   set( extra_deps Control/AthenaBaseComps GaudiKernel )
endif()

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthToolSupport/AsgTools
   PRIVATE
   Control/AthToolSupport/AsgTesting
   ${extra_deps} )

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
   LINK_LIBRARIES AsgTools AsgTestingLib )

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

if( XAOD_STANDALONE )

   atlas_add_test( gt_AsgTool
      SOURCES test/gt_AsgTool.cxx
      INCLUDE_DIRS ${GTEST_INCLUDE_DIRS}
      LINK_LIBRARIES ${GMOCK_LIBRARIES} AsgTools AsgExampleToolsLib AsgTestingLib
      EXTRA_PATTERNS "\([0-9]+ ms\)|\([0-9]+ ms total\)" )
   set_tests_properties (AsgExampleTools_gt_AsgTool_ctest PROPERTIES LABELS "AsgTools;AsgExampleTools" )

   atlas_add_test( gt_MessageCheck
      SOURCES test/gt_MessageCheck.cxx
      INCLUDE_DIRS ${GTEST_INCLUDE_DIRS}
      LINK_LIBRARIES ${GMOCK_LIBRARIES} AsgTools AsgExampleToolsLib AsgTestingLib
      EXTRA_PATTERNS "\([0-9]+ ms\)|\([0-9]+ ms total\)" )
   set_tests_properties (AsgExampleTools_gt_MessageCheck_ctest PROPERTIES LABELS "AsgTools;AsgExampleTools" )

   atlas_add_test( gt_TProperty
      SOURCES test/gt_TProperty.cxx
      INCLUDE_DIRS ${GTEST_INCLUDE_DIRS}
      LINK_LIBRARIES ${GTEST_LIBRARIES} AsgTools AsgExampleToolsLib AsgTestingLib
      EXTRA_PATTERNS "\([0-9]+ ms\)|\([0-9]+ ms total\)" )
   set_tests_properties (AsgExampleTools_gt_TProperty_ctest PROPERTIES LABELS "AsgTools;AsgExampleTools" )

   atlas_add_test( gt_asgtools_toolhandle_test
      SOURCES test/gt_asgtools_toolhandle_test.cxx
      INCLUDE_DIRS ${GTEST_INCLUDE_DIRS}
      LINK_LIBRARIES ${GTEST_LIBRARIES} AsgTools AsgExampleToolsLib AsgTestingLib
      EXTRA_PATTERNS "\([0-9]+ ms\)|\([0-9]+ ms total\)" )
   set_tests_properties (AsgExampleTools_gt_asgtools_toolhandle_test_ctest PROPERTIES LABELS "AsgTools;AsgExampleTools" )

endif()


atlas_add_test( gt_AnaCheck
   SOURCES test/gt_AnaCheck.cxx
   INCLUDE_DIRS ${GTEST_INCLUDE_DIRS}
   LINK_LIBRARIES ${GMOCK_LIBRARIES} AsgTools AsgExampleToolsLib AsgTestingLib GaudiKernel
   EXTRA_PATTERNS "\([0-9]+ ms\)|\([0-9]+ ms total\)" )
set_tests_properties (AsgExampleTools_gt_AnaCheck_ctest PROPERTIES LABELS "AsgTools;AsgExampleTools" )
