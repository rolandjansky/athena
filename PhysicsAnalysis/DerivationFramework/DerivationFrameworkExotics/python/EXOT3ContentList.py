# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

EXOT3SmartContent = [
    "Electrons",
    "Muons",
    "Photons",
    "InDetTrackParticles",
    "PrimaryVertices", 
]

EXOT3ExtraVariables = [
    "Electrons.AverageLArQF",
    "Electrons.author.Medium.Tight.Loose.charge", 
    "Muons.etcone30"
]

EXOT3AllVariablesContent = [
     ### TRUTH ###
     "TruthEvents",
     "TruthVertices",
     "MuonTruthParticles",
     "TruthParticles",
     "MET_Truth",
     "CamKt12TruthJets",
     "AntiKt10TruthJets",
     "AntiKt4TruthJets",

     ### JETS ###
     "AntiKt10LCTopoJets",
     "AntiKt4LCTopoJets",
     "AntiKt4EMTopoJets",
     "CamKt12LCTopoJets",
     "AntiKt3PV0TrackJets",
     "AntiKt2PV0TrackJets",
     "TauJets",
     
     ### MET ###
     "MET_LocHadTopo",
     
     ### B JETS ###
     "BTagging_AntiKt4EMTopo",
     "BTagging_AntiKt3Track",
     "BTagging_AntiKt2Track",

     ### MUONS ###
     "MuonSegments",

     ### TOPOCLUSTERS ###
     "CaloCalTopoClusters",

     "METAssoc_AntiKt4EMTopo",
     "MET_Core_AntiKt4EMTopo",
     "MET_Reference_AntiKt4EMTopo",

     "METAssoc_AntiKt4LCTopo",
     "MET_Core_AntiKt4LCTopo",
     "MET_Reference_AntiKt4LCTopo",

     "egammaTopoSeededClusters",

     "InDetForwardTrackParticles",

     "CombinedMuonTrackParticles",
     "ExtrapolatedMuonTrackParticles",
     "GSFTrackParticles",
]

EXOT3StaticContent = [
    ### JETS ###
    "xAOD::JetContainer#CamKt12LCTopoBDRSFilteredMU100Y4Jets",
    "xAOD::JetContainer#CamKt12PV0TrackBDRSFilteredMU100Y4Jets",
    "xAOD::JetContainer#CamKt12TruthBDRSFilteredMU100Y4Jets",
    
    "xAOD::JetAuxContainer#CamKt12LCTopoBDRSFilteredMU100Y4JetsAux.",
    "xAOD::JetAuxContainer#CamKt12PV0TrackBDRSFilteredMU100Y4JetsAux.",
    "xAOD::JetAuxContainer#CamKt12TruthBDRSFilteredMU100Y4JetsAux.",
    
    "xAOD::JetContainer#AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
    "xAOD::JetContainer#AntiKt10PV0TrackTrimmedPtFrac5SmallR20Jets",
    "xAOD::JetContainer#AntiKt10TruthTrimmedPtFrac5SmallR20Jets",
    
    "xAOD::JetAuxContainer#AntiKt10LCTopoTrimmedPtFrac5SmallR20JetsAux.",
    "xAOD::JetAuxContainer#AntiKt10PV0TrackTrimmedPtFrac5SmallR20JetsAux.",
    "xAOD::JetAuxContainer#AntiKt10TruthTrimmedPtFrac5SmallR20JetsAux.",
]
