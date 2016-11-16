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
   ${extra_deps} )

# External dependencies:
find_package( GTest )

# Libraries in the package:
atlas_add_root_dictionary( AsgExampleToolsLib AsgExampleToolsCintDict
   ROOT_HEADERS AsgExampleTools/UnitTestTool1.h Root/LinkDef.h
   EXTERNAL_PACKAGES ROOT )

atlas_add_library( AsgExampleToolsLib
   AsgExampleTools/*.h Root/*.cxx ${AsgExampleToolsCintDict}
   PUBLIC_HEADERS AsgExampleTools
   LINK_LIBRARIES AsgTools )

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
   LINK_LIBRARIES ${GTEST_LIBRARIES} AsgTools AsgExampleToolsLib ${extra_libs} )

atlas_add_test( gt_AnaToolHandle_test
   SOURCES test/gt_AnaToolHandle_test.cxx
   INCLUDE_DIRS ${GTEST_INCLUDE_DIRS}
   LINK_LIBRARIES ${GTEST_LIBRARIES} AsgTools AsgExampleToolsLib )
