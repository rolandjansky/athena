# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the name of this package.
atlas_subdir( TopObjectSelectionTools )

# External(s).
find_package( Boost )

# Component(s) in the package.
atlas_add_library( TopObjectSelectionTools TopObjectSelectionTools/*.h Root/*.cxx
                   PUBLIC_HEADERS TopObjectSelectionTools
                   LINK_LIBRARIES AsgMessagingLib
                                  AsgTools
                                  xAODRootAccess
                                  xAODBase
                                  xAODCore
                                  xAODEgamma
                                  xAODJet
                                  xAODMuon
                                  xAODPrimitives
                                  xAODTau
                                  xAODTracking
                                  xAODTruth
                                  AsgAnalysisInterfaces
                                  FTagAnalysisInterfacesLib
                                  MuonAnalysisInterfacesLib
                                  TriggerAnalysisInterfaces
                                  TrigGlobalEfficiencyCorrectionLib
                                  JetAnalysisInterfacesLib
                                  AssociationUtilsLib
                                  TrigConfInterfaces
                                  TrigDecisionToolLib
                                  TriggerMatchingToolLib
                                  TrigTauMatchingLib
                                  InDetTrackSelectionToolLib
                                  ElectronPhotonSelectorToolsLib
                                  TauAnalysisToolsLib
                                  TopEvent
                                  TopConfiguration
                                  TopSystematicObjectMaker
                   PRIVATE_INCLUDE_DIRS ${Boost_INCLUDE_DIRS}
                   PRIVATE_LINK_LIBRARIES ${Boost_LIBRARIES}
                                          xAODEventInfo
                                          xAODMissingET
                                          AthContainers
                                          PATInterfaces
                                          FourMomUtils
                                          TopParticleLevel
                                          TopDataPreparation )
