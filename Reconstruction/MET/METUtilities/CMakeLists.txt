# $Id: CMakeLists.txt 788236 2016-12-06 14:03:54Z khoo $
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
   PhysicsAnalysis/ElectronPhotonID/ElectronPhotonSelectorTools
   PhysicsAnalysis/MuonID/MuonSelectorTools
   PhysicsAnalysis/TauID/TauAnalysisTools
   Tools/PathResolver
   ${extra_deps} )

# External dependencies:
find_package( Boost COMPONENTS unit_test_framework )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO )

# Libraries in the package:
atlas_add_library( METUtilitiesLib
   METUtilities/*.h Root/*.cxx
   PUBLIC_HEADERS METUtilities
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AsgTools xAODEgamma xAODJet xAODMissingET
   xAODMuon xAODTau xAODTracking InDetTrackSelectionToolLib PATInterfaces
   METInterface
   PRIVATE_LINK_LIBRARIES EventPrimitives FourMomUtils xAODCore PathResolver )

if( NOT XAOD_STANDALONE )
   atlas_add_component( METUtilities
      src/*.h src/*.cxx src/components/*.cxx
      LINK_LIBRARIES GaudiKernel AthenaBaseComps METInterface xAODEgamma
      xAODMuon xAODTau xAODMissingET xAODJet MuonSelectorToolsLib
      ElectronPhotonSelectorToolsLib TauAnalysisToolsLib METUtilitiesLib  )
endif()

atlas_add_dictionary( METUtilitiesDict
   METUtilities/METUtilitiesDict.h
   METUtilities/selection.xml
   LINK_LIBRARIES METUtilitiesLib )

# Executable(s) in the package:
foreach( utility example_METMaker_METSystematicsTool
         example_METMaker_advanced example_rebuildTrackMET )
   if( XAOD_STANDALONE )
      atlas_add_executable( ${utility}
         util/${utility}.cxx
         INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
         LINK_LIBRARIES ${ROOT_LIBRARIES} xAODRootAccess AsgTools xAODBase
         xAODMissingET xAODCore xAODJet xAODEgamma xAODMuon xAODTau
         JetCalibToolsLib METInterface PATInterfaces METUtilitiesLib )
   elseif( XAOD_ANALYSIS )
      atlas_add_executable( ${utility}
         util/${utility}.cxx
         INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
         LINK_LIBRARIES ${ROOT_LIBRARIES} POOLRootAccess AsgTools xAODBase
         xAODMissingET xAODCore xAODJet xAODEgamma xAODMuon xAODTau
         JetCalibToolsLib METInterface PATInterfaces METUtilitiesLib )
   endif()
endforeach()

# Test(s) in the package:
if( XAOD_STANDALONE )
   foreach( test ut_metMaker ut_metSystematicsTool )
      atlas_add_test( ${test}
         SOURCES test/${test}.cxx
         INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
         LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} xAODRootAccess
         AsgTools xAODBase xAODMissingET METUtilitiesLib )
   endforeach()
endif()

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )

atlas_install_data( data/* )

