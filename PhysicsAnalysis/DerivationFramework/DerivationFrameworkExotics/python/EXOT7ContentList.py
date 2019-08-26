# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

EXOT7Content = [
    'xAOD::TruthParticleContainer#TruthElectrons',
    'xAOD::TruthParticleAuxContainer#TruthElectronsAux.',
    'xAOD::TruthParticleContainer#TruthMuons',
    'xAOD::TruthParticleAuxContainer#TruthMuonsAux.',
]

EXOT7SmartCollections = [
    "Electrons",
    "Muons",
    "PrimaryVertices",
    'MET_Reference_AntiKt4EMTopo',
    'MET_Reference_AntiKt4EMPFlow',
    'AntiKt4TruthJets',
    'AntiKt4TruthWZJets',
    'AntiKt4EMTopoJets',
    'AntiKt4EMTopoJets_BTagging201810',
    'AntiKt4EMPFlowJets', 
    'AntiKt4EMPFlowJets_BTagging201810', 
    'AntiKt4EMPFlowJets_BTagging201903', 
    "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
    "AntiKt10LCTopoCSSKSoftDropBeta100Zcut10Jets",
    "BTagging_AntiKt4EMTopo_201810",
    "BTagging_AntiKt4EMPFlow_201810",
    "BTagging_AntiKt4EMPFlow_201903",
    "BTagging_AntiKtVR30Rmax4Rmin02Track",
]
    
EXOT7ExtraVariables = [
    "Electrons.author.Medium.Tight.Loose.charge",
    "Muons.charge",
    "AntiKt10TruthJets.pt.eta.phi.m",
    "CaloCalTopoClusters.calE.calEta.calPhi.calM.rawM.rawE.rawEta.rawPhi.e_sampl.eta_sampl.etaCalo.phiCalo",
    "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.constituentLinks.Qw.GhostBHadronsFinalCount",
    "AntiKt10TruthTrimmedPtFrac5SmallR20Jets.pt.eta.phi.m.GhostBHadronsFinalCount.GhostCHadronsFinalCount"
]

EXOT7AllVariables = [
    "AntiKt2PV0TrackJets",
    "AntiKtVR30Rmax4Rmin02TrackJets",
    "TruthParticles",
    "TruthEvents",
    "TruthVertices"
]
