#################################################################################
# Package: PMGTools
#################################################################################

# Declare the package name:
atlas_subdir( PMGTools )

# Extra dependencies based on the build environment:
set( extra_deps )
if( XAOD_STANDALONE )
   set( extra_deps Control/xAODRootAccess )
else()
   set( extra_deps Control/AthAnalysisBaseComps PhysicsAnalysis/POOLRootAccess GaudiKernel )
endif()

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthToolSupport/AsgTools
   PhysicsAnalysis/AnalysisCommon/PATInterfaces
   PhysicsAnalysis/Interfaces/PMGAnalysisInterfaces
   PRIVATE
   Event/FourMomUtils
   Event/xAOD/xAODEventInfo
   Event/xAOD/xAODJet
   Event/xAOD/xAODTruth
   Tools/PathResolver
   ${extra_deps} )

# External(s) used by the package:
find_package( ROOT COMPONENTS Core Hist RIO )

# Libraries in the package:
atlas_add_library( PMGToolsLib
   PMGTools/*.h Root/*.cxx
   PUBLIC_HEADERS PMGTools
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools PATInterfaces
   PRIVATE_LINK_LIBRARIES FourMomUtils xAODEventInfo xAODJet xAODTruth
   PathResolver )

if( NOT XAOD_STANDALONE )
   atlas_add_component( PMGTools
      src/components/*.cxx
      LINK_LIBRARIES GaudiKernel PMGToolsLib )
endif()

atlas_add_dictionary( PMGToolsDict
   PMGTools/PMGToolsDict.h
   PMGTools/selection.xml
   LINK_LIBRARIES PMGToolsLib )

# Executable(s) in the package:
if( NOT XAOD_STANDALONE )
   atlas_add_executable( MyPMGApp
      test/MyPMGApp.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} AthAnalysisBaseCompsLib POOLRootAccessLib
      AsgTools xAODJet xAODTruth FourMomUtils PATInterfaces )
endif()

# Test(s) in the package:
if( XAOD_STANDALONE )
   atlas_add_test( ut_PMGSherpa22VJetsWeightTool_test
      SOURCES test/ut_PMGSherpa22VJetsWeightTool_test.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODRootAccess PATInterfaces
      PMGToolsLib )

   atlas_add_test( ut_PMGSherpaVjetsSysTool_test
      SOURCES test/ut_PMGSherpaVjetsSysTool_test.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODRootAccess PATInterfaces
      PMGToolsLib )
else()
   atlas_add_test( ut_PMGTruthWeightTool_test
      SOURCES test/ut_PMGTruthWeightTool_test.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} PMGToolsLib POOLRootAccessLib )
endif()


# Install files from the package:
atlas_install_data( data/*.txt share/*.txt )
