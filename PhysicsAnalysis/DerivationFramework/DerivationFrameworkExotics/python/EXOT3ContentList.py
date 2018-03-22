# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

EXOT3SmartContent = [
    "Electrons",
    "Muons",
    "Photons",
    "InDetTrackParticles",
    "PrimaryVertices", 
    "MET_Reference_AntiKt4EMTopo",
    "AntiKt4EMTopoJets",
    "BTagging_AntiKt4EMTopo",
    "BTagging_AntiKt2Track",
    "TauJets"
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
     "AntiKt10TruthJets",
     "AntiKt4TruthJets",

     ### JETS ###
     "AntiKt10LCTopoJets",
     "AntiKt10TrackCaloClusterJets",
     "AntiKt4EMTopoJets",
     "AntiKt2PV0TrackJets",
     "TauJets",
     
     ### MET ###
     "MET_LocHadTopo",
     
     ### B JETS ###
     "BTagging_AntiKt4EMTopo",
     "BTagging_AntiKt2Track",

     ### MUONS ###
     "MuonSegments",

     ### TOPOCLUSTERS ###
     "CaloCalTopoClusters",
     
     ### TRACKCALOCLUSTERS ###
     "TrackCaloClustersCombinedAndNeutral",

     "METAssoc_AntiKt4EMTopo",
     "MET_Core_AntiKt4EMTopo",
     "MET_Reference_AntiKt4EMTopo",

     "egammaTopoSeededClusters",

     "CombinedMuonTrackParticles",
     "ExtrapolatedMuonTrackParticles",
     "GSFTrackParticles",
]

EXOT3StaticContent = []
