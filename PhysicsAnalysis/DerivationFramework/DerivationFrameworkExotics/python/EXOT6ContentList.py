# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

EXOT6SmartContent = [
    "Electrons",
    "Muons",
    "Photons",
    "InDetTrackParticles",
    "InDetForwardTrackParticles",
    "CombinedMuonTrackParticles",
    "ExtrapolatedMuonTrackParticles",
    "GSFTrackParticles",
    "PrimaryVertices", 
]

EXOT6AllVariablesContent = [
     ### TRUTH ###
     "TruthVertices",
     "MuonTruthParticles",
     "TruthParticles",
     "MET_Truth",
     "AntiKt4TruthJets",

     ### JETS ###
     "AntiKt4LCTopoJets",
     
     ### MET ###
     "MET_LocHadTopo",
     
     ### MUONS ###
     "MuonSegments",

     ### TOPOCLUSTERS ###
#     "CaloCalTopoCluster",

]

EXOT6ExtraVariables = [ 
     "Electrons.author.Medium.Tight.Loose.charge", 
#     "AntiKt4LCTopoJets.AverageLArQF"
]

EXOT6UnslimmedContent = [
]
