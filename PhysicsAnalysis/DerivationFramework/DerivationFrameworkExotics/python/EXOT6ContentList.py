# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

EXOT6SmartContent = [
    "Electrons",
    "Muons",
    "Photons",
    "InDetTrackParticles",
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

     "AntiKt4EMTopoJets",
     "AntiKt4LCTopoJets",
     "TauJets",

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

EXOT6ExtraVariables = [ 
     "Electrons.author.Medium.Tight.Loose.charge", 
     "egammaClusters.time",
#     "AntiKt4LCTopoJets.AverageLArQF"
     "TruthEvents.PDGID1.PDGID2.PDFID1.PDFID2.X1.X2.Q.XF1.XF2"
]

EXOT6UnslimmedContent = [
]
