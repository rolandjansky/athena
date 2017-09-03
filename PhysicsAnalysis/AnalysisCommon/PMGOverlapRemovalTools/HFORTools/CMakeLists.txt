# $Id: CMakeLists.txt 777718 2016-10-11 16:52:13Z jrobinso $
################################################################################
# Package: HFORTools
################################################################################

# Declare the package name:
atlas_subdir( HFORTools )

# Extra dependencies, based on the environment:
set( extra_deps )
set( extra_deps Control/AthenaBaseComps )
if( XAOD_STANDALONE )
    set( extra_deps Control/xAODRootAccess )
else()
   set( extra_deps Control/AthenaBaseComps GaudiKernel PhysicsAnalysis/POOLRootAccess)
endif()

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthToolSupport/AsgTools
   Event/xAOD/xAODJet
   Event/xAOD/xAODTruth
   Event/xAOD/xAODEventInfo
   PhysicsAnalysis/Interfaces/PMGAnalysisInterfaces
   PRIVATE
   Event/xAOD/xAODBase
   Tools/PathResolver
   ${extra_deps} )

# External dependencies:
find_package( Boost COMPONENTS program_options )
find_package( ROOT COMPONENTS Core Hist RIO Gpad )
find_package( Eigen )

# Libraries in the package:
atlas_add_library( HFORToolsLib
   HFORTools/*.h Root/*.cxx
   PUBLIC_HEADERS HFORTools
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} AsgTools xAODCore xAODBase xAODJet xAODTruth xAODEventInfo PathResolver )

if( NOT XAOD_STANDALONE )
   atlas_add_component( HFORTools
      src/*.h src/*.cxx src/components/*.cxx
      LINK_LIBRARIES AthenaBaseComps GaudiKernel HFORToolsLib )
endif()

atlas_add_dictionary( HFORToolsDict
   HFORTools/HFORToolsDict.h
   HFORTools/selection.xml
   LINK_LIBRARIES HFORToolsLib )

if( NOT XAOD_STANDALONE )
  # Test(s) in the package:
  atlas_add_test( ut_HFORTools_test
    SOURCES test/ut_HFORTools_test.cxx
    INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
    LINK_LIBRARIES ${Boost_LIBRARIES} ${GTEST_LIBRARIES} POOLRootAccessLib AthenaBaseComps GaudiKernel HFORToolsLib )
endif()

# Install files from the package:
atlas_install_data( data/*.cfg )
