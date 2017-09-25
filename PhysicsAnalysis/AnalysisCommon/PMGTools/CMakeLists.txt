#################################################################################
# Package: PMGTools
#################################################################################

# Declare the package name:
atlas_subdir( PMGTools )

# Extra dependencies based on the build environment:
set( extra_deps )
# ... for AnalysisBase
if( XAOD_STANDALONE )
   set( extra_deps PhysicsAnalysis/POOLRootAccess )
# ... for AthAnalysisBase/Athena
else()
   set( extra_deps Control/AthAnalysisBaseComps GaudiKernel PhysicsAnalysis/POOLRootAccess )
endif()

# Extra libraries based on the build environment:
set( extra_libs )
# ... for AnalysisBase
if( XAOD_STANDALONE )
   set( extra_libs )
# ... for AthAnalysisBase
elseif( XAOD_ANALYSIS AND NOT XAOD_STANDALONE )
   set( extra_libs POOLRootAccessLib )
# ... for Athena
else()
   set( extra_libs POOLRootAccess )
endif()


# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthToolSupport/AsgTools
   PhysicsAnalysis/AnalysisCommon/PATInterfaces
   PhysicsAnalysis/Interfaces/PMGAnalysisInterfaces
   PRIVATE
   Control/xAODRootAccess
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
   LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools PATInterfaces xAODTruth
   PRIVATE_LINK_LIBRARIES FourMomUtils PathResolver xAODEventInfo xAODJet )

if( NOT XAOD_STANDALONE )
   atlas_add_component( PMGTools
      src/components/*.cxx
      LINK_LIBRARIES GaudiKernel PMGToolsLib )
endif()

atlas_add_dictionary( PMGToolsDict
   PMGTools/PMGToolsDict.h
   PMGTools/selection.xml
   LINK_LIBRARIES PMGToolsLib )

## Executable(s) in the package:
# ... for AthAnalysisBase/Athena
if( NOT XAOD_STANDALONE )
   atlas_add_executable( MyPMGApp
      test/MyPMGApp.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} ${extra_libs} AsgTools AthAnalysisBaseCompsLib GaudiKernel PMGToolsLib xAODJet )
endif()

# Test(s) in the package:
#
# ... for AnalysisBase
if( XAOD_STANDALONE )
   atlas_add_test( ut_PMGSherpa22VJetsWeightTool_test
      SOURCES test/ut_PMGSherpa22VJetsWeightTool_test.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools PATInterfaces PMGToolsLib xAODRootAccess )

   atlas_add_test( ut_PMGSherpaVjetsSysTool_test
      SOURCES test/ut_PMGSherpaVjetsSysTool_test.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools PATInterfaces PMGToolsLib xAODRootAccess )
# ... for AthAnalysisBase/Athena
else()
   atlas_add_test( ut_PMGTruthWeightTool_test
      SOURCES test/ut_PMGTruthWeightTool_test.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} ${extra_libs} PMGToolsLib )
endif()

# Install files from the package:
atlas_install_data( data/*.txt share/*.txt )
