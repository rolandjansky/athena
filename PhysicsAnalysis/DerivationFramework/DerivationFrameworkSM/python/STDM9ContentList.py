# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

STDM9SmartContent = [
    "Electrons",
    "Muons",
    "Photons",
    "InDetTrackParticles",
    "PrimaryVertices", 
     "MET_Reference_AntiKt4EMTopo",
     "MET_Reference_AntiKt4LCTopo",
     "AntiKt4LCTopoJets",
     "AntiKt4EMTopoJets",
]

STDM9ExtraVariables = [
    "Electrons.AverageLArQF",
    "Electrons.author.Medium.Tight.Loose.charge", 
    "Muons.etcone30"
]

# These are only added if running on MC
STDM9ExtraVariablesTruth = [".".join(["Electrons", 
    "truthOrigin", 
    "truthType", 
    "truthParticleLink", 
    "truthPdgId", 
    "bkgTruthType", 
    "bkgTruthOrigin", 
    "bkgTruthParticleLink", 
    "bkgMotherPdgId", 
    "firstEgMotherTruthType", 
    "firstEgMotherTruthOrigin", 
    "firstEgMotherTruthParticleLink", 
    "firstEgMotherPdgId"
    ]),
    "InDetTrackParticles.truthMatchProbability"]

STDM9AllVariablesContent = [
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
     "AntiKt4LCTopoJets",
     "AntiKt4EMTopoJets",
     "AntiKt2PV0TrackJets",
     
     ### MET ###
     "MET_LocHadTopo",
     
     ### B JETS ###
     "BTagging_AntiKt4EMTopo",
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

STDM9StaticContent = [
"xAOD::JetContainer#AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
"xAOD::JetAuxContainer#AntiKt10LCTopoTrimmedPtFrac5SmallR20JetsAux.",
"xAOD::JetContainer#AntiKt10TruthTrimmedPtFrac5SmallR20Jets",
"xAOD::JetAuxContainer#AntiKt10TruthTrimmedPtFrac5SmallR20JetsAux."
]
