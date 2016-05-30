# $Id: CMakeLists.txt 750885 2016-05-30 16:41:48Z krasznaa $
################################################################################
# Package: AsgExampleTools
################################################################################

# Declare the package name:
atlas_subdir( AsgExampleTools )

# External dependencies:
find_package( GTest )

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthToolSupport/AsgTools
   PRIVATE
   Control/AthenaBaseComps
   GaudiKernel )

# Component(s) in the package:
atlas_add_component( AsgExampleTools
   src/*.cxx
   Root/AsgHelloTool.cxx
   AsgTools/UnitTestTool1.h AsgTools/UnitTestTool1A.h AsgTools/UnitTestTool2.h
	Root/UnitTestTool1.cxx Root/UnitTestTool1A.cxx Root/UnitTestTool2.cxx
   src/components/*.cxx
   LINK_LIBRARIES AsgTools AthenaBaseComps GaudiKernel )

# Install files from the package:
atlas_install_headers( AsgExampleTools )

# Test(s) in the package:
atlas_add_test( gt_ToolHandle_test
   SOURCES test/gt_ToolHandle_test.cxx
   Root/UnitTestTool1.cxx Root/UnitTestTool2.cxx
   INCLUDE_DIRS ${GTEST_INCLUDE_DIRS}
   LINK_LIBRARIES ${GTEST_LIBRARIES} AsgTools )

atlas_add_test( gt_AnaToolHandle_test
   SOURCES test/gt_AnaToolHandle_test.cxx
   Root/UnitTestTool1.cxx Root/UnitTestTool2.cxx
   INCLUDE_DIRS ${GTEST_INCLUDE_DIRS}
   LINK_LIBRARIES ${GTEST_LIBRARIES} AsgTools )

atlas_add_test( gt_UnitTest_test
   SOURCES test/gt_UnitTest_test.cxx
   INCLUDE_DIRS ${GTEST_INCLUDE_DIRS}
   LINK_LIBRARIES ${GTEST_LIBRARIES} AsgTools )
