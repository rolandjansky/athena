# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

STDM9SmartContent = [
    "InDetTrackParticles",
    "PrimaryVertices", 
     "MET_Reference_AntiKt4EMTopo",
     "MET_Reference_AntiKt4EMPFlow",
     "AntiKt4EMTopoJets",
     "AntiKt4EMPFlowJets",
     "AntiKt6LCTopoJets"
]

STDM9ExtraVariables = [
    "CaloCalTopoClusters.calE.calEta.calPhi.calM.rawE.rawEta.rawPhi.rawM.altE.altEta.altM.altPhi"
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
     "TruthParticles",
     "MET_Truth",
     "AntiKt10TruthJets",
     "AntiKt4TruthJets",

     ### JETS ###
     "AntiKt10LCTopoJets",

     ### MET ###
     "MET_LocHadTopo",

     ### TOPOCLUSTERS ###
     "CaloCalTopoClusters",

     "METAssoc_AntiKt4EMTopo",
     "MET_Core_AntiKt4EMTopo",
     "MET_Reference_AntiKt4EMTopo",

     "METAssoc_AntiKt4EMPFlow",
     "MET_Core_AntiKt4EMPFlow",
     "MET_Reference_AntiKt4EMPFlow",
     "InDetForwardTrackParticles",
]

