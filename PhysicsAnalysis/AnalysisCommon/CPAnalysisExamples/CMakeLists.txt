# $Id: CMakeLists.txt 782717 2016-11-07 12:21:17Z krasznaa $

# The name of the package:
atlas_subdir( CPAnalysisExamples )

# Extra dependencies, based on the build environment:
set( extra_deps )
set( extra_libs )
if( XAOD_STANDALONE )
   set( extra_deps Control/xAODRootAccess PhysicsAnalysis/D3PDTools/EventLoop
      PRIVATE )
   set( extra_libs xAODRootAccess EventLoop )
else()
   set( extra_deps Control/AthenaBaseComps Control/AthAnalysisBaseComps
      PRIVATE PhysicsAnalysis/POOLRootAccess GaudiKernel )
   set( extra_libs AthAnalysisBaseCompsLib )
endif()

# The dependencies of the package:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthToolSupport/AsgTools
   Event/xAOD/xAODMuon
   Event/xAOD/xAODJet
   Event/xAOD/xAODBTagging
   Event/xAOD/xAODCore
   Event/xAOD/xAODEgamma
   Event/xAOD/xAODMissingET
   Event/xAOD/xAODPrimitives
   Event/xAOD/xAODTau
   Event/xAOD/xAODTracking
   Event/xAOD/xAODTruth
   Event/xAOD/xAODEventInfo
   PhysicsAnalysis/AnalysisCommon/AssociationUtils
   PhysicsAnalysis/AnalysisCommon/IsolationSelection
   PhysicsAnalysis/AnalysisCommon/PATCore
   PhysicsAnalysis/AnalysisCommon/PATInterfaces
   PhysicsAnalysis/AnalysisCommon/PileupReweighting
   PhysicsAnalysis/Interfaces/AsgAnalysisInterfaces
   PhysicsAnalysis/ElectronPhotonID/ElectronEfficiencyCorrection
   PhysicsAnalysis/ElectronPhotonID/ElectronPhotonFourMomentumCorrection
   PhysicsAnalysis/ElectronPhotonID/IsolationCorrections
   PhysicsAnalysis/JetMissingEtID/JetSelectorTools
   PhysicsAnalysis/JetTagging/JetTagPerformanceCalibration/xAODBTaggingEfficiency
   PhysicsAnalysis/MuonID/MuonIDAnalysis/MuonEfficiencyCorrections
   PhysicsAnalysis/MuonID/MuonIDAnalysis/MuonMomentumCorrections
   PhysicsAnalysis/MuonID/MuonSelectorTools
   PhysicsAnalysis/TauID/TauAnalysisTools
   Reconstruction/Jet/JetUncertainties
   Reconstruction/Jet/JetCalibTools
   Reconstruction/Jet/JetInterface
   Reconstruction/MET/METInterface
   Reconstruction/MET/METUtilities
   Trigger/TrigAnalysis/TrigDecisionTool
   Trigger/TrigAnalysis/TriggerMatchingTool
   ${extra_deps}
   Event/xAOD/xAODBase
   Event/xAOD/xAODMetaData )

# External(s) used by the package:
find_package( ROOT COMPONENTS Core Hist RIO MathCore )
find_package( Boost )

# Libraries in the package:
if( XAOD_STANDALONE )
   atlas_add_root_dictionary( CPAnalysisExamplesLib
      CPAnalysisExamplesLibCintSource
      ROOT_HEADERS CPAnalysisExamples/xExample.h Root/LinkDef.h
      EXTERNAL_PACKAGES ROOT )
endif()

atlas_add_library( CPAnalysisExamplesLib
   CPAnalysisExamples/*.h Root/*.cxx ${CPAnalysisExamplesLibCintSource}
   PUBLIC_HEADERS CPAnalysisExamples
   INCLUDE_DIRS ${Boost_INCLUDE_DIRS} ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${Boost_LIBRARIES} ${ROOT_LIBRARIES} AsgTools 
   xAODMuon xAODJet xAODBTagging xAODCore xAODEgamma xAODMissingET
   xAODPrimitives xAODTau xAODTracking xAODTruth xAODEventInfo
   AssociationUtilsLib IsolationSelectionLib PATCoreLib PATInterfaces
   PileupReweightingLib ElectronEfficiencyCorrectionLib
   ElectronPhotonFourMomentumCorrectionLib IsolationCorrectionsLib
   JetSelectorToolsLib xAODBTaggingEfficiencyLib MuonEfficiencyCorrectionsLib
   MuonMomentumCorrectionsLib MuonSelectorToolsLib TauAnalysisToolsLib
   JetUncertaintiesLib JetCalibToolsLib JetInterface
   METInterface METUtilitiesLib TrigDecisionToolLib
   TriggerMatchingToolLib ${extra_libs}
   PRIVATE_LINK_LIBRARIES xAODBase xAODMetaData )

if( NOT XAOD_STANDALONE )
   atlas_add_component( CPAnalysisExamples
      src/*.h src/*.cxx src/components/*.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} AthAnalysisBaseCompsLib
      ElectronPhotonFourMomentumCorrectionLib xAODEgamma xAODCore xAODBase
      AsgTools xAODTracking xAODMuon AthenaBaseComps AsgTools GaudiKernel
      AthAnalysisBaseCompsLib CPAnalysisExamplesLib )
endif()

atlas_add_dictionary( CPAnalysisExamplesDict
   CPAnalysisExamples/CPAnalysisExamplesDict.h
   CPAnalysisExamples/selection.xml
   LINK_LIBRARIES CPAnalysisExamplesLib )

# Executable(s) in the package:
if( XAOD_STANDALONE )
   atlas_add_executable( CPSystematicExample
      util/CPSystematicExample.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} ${Boost_LIBRARIES} xAODRootAccess
      xAODEventInfo xAODJet xAODCore PATInterfaces CPAnalysisExamplesLib )

   atlas_add_executable( CPToolTester
      util/CPToolTester.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} xAODRootAccess xAODEventInfo xAODMuon
      PATInterfaces CPAnalysisExamplesLib )

   atlas_add_executable( runExample
      util/runExample.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} xAODRootAccess SampleHandler EventLoop
      CPAnalysisExamplesLib )

   atlas_add_executable( run_ut_accessDataAndCpTools
      util/run_ut_accessDataAndCpTools.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} xAODRootAccess xAODEventInfo xAODJet
      xAODCore xAODEgamma xAODMuon xAODTau CPAnalysisExamplesLib )

   atlas_add_executable( run_ut_accessDataAndCpTools_v20
      util/run_ut_accessDataAndCpTools_v20.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} xAODRootAccess xAODEventInfo xAODJet
      xAODCore xAODEgamma xAODMuon xAODTau CPAnalysisExamplesLib )
else()
   atlas_add_executable( checkxAODTrigger
      util/checkxAODTrigger.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} AthAnalysisBaseCompsLib POOLRootAccess
      GaudiKernel AsgTools TrigDecisionToolLib )
endif()

# Test(s) in the package:
if( XAOD_STANDALONE )
   atlas_add_test( ut_accessData
      SOURCES test/ut_accessData.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} xAODRootAccess xAODJet
      CPAnalysisExamplesLib )

   atlas_add_test( ut_accessDataAndCPTools_8TeV
      SCRIPT test/ut_accessDataAndCPTools_8TeV.sh )
   atlas_add_test( ut_accessDataAndCPTools_13TeV
      SCRIPT test/ut_accessDataAndCPTools_13TeV.sh )
else()
   atlas_add_test( ut_ath_EgammaCalibrationAndSmearingTool_test
      SOURCES test/ut_ath_EgammaCalibrationAndSmearingTool_test.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} AthAnalysisBaseCompsLib POOLRootAccess
      GaudiKernel AsgTools xAODEgamma xAODCore
      ElectronPhotonFourMomentumCorrectionLib )

   atlas_add_test( ut_ath_checkTrigger_test
      SOURCES test/ut_ath_checkTrigger_test.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      PROPERTIES TIMEOUT 300
      LINK_LIBRARIES ${ROOT_LIBRARIES} AthAnalysisBaseCompsLib POOLRootAccess
      GaudiKernel AsgTools TrigDecisionToolLib )
endif()

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_data( data/* )
