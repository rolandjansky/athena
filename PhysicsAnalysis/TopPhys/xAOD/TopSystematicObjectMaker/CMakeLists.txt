# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the name of this package.
atlas_subdir( TopSystematicObjectMaker )

# Component(s) in the package.
atlas_add_library( TopSystematicObjectMaker
   TopSystematicObjectMaker/*.h Root/*.cxx
   PUBLIC_HEADERS TopSystematicObjectMaker
   LINK_LIBRARIES AsgTools PATInterfaces EgammaAnalysisInterfacesLib
   FTagAnalysisInterfacesLib JetAnalysisInterfacesLib MuonAnalysisInterfacesLib
   IsolationSelectionLib IsolationCorrectionsLib JetCalibToolsLib ParticleJetToolsLib
   JetCPInterfaces JetInterface METInterface xAODEgamma xAODJet xAODTracking
   TopJetSubstructure MuonMomentumCorrectionsLib TauAnalysisToolsLib
   PRIVATE_LINK_LIBRARIES AthContainers xAODBTagging xAODBase xAODCore xAODEventInfo
   xAODMissingET xAODMuon xAODTau TopConfiguration TopEvent
   InDetTrackSystematicsToolsLib METUtilitiesLib )
