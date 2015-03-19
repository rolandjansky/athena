# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

EXOT3SmartContent = [
    "Electrons",
    "Muons",
    "MET_Reference_AntiKt4LCTopo",
    "InDetTrackParticles",
    "PrimaryVertices",
    "CaloCalTopoClusters"
]

EXOT3AllVariablesContent = [
     ### TRUTH ###
     "TruthVertices",
     "MuonTruthParticles",
     "TruthParticles",
     "MET_Truth",
     "CamKt12TruthJets",
     #"BTagging_AntiKt10Truth",
     "AntiKt10TruthJets",
     "AntiKt4TruthJets",

     ### JETS ###
     "AntiKt10LCTopoJets",
     "AntiKt4LCTopoJets",
     "AntiKt4EMTopoJets",
     "CamKt12LCTopoJets",
     
     ### MET ###
     "MET_LocHadTopo",
     
     ### B JETS ###
     "BTagging_AntiKt10LCTopo",
     
     ### MUONS ###
     "MuonSegments",
]

EXOT3UnslimmedContent = [
    ### JETS ###
    "xAOD::JetContainer#CamKt08LCTopoJets",
    "xAOD::JetContainer#CamKt10LCTopoJets",
    "xAOD::JetContainer#CamKt12LCTopoSplitFilteredMu100SmallR30YCut15Jets",
    "xAOD::JetContainer#AntiKt10LCTopoTrimmedPtFrac5SmallR30Jets",
    "xAOD::JetContainer#AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",

    "xAOD::JetAuxContainer#CamKt08LCTopoJetsAux.",
    "xAOD::JetAuxContainer#CamKt10LCTopoJetsAux.",
    "xAOD::JetAuxContainer#CamKt12LCTopoSplitFilteredMu100SmallR30YCut15JetsAux.",
    "xAOD::JetAuxContainer#AntiKt10LCTopoTrimmedPtFrac5SmallR30JetsAux.",
    "xAOD::JetAuxContainer#AntiKt10LCTopoTrimmedPtFrac5SmallR20JetsAux.",
]
