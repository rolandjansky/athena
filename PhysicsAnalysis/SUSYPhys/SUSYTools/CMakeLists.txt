# $Id: CMakeLists.txt 797759 2017-02-18 01:24:37Z zmarshal $
################################################################################
# Package: SUSYTools
################################################################################

 
# Declare the package name:
atlas_subdir( SUSYTools )

# Extra dependencies based on the build environment:
set( extra_deps )
set( extra_libs )
if( NOT XAOD_STANDALONE )
   set( extra_deps GaudiKernel PhysicsAnalysis/POOLRootAccess
      Control/AthAnalysisBaseComps Control/AthenaBaseComps
      Control/AthenaKernel )
   set( extra_libs GaudiKernel AthAnalysisBaseCompsLib )
endif()

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthToolSupport/AsgTools
   Event/xAOD/xAODCore
   Event/xAOD/xAODEgamma
   Event/xAOD/xAODEventInfo
   Event/xAOD/xAODJet
   Event/xAOD/xAODMissingET
   Event/xAOD/xAODTrigMissingET
   Event/xAOD/xAODMuon
   Event/xAOD/xAODTau
   Event/xAOD/xAODTracking
   Event/xAOD/xAODTruth
   PhysicsAnalysis/AnalysisCommon/AssociationUtils
   PhysicsAnalysis/AnalysisCommon/PATInterfaces
   PhysicsAnalysis/Interfaces/PMGAnalysisInterfaces
   PhysicsAnalysis/MCTruthClassifier
   Reconstruction/Jet/JetJvtEfficiency
   Reconstruction/Jet/JetSubStructureUtils
   Trigger/TrigAnalysis/TrigDecisionTool
   PRIVATE
   AsgExternal/Asg_Test
   Control/xAODRootAccess
   Control/AthContainers
   DataQuality/GoodRunsLists
   Event/EventPrimitives
   Event/FourMomUtils
   Event/xAOD/xAODBTagging
   Event/xAOD/xAODBase
   Event/xAOD/xAODCaloEvent
   Event/xAOD/xAODCutFlow
   Event/xAOD/xAODPrimitives
   PhysicsAnalysis/AnalysisCommon/IsolationSelection
   PhysicsAnalysis/AnalysisCommon/PileupReweighting
   PhysicsAnalysis/ElectronPhotonID/ElectronEfficiencyCorrection
   PhysicsAnalysis/ElectronPhotonID/ElectronPhotonFourMomentumCorrection
   PhysicsAnalysis/ElectronPhotonID/ElectronPhotonSelectorTools
   PhysicsAnalysis/ElectronPhotonID/ElectronPhotonShowerShapeFudgeTool
   PhysicsAnalysis/ElectronPhotonID/IsolationCorrections
   PhysicsAnalysis/ElectronPhotonID/PhotonEfficiencyCorrection
   PhysicsAnalysis/Interfaces/AsgAnalysisInterfaces
   PhysicsAnalysis/JetMissingEtID/JetSelectorTools
   PhysicsAnalysis/JetTagging/JetTagPerformanceCalibration/xAODBTaggingEfficiency
   PhysicsAnalysis/MuonID/MuonIDAnalysis/MuonEfficiencyCorrections
   PhysicsAnalysis/MuonID/MuonIDAnalysis/MuonMomentumCorrections
   PhysicsAnalysis/MuonID/MuonSelectorTools
   PhysicsAnalysis/TauID/TauAnalysisTools
   Reconstruction/Jet/JetCPInterfaces
   Reconstruction/Jet/JetCalibTools
   Reconstruction/Jet/JetInterface
   Reconstruction/Jet/JetResolution
   Reconstruction/Jet/JetUncertainties
   Reconstruction/Jet/JetMomentTools
   Reconstruction/MET/METInterface
   Reconstruction/MET/METUtilities
   Reconstruction/tauRecTools
   Tools/PathResolver
   Trigger/TrigAnalysis/TriggerMatchingTool
   Trigger/TrigConfiguration/TrigConfInterfaces
   Trigger/TrigConfiguration/TrigConfxAOD
   ${extra_deps} )

# External dependencies:
find_package( Boost )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO )
find_package( GTest )

# Libraries in the package:
atlas_add_library( SUSYToolsLib
   SUSYTools/*.h Root/*.cxx
   PUBLIC_HEADERS SUSYTools
   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${BOOST_INCLUDE_DIRS}
   LINK_LIBRARIES AsgTools xAODCore xAODEgamma xAODEventInfo
   xAODJet xAODMissingET xAODMuon xAODTau xAODTracking xAODTruth
   AssociationUtilsLib PATInterfaces TrigDecisionToolLib 
   MCTruthClassifierLib JetJvtEfficiencyLib JetSubStructureUtils
   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} ${BOOST_LIBRARIES}
   AthContainers EventPrimitives FourMomUtils xAODBTagging xAODBase
   xAODPrimitives IsolationSelectionLib PileupReweightingLib
   ElectronEfficiencyCorrectionLib ElectronPhotonFourMomentumCorrectionLib
   ElectronPhotonSelectorToolsLib ElectronPhotonShowerShapeFudgeToolLib
   IsolationCorrectionsLib PhotonEfficiencyCorrectionLib JetSelectorToolsLib
   xAODBTaggingEfficiencyLib MuonEfficiencyCorrectionsLib
   MuonMomentumCorrectionsLib MuonSelectorToolsLib TauAnalysisToolsLib
   JetCPInterfaces JetCalibToolsLib JetInterface JetResolutionLib
   JetUncertaintiesLib JetMomentToolsLib METInterface METUtilitiesLib
   PathResolver TriggerMatchingToolLib TrigConfInterfaces TrigConfxAODLib
   xAODTrigMissingET ${extra_libs} )

if( NOT XAOD_STANDALONE )
   atlas_add_component( SUSYTools
      src/*.h src/*.cxx src/components/*.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps GaudiKernel
      AthenaKernel AthAnalysisBaseCompsLib xAODEventInfo xAODMuon xAODPrimitives
      xAODJet xAODBTagging xAODEgamma xAODMissingET xAODTracking xAODTau
      TauAnalysisToolsLib xAODCore AthContainers AsgTools xAODBase xAODCutFlow
      PATInterfaces PathResolver SUSYToolsLib )
endif()

atlas_add_dictionary( SUSYToolsDict
   SUSYTools/SUSYToolsDict.h
   SUSYTools/selection.xml
   LINK_LIBRARIES SUSYToolsLib )

# Executable(s) in the package:
set( extra_libs )
if( NOT XAOD_STANDALONE )
   set( extra_libs POOLRootAccessLib )
endif()
atlas_add_executable( SUSYToolsTester
   util/SUSYToolsTester.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} xAODRootAccess xAODEventInfo xAODMuon
   xAODEgamma xAODTau xAODTruth xAODJet xAODCaloEvent xAODCore xAODMissingET
   xAODBTaggingEfficiencyLib xAODBase TauAnalysisToolsLib GoodRunsListsLib
   PileupReweightingLib PATInterfaces PathResolver METUtilitiesLib xAODCutFlow
   SUSYToolsLib ${extra_libs} )

atlas_add_executable( SUSYTools_check_xsections
   util/check_xsections.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} SUSYToolsLib )

if( XAOD_STANDALONE )
   atlas_add_executable( SUSYTools_check_syst
      util/check_syst.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} xAODRootAccess PATInterfaces
      SUSYToolsLib )
endif()

# Test(s) in the package:
if( XAOD_STANDALONE )
   atlas_add_test( gt_SUSYTools_xs_loader
      SOURCES test/gt_SUSYTools_xs_loader.cxx
      INCLUDE_DIRS ${GTEST_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${GTEST_LIBRARIES} ${ROOT_LIBRARIES} SampleHandler )

   atlas_add_test( ut_SUSYTools_test_xs_files
      SOURCES test/ut_SUSYTools_test_xs_files.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} SampleHandler )

   atlas_add_test( ut_SUSYTools_config_checker
      SCRIPT test/ut_SUSYTools_config_checker.py )
   atlas_add_test( ut_SUSYTools_wp_checker
      SCRIPT test/ut_SUSYTools_wp_checker.py )
   # This test takes a bit longer, but it's worth it
   set_tests_properties( SUSYTools_ut_SUSYTools_wp_checker_ctest PROPERTIES TIMEOUT 600)
   atlas_add_test( ut_SUSYToolsTester_atlfast
      SOURCES test/ut_SUSYToolsTester_atlfast.cxx )
   atlas_add_test( ut_SUSYToolsTester_data
      SOURCES test/ut_SUSYToolsTester_data.cxx )
   atlas_add_test( ut_SUSYToolsTester_mc
      SOURCES test/ut_SUSYToolsTester_mc.cxx )
   atlas_add_test( ut_SUSYToolsTester_mc_w_2016_lumi
      SOURCES test/ut_SUSYToolsTester_mc_w_2016_lumi.cxx )
endif()

# Install files from the package:
atlas_install_python_modules( python/*.py )
atlas_install_joboptions( share/*.py )
atlas_install_data( data/* )

