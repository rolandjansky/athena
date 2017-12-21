# $Id: CMakeLists.txt 801725 2017-03-28 19:23:28Z khoo $
################################################################################
# Package: METUtilities
################################################################################

# Declare the package name:
atlas_subdir( METUtilities )

# Extra dependencies, based on the environment:
set( extra_deps )
if( XAOD_STANDALONE )
   set( extra_deps Control/xAODRootAccess
      Reconstruction/Jet/JetCalibTools )
elseif( XAOD_ANALYSIS )
   set( extra_deps GaudiKernel
        Control/AthenaBaseComps
	PhysicsAnalysis/POOLRootAccess
        Reconstruction/Jet/JetCalibTools )
else()
   set( extra_deps GaudiKernel
        Control/AthenaBaseComps )
endif()

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthToolSupport/AsgTools
   Event/xAOD/xAODEgamma
   Event/xAOD/xAODJet
   Event/xAOD/xAODMissingET
   Event/xAOD/xAODMuon
   Event/xAOD/xAODTau
   Event/xAOD/xAODTracking
   InnerDetector/InDetRecTools/InDetTrackSelectionTool
   PhysicsAnalysis/AnalysisCommon/PATInterfaces
   Reconstruction/MET/METInterface
   PRIVATE
   Event/EventPrimitives
   Event/FourMomUtils
   Event/xAOD/xAODCore
   PhysicsAnalysis/Interfaces/EgammaAnalysisInterfaces
   PhysicsAnalysis/MuonID/MuonSelectorTools
   PhysicsAnalysis/TauID/TauAnalysisTools
   Tools/PathResolver
   ${extra_deps} )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO )
find_package( GTest )

# Libraries in the package:
atlas_add_library( METUtilitiesLib
   METUtilities/*.h Root/*.cxx
   PUBLIC_HEADERS METUtilities
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}  ${GTEST_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODEgamma xAODJet xAODMissingET
   xAODMuon xAODTau xAODTracking InDetTrackSelectionToolLib PATInterfaces
   METInterface
   PRIVATE_LINK_LIBRARIES EventPrimitives FourMomUtils xAODCore PathResolver )

if( NOT XAOD_STANDALONE )
   atlas_add_component( METUtilities
      src/*.h src/*.cxx src/components/*.cxx
      LINK_LIBRARIES GaudiKernel AthenaBaseComps METInterface xAODEgamma
      xAODMuon xAODTau xAODMissingET xAODJet MuonSelectorToolsLib
      EgammaAnalysisInterfacesLib TauAnalysisToolsLib METUtilitiesLib  )
endif()

atlas_add_dictionary( METUtilitiesDict
   METUtilities/METUtilitiesDict.h
   METUtilities/selection.xml
   LINK_LIBRARIES METUtilitiesLib )


#set readLib, used below to compile apps/tests
set( readLib )
if( XAOD_STANDALONE ) 
  set( readLib xAODRootAccess )
else()
  set( readLib POOLRootAccessLib )
endif()

# Executable(s) in the package:
# only compiled in analysis releases
if( XAOD_ANALYSIS )
  foreach( utility example_METMaker_METSystematicsTool
          example_METMaker_advanced example_rebuildTrackMET )
    atlas_add_executable( ${utility}
          util/${utility}.cxx
          INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
          LINK_LIBRARIES ${ROOT_LIBRARIES} ${readLib} AsgTools xAODBase
          xAODMissingET xAODCore xAODJet xAODEgamma xAODMuon xAODTau
          JetCalibToolsLib METInterface PATInterfaces METUtilitiesLib )
  endforeach()
endif()


# Test(s) in the package:
if( XAOD_ANALYSIS )
  foreach( test gt_metMaker gt_metSystematicsTool )
    atlas_add_test( ${test}
          SOURCES test/${test}.cxx
          INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${GTEST_INCLUDE_DIRS}
          LINK_LIBRARIES ${ROOT_LIBRARIES} ${GTEST_LIBRARIES} ${readLib}
                AsgTools xAODBase xAODMissingET METUtilitiesLib )
  endforeach()
endif()

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

atlas_install_data( data/* )

