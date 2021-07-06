# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the name of this package:
atlas_subdir( TopCPTools )

# External depdendencies:
find_package( Boost )

# Build a library that other components can link against:
atlas_add_library( TopCPTools Root/*.cxx Root/*.h Root/*.icc
                   TopCPTools/*.h TopCPTools/*.icc TopCPTools/*/*.h
                   TopCPTools/*/*.icc 
                   PUBLIC_HEADERS TopCPTools
                   LINK_LIBRARIES AsgAnalysisInterfaces
                                  AsgTools
                                  AssociationUtilsLib
                                  BoostedJetTaggersLib
                                  EgammaAnalysisInterfacesLib
                                  FTagAnalysisInterfacesLib
                                  InDetTrackSelectionToolLib
                                  InDetTrackSystematicsToolsLib
                                  IsolationCorrectionsLib
                                  IsolationSelectionLib
                                  JetAnalysisInterfacesLib
                                  JetCPInterfaces
                                  JetCalibToolsLib
                                  JetInterface
                                  JetSelectorToolsLib
                                  METInterface
                                  MuonAnalysisInterfacesLib
                                  MuonMomentumCorrectionsLib
                                  PMGAnalysisInterfacesLib
                                  PMGToolsLib
                                  TauAnalysisToolsLib
                                  TopConfiguration
                                  TrigConfInterfaces
                                  TrigDecisionToolLib
                                  TriggerAnalysisInterfaces
                                  TrigGlobalEfficiencyCorrectionLib
                                  TrigTauMatchingLib
                                  TriggerMatchingToolLib
                                  xAODBTaggingEfficiencyLib
                   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES}
                                          ElectronEfficiencyCorrectionLib
                                          ElectronPhotonFourMomentumCorrectionLib
                                          ElectronPhotonSelectorToolsLib
                                          ElectronPhotonShowerShapeFudgeToolLib
                                          GoodRunsListsLib
                                          JetJvtEfficiencyLib
                                          JetMomentToolsLib
                                          JetUncertaintiesLib
                                          METUtilitiesLib
                                          MuonEfficiencyCorrectionsLib
                                          MuonSelectorToolsLib
                                          PATCoreAcceptLib
                                          PathResolver
                                          PhotonEfficiencyCorrectionLib
                                          PileupReweightingLib
                                          TopEvent
                                          TrigConfxAODLib
                                          xAODBase )
