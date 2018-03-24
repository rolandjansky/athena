# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

EXOT3SmartContent = [
    "Electrons",
    "Muons",
    "Photons",
    "InDetTrackParticles",
    "PrimaryVertices", 
    "AntiKt4TruthJets",
    "AntiKt4EMTopoJets",
    "BTagging_AntiKt4EMTopo",
    "BTagging_AntiKt2Track",
    "MET_Reference_AntiKt4EMTopo",
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

     ### JETS ###
     "AntiKt10LCTopoJets",
     "AntiKt10TrackCaloClusterJets",
     "AntiKt2PV0TrackJets",
     
     ### MET ###
     "MET_LocHadTopo",
     "METAssoc_AntiKt4EMTopo",
     "MET_Core_AntiKt4EMTopo",
     
     ### MUONS ###
     "MuonSegments",

     ### TOPOCLUSTERS ###
     "CaloCalTopoClusters",
     
     ### TRACKCALOCLUSTERS ###
     "TrackCaloClustersCombinedAndNeutral",

     ### CLUSTERS ###
     "egammaTopoSeededClusters",

     ### TRACKPARTICLES ###
     "CombinedMuonTrackParticles",
     "ExtrapolatedMuonTrackParticles",
     "GSFTrackParticles"
]

EXOT3StaticContent = []
