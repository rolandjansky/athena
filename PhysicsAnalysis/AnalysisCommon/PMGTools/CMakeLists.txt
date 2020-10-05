#################################################################################
# Package: PMGTools
#################################################################################

# Declare the package name:
atlas_subdir( PMGTools )

# Extra dependencies based on the build environment:
set( extra_private_deps )
# ... for AnalysisBase
if( XAOD_STANDALONE )
   set( extra_private_deps Control/xAODRootAccess )
# ... for AthAnalysisBase/Athena
else()
   set( extra_private_deps Control/AthAnalysisBaseComps PhysicsAnalysis/POOLRootAccess GaudiKernel Event/EventInfo)
endif()

# Extra libraries based on the build environment:
set( xaod_access_lib )
set( extra_private_libs )
# ... for AnalysisBase
if( XAOD_STANDALONE )
   set( xaod_access_lib xAODRootAccess )
# ... for AthAnalysisBase (Athena calls this POOLRootAccess)
else()
   set( xaod_access_lib POOLRootAccessLib )
   set( extra_private_libs EventInfo )
endif()


# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthToolSupport/AsgTools
   Event/xAOD/xAODTruth
   PhysicsAnalysis/AnalysisCommon/PATCore
   PhysicsAnalysis/AnalysisCommon/PATInterfaces
   PhysicsAnalysis/Interfaces/PMGAnalysisInterfaces
   PRIVATE
   Event/FourMomUtils
   Event/xAOD/xAODEventInfo
   Event/xAOD/xAODJet
   Event/xAOD/xAODMetaData
   PhysicsAnalysis/D3PDTools/RootCoreUtils
   Tools/PathResolver
   ${extra_private_deps} )

# External(s) used by the package:
find_package( ROOT COMPONENTS Core Hist RIO )
find_package( Boost )

# Libraries in the package:
atlas_add_library( PMGToolsLib
   PMGTools/*.h Root/*.cxx
   PUBLIC_HEADERS PMGTools
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools PATCoreLib PATInterfaces xAODTruth
   PRIVATE_LINK_LIBRARIES FourMomUtils PathResolver RootCoreUtils xAODEventInfo xAODJet xAODMetaData ${extra_private_libs})

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
#
# ... for Athena/AthAnalysis
if( NOT XAOD_STANDALONE )
   atlas_add_executable( MyPMGApp
      test/MyPMGApp.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} AthAnalysisBaseCompsLib ${xaod_access_lib}
      AsgTools xAODJet xAODTruth FourMomUtils PATInterfaces PMGToolsLib )
endif()

# Test(s) in the package:
#
# ... for AnalysisBase
if( XAOD_STANDALONE )
   atlas_add_test( ut_PMGSherpa22VJetsWeightTool_test
      SOURCES test/ut_PMGSherpa22VJetsWeightTool_test.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools PATInterfaces PMGToolsLib ${xaod_access_lib} )

   atlas_add_test( ut_PMGSherpaVjetsSysTool_test
      SOURCES test/ut_PMGSherpaVjetsSysTool_test.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools PATInterfaces PMGToolsLib ${xaod_access_lib} )
endif()
# ... AthAnalysis/AnalysisBase
atlas_add_test( ut_PMGTruthWeightTool_test
   SOURCES test/ut_PMGTruthWeightTool_test.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} PMGToolsLib ${xaod_access_lib} )
