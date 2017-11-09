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
    "MET_Reference_AntiKt4EMTopo",
    "PrimaryVertices",
    'MET_Reference_AntiKt4EMTopo',
    'AntiKt4EMTopoJets',
    "AntiKt4LCTopoJets",
    "BTagging_AntiKt4EMTopo",
    "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets",
    "BTagging_AntiKt2Track"
]
    
EXOT7ExtraVariables = [
    "Electrons.author.Medium.Tight.Loose.charge",
    "Muons.charge",
    "AntiKt10TruthJets.pt.eta.phi.m",
    "AntiKt2PV0TrackJets.pt.eta.phi.m.constituentLinks.btaggingLink.GhostBHadronsFinal.GhostBHadronsInitial.GhostBQuarksFinal.GhostCHadronsFinal.GhostCHadronsInitial.GhostCQuarksFinal.GhostHBosons.GhostPartons.GhostTQuarksFinal.GhostTausFinal.GhostWBosons.GhostZBosons.GhostTruth.OriginVertex.GhostAntiKt3TrackJet.GhostAntiKt4TrackJet.GhostTrack.GhostTruthAssociationLink",
    "AntiKt4PV0TrackJets.pt.eta.phi.m.constituentLinks.btaggingLink.GhostBHadronsFinal.GhostBHadronsInitial.GhostBQuarksFinal.GhostCHadronsFinal.GhostCHadronsInitial.GhostCQuarksFinal.GhostHBosons.GhostPartons.GhostTQuarksFinal.GhostTausFinal.GhostWBosons.GhostZBosons.GhostTruth.OriginVertex.GhostTrack.GhostTruthAssociationLink",
    "CaloCalTopoClusters.calE.calEta.calPhi.calM.rawM.rawE.rawEta.rawPhi.e_sampl.eta_sampl.etaCalo.phiCalo",
    "AntiKt4TruthJets.pt.eta.phi.m",
    "AntiKt4TruthWZJets.pt.eta.phi.m",
    "AntiKt10LCTopoTrimmedPtFrac5SmallR20Jets.constituentLinks.Qw.GhostBHadronsFinalCount",
    "AntiKt10TruthTrimmedPtFrac5SmallR20Jets.pt.eta.phi.m.GhostBHadronsFinalCount"
]

EXOT7AllVariables = [
    "TruthParticles",
    "TruthEvents",
    "TruthVertices"
]
