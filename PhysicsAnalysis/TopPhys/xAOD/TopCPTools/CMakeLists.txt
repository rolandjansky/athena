# Auto-generated on: 2017-03-08 14:47:34.346308

# Declare the name of this package:
atlas_subdir( TopCPTools None )

# This package depends on other packages:
atlas_depends_on_subdirs( PUBLIC
                          xAODRootAccess
                          AsgTools
                          AthContainers
                          PATCore
                          PATInterfaces
                          TrigConfInterfaces
                          TrigConfxAOD
                          TrigDecisionTool
                          TriggerMatchingTool
                          TrigTauMatching
                          GoodRunsLists
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
                          JetResolution
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
                          InDetTrackSystematicsTools )

# This package uses ROOT:
find_package( ROOT REQUIRED COMPONENTS Core Gpad Tree Hist RIO MathCore Graf )

# Custom definitions needed for this package:
add_definitions( -g -std=c++14 )

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
                                  TrigConfInterfaces
                                  TrigConfxAODLib
                                  TrigDecisionToolLib
                                  TriggerMatchingToolLib
                                  TrigTauMatchingLib
                                  GoodRunsListsLib
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
                                  JetResolutionLib
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
                                  InDetTrackSystematicsToolsLib
                                  ${ROOT_LIBRARIES}
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} )

# Install data files from the package:
atlas_install_data( share/* )

