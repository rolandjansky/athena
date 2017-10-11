
# The name of the package:
atlas_subdir( QuickAna )

# Extra dependencies, based on the build environment:
set( extra_deps )
if( XAOD_STANDALONE )
   set( extra_deps PhysicsAnalysis/D3PDTools/EventLoop
      PRIVATE Control/xAODRootAccessInterfaces Control/xAODRootAccess )
else()
   set( extra_deps PRIVATE Control/AthenaBaseComps Control/AthAnalysisBaseComps
      Control/StoreGate GaudiKernel )
endif()

# The dependencies of the package:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthContainers
   Control/AthToolSupport/AsgTools
   Event/xAOD/xAODEgamma
   Event/xAOD/xAODJet
   Event/xAOD/xAODMuon
   Event/xAOD/xAODTau
   Event/xAOD/xAODCore
   Event/xAOD/xAODMissingET
   Event/xAOD/xAODEventInfo
   Event/xAOD/xAODTruth
   PhysicsAnalysis/AnalysisCommon/AssociationUtils
   PhysicsAnalysis/AnalysisCommon/IsolationSelection
   PhysicsAnalysis/AnalysisCommon/PATInterfaces
   PhysicsAnalysis/D3PDTools/RootCoreUtils
   ${extra_deps}
   Control/CxxUtils
   Event/EventPrimitives
   Event/xAOD/xAODBase
   Event/xAOD/xAODTracking
   PhysicsAnalysis/AnalysisCommon/PileupReweighting
   PhysicsAnalysis/ElectronPhotonID/ElectronEfficiencyCorrection
   PhysicsAnalysis/ElectronPhotonID/ElectronPhotonFourMomentumCorrection
   PhysicsAnalysis/ElectronPhotonID/ElectronPhotonShowerShapeFudgeTool
   PhysicsAnalysis/ElectronPhotonID/ElectronPhotonSelectorTools
   PhysicsAnalysis/ElectronPhotonID/IsolationCorrections
   PhysicsAnalysis/ElectronPhotonID/PhotonEfficiencyCorrection
   PhysicsAnalysis/ElectronPhotonID/EgammaAnalysisHelper
   PhysicsAnalysis/JetMissingEtID/JetSelectorTools
   PhysicsAnalysis/JetTagging/JetTagPerformanceCalibration/xAODBTaggingEfficiency
   PhysicsAnalysis/MuonID/MuonsIDAnalysis/MuonEfficiencyCorrections
   PhysicsAnalysis/MuonID/MuonsIDAnalysis/MuonMomentumCorrections
   PhysicsAnalysis/MuonID/MuonSelectorTools
   PhysicsAnalysis/TauID/TauAnalysisTools
   Reconstruction/Jet/JetCalibTools
   Reconstruction/Jet/JetJvtEfficiency
   Reconstruction/Jet/JetMomentTools
   Reconstruction/Jet/JetResolution
   Reconstruction/Jet/JetUncertainties
   Reconstruction/MET/METInterface
   Reconstruction/MET/METUtilities
   Tools/PathResolver
   Trigger/TrigAnalysis/TrigDecisionTool
   Trigger/TrigAnalysis/TriggerMatchingTool
   Trigger/TrigConfiguration/TrigConfxAOD )

# External(s) used by the package:
find_package( Boost COMPONENTS program_options )
find_package( ROOT COMPONENTS Core Hist RIO Gpad )
find_package( GTest )

# Libraries in the package:
if( XAOD_STANDALONE )
   atlas_add_root_dictionary( QuickAnaLib QuickAnaLibCintDict
      ROOT_HEADERS QuickAna/Configuration.h QuickAna/ELExample.h
      QuickAna/QuickAna.h QuickAna/ValidationEL.h QuickAna/xAODTruth.h
      Root/LinkDef.h
      EXTERNAL_PACKAGES ROOT )
endif()

set( extra_public_libs )
set( extra_private_libs )
if( XAOD_STANDALONE )
   set( extra_public_libs EventLoop )
   set( extra_private_libs xAODRootAccessInterfaces xAODRootAccess )
else()
   set( extra_public_libs )
   set( extra_private_libs AthAnalysisBaseCompsLib StoreGateLib )
endif()
atlas_add_library( QuickAnaLib
   QuickAna/*.h Root/*.cxx ${QuickAnaLibCintDict}
   PUBLIC_HEADERS QuickAna
   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES AthContainers AsgTools xAODEgamma xAODJet xAODMuon xAODTau
   xAODCore xAODMissingET xAODEventInfo xAODTruth AssociationUtilsLib
   IsolationSelectionLib PATInterfaces RootCoreUtils ${extra_public_libs}
   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES}
   CxxUtils EventPrimitives xAODBase xAODTracking PileupReweightingLib
   ElectronEfficiencyCorrectionLib ElectronPhotonFourMomentumCorrectionLib
   ElectronPhotonShowerShapeFudgeToolLib ElectronPhotonSelectorToolsLib EgammaAnalysisHelpersLib
   IsolationCorrectionsLib PhotonEfficiencyCorrectionLib JetSelectorToolsLib
   xAODBTaggingEfficiencyLib MuonEfficiencyCorrectionsLib
   MuonMomentumCorrectionsLib MuonSelectorToolsLib TauAnalysisToolsLib
   JetCalibToolsLib JetJvtEfficiencyLib JetMomentToolsLib JetResolutionLib
   JetUncertaintiesLib METInterface METUtilitiesLib PathResolver
   TrigDecisionToolLib TriggerMatchingToolLib TrigConfxAODLib
   ${extra_private_libs} )

if( NOT XAOD_STANDALONE )
   atlas_add_component( QuickAna
      src/*.h src/*.cxx src/components/*.cxx
      LINK_LIBRARIES GaudiKernel AthenaBaseComps xAODEgamma QuickAnaLib )
endif()

# Executable(s) in the package:
if( XAOD_STANDALONE )
   foreach( exec quickana_basic quickana_multi_wp quickana_event_data_filler quickana_output
         quickana_systematics quickana_test_or quickana_truth )
      atlas_add_executable( ${exec}
         util/${exec}.cxx
         INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
         LINK_LIBRARIES ${ROOT_LIBRARIES} PATInterfaces xAODRootAccess
         RootCoreUtils QuickAnaLib )
   endforeach()

   atlas_add_executable( quickana_trigger
      util/quickana_trigger.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} PATInterfaces
      xAODRootAccess AsgTools TrigConfxAODLib TrigDecisionToolLib
      QuickAnaLib )

   atlas_add_executable( quickana_validate
      util/quickana_validate.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} EventLoop
      PATInterfaces SampleHandler xAODRootAccess QuickAnaLib )
endif()

# Test(s) in the package:
atlas_add_test( gt_KinSelect
   SOURCES test/gt_KinSelect.cxx
   INCLUDE_DIRS ${GTEST_INCLUDE_DIRS}
   LINK_LIBRARIES ${GTEST_LIBRARIES} AsgTools xAODJet QuickAnaLib )

atlas_add_test( it_BaseTools_test
   SOURCES test/it_BaseTools_test.cxx
   INCLUDE_DIRS ${GTEST_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
   LINK_LIBRARIES ${GTEST_LIBRARIES} ${Boost_LIBRARIES} AsgTools
   AssociationUtilsLib ElectronEfficiencyCorrectionLib
   ElectronPhotonFourMomentumCorrectionLib ElectronPhotonSelectorToolsLib
   IsolationCorrectionsLib IsolationSelectionLib JetCalibToolsLib
   JetJvtEfficiencyLib JetMomentToolsLib JetResolutionLib JetSelectorToolsLib
   JetUncertaintiesLib METUtilitiesLib MuonEfficiencyCorrectionsLib
   MuonMomentumCorrectionsLib MuonSelectorToolsLib PhotonEfficiencyCorrectionLib
   PileupReweightingLib TauAnalysisToolsLib TrigConfxAODLib
   xAODBTaggingEfficiencyLib ElectronPhotonShowerShapeFudgeToolLib )

foreach( test inc_AnaToolCorrect inc_AnaToolSelect inc_AnaToolWeight
      inc_IAnaTool inc_IEventObjects inc_IQuickAna inc_MessageCheck
      inc_OutputToolXAOD inc_xAODInclude )
   atlas_add_test( ${test}
      SOURCES test/${test}.cxx
      LINK_LIBRARIES QuickAnaLib )
endforeach()

if( XAOD_STANDALONE )
   foreach( test ut_MessageCheck ut_OutputToolXAOD ut_xaod_truth )
      atlas_add_test( ${test}
         SOURCES test/${test}.cxx
         INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
         LINK_LIBRARIES ${ROOT_LIBRARIES} PATInterfaces xAODRootAccess CxxUtils
         xAODEventInfo xAODEgamma xAODMissingET xAODCore RootCoreUtils
         QuickAnaLib )
   endforeach()

   atlas_add_test( ut_event_data_filler SCRIPT test/ut_event_data_filler.sh )
   atlas_add_test( ut_eventloop_example SCRIPT test/ut_eventloop_example.sh )
   atlas_add_test( ut_python SCRIPT test/ut_python.py )
   atlas_add_test( ut_python_no_qa SCRIPT test/ut_python_no_qa.py )
   atlas_add_test( ut_quickana_basic SCRIPT test/ut_quickana_basic.sh )
   atlas_add_test( ut_quickana_hammercloud
      SCRIPT test/ut_quickana_hammercloud.sh )
   atlas_add_test( ut_quickana_multi_wp SCRIPT test/ut_quickana_multi_wp.sh )
   atlas_add_test( ut_quickana_optimized SCRIPT test/ut_quickana_optimized.sh )
   atlas_add_test( ut_quickana_output SCRIPT test/ut_quickana_output.sh )
   atlas_add_test( ut_quickana_trigger SCRIPT test/ut_quickana_trigger.sh )
   atlas_add_test( ut_quickana_validate SCRIPT test/ut_quickana_validate.sh )
   atlas_add_test( ut_quickana_validate_electron
      SCRIPT test/ut_quickana_validate_electron.sh )
   atlas_add_test( ut_quickana_validate_jet
      SCRIPT test/ut_quickana_validate_jet.sh )
   atlas_add_test( ut_quickana_validate_fat_jet
      SCRIPT test/ut_quickana_validate_fat_jet.sh )
   atlas_add_test( ut_quickana_validate_muon
      SCRIPT test/ut_quickana_validate_muon.sh )
   atlas_add_test( ut_quickana_validate_photon
      SCRIPT test/ut_quickana_validate_photon.sh )
   atlas_add_test( ut_quickana_validate_tau
      SCRIPT test/ut_quickana_validate_tau.sh )
endif()

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_data( share/*.conf )
atlas_install_scripts( scripts/* )
