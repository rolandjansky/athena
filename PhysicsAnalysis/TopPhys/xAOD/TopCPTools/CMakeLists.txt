# Declare the name of this package:
atlas_subdir( TopCPTools )

# This package depends on other packages:
atlas_depends_on_subdirs( PUBLIC
                          xAODRootAccess
                          AsgTools
                          AthContainers
                          PATCore
                          PATInterfaces
                          TrigBunchCrossingTool
                          TrigConfInterfaces
                          TrigConfxAOD
                          TrigDecisionTool
                          TriggerMatchingTool
                          TrigTauMatching
                          GoodRunsLists
			  EgammaAnalysisInterfaces
			  ElectronPhotonFourMomentumCorrection
                          ElectronPhotonSelectorTools
                          ElectronEfficiencyCorrection
                          ElectronPhotonShowerShapeFudgeTool
                          PhotonEfficiencyCorrection
                          MuonMomentumCorrections
                          MuonSelectorTools
                          MuonEfficiencyCorrections
                          TauAnalysisTools
                          CalibrationDataInterface
                          xAODBTaggingEfficiency
                          JetCalibTools
                          JetCPInterfaces
                          JetUncertainties
                          JetInterface
                          JetMomentTools
                          JetSelectorTools
                          METInterface
                          METUtilities
                          IsolationSelection
                          IsolationCorrections
                          PathResolver
                          TopConfiguration
                          TopEvent
                          PileupReweighting
                          AssociationUtils
                          JetJvtEfficiency
                          PMGTools
			  InDetTrackSelectionTool
                          InDetTrackSystematicsTools
                          #BoostedJetTaggers 
			  FTagAnalysisInterfaces 
			  MuonAnalysisInterfaces 
        #TriggerAnalysisInterfaces
			  #TrigGlobalEfficiencyCorrection
			  PMGAnalysisInterfaces
			  JetAnalysisInterfaces)

# This package uses ROOT:
find_package( ROOT REQUIRED COMPONENTS Core Gpad Tree Hist RIO MathCore Graf )

# Build a library that other components can link against:
atlas_add_library( TopCPTools Root/*.cxx Root/*.h Root/*.icc
                   TopCPTools/*.h TopCPTools/*.icc TopCPTools/*/*.h
                   TopCPTools/*/*.icc 
                   PUBLIC_HEADERS TopCPTools
                   LINK_LIBRARIES xAODRootAccess
                                  AsgTools
                                  AthContainers                                  
				  PATCoreLib			  
                                  PATInterfaces
                                  TrigBunchCrossingTool
                                  TrigConfInterfaces
                                  TrigConfxAODLib
                                  TrigDecisionToolLib
                                  TriggerMatchingToolLib
                                  TrigTauMatchingLib
                                  GoodRunsListsLib
                                  EgammaAnalysisInterfacesLib
				  ElectronPhotonFourMomentumCorrectionLib
                                  ElectronPhotonSelectorToolsLib
                                  ElectronEfficiencyCorrectionLib
                                  ElectronPhotonShowerShapeFudgeToolLib
                                  PhotonEfficiencyCorrectionLib
                                  MuonMomentumCorrectionsLib
                                  MuonSelectorToolsLib
                                  MuonEfficiencyCorrectionsLib
                                  TauAnalysisToolsLib
                                  CalibrationDataInterfaceLib
                                  xAODBTaggingEfficiencyLib
                                  JetCalibToolsLib
                                  JetCPInterfaces
                                  JetUncertaintiesLib
                                  JetInterface
                                  JetMomentToolsLib
                                  JetSelectorToolsLib
                                  METInterface
                                  METUtilitiesLib
                                  IsolationSelectionLib
                                  IsolationCorrectionsLib
                                  PathResolver
                                  TopConfiguration
                                  TopEvent
                                  PileupReweightingLib
                                  AssociationUtilsLib
                                  JetJvtEfficiencyLib
                                  PMGToolsLib
				  InDetTrackSelectionToolLib
                                  InDetTrackSystematicsToolsLib
                                  #BoostedJetTaggersLib
				  FTagAnalysisInterfacesLib
				  MuonAnalysisInterfacesLib
				  #TriggerAnalysisInterfacesLib
				  #TrigGlobalEfficiencyCorrectionLib
				  PMGAnalysisInterfacesLib
				  JetAnalysisInterfacesLib
                                  ${ROOT_LIBRARIES}
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} )

