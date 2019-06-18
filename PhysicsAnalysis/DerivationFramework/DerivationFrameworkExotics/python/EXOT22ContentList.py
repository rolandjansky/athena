# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

EXOT22Content = []

EXOT22AllVariables = [
    "MET_Track"
]

EXOT22AllVariablesTruth = [
    "TruthParticles",
    "TruthEvents",
    "TruthVertices",
    "MET_Truth",
    "MuonTruthParticles",
    "egammaTruthParticles"
]

EXOT22SmartCollections = [
    "Photons",
    "TauJets",
    "Electrons",
    "Muons",
    "MET_Reference_AntiKt4EMTopo",
    "MET_Reference_AntiKt4EMPFlow",
    "BTagging_AntiKt4EMTopo",
    "BTagging_AntiKt4EMPFlow",
    "AntiKt4EMTopoJets",
    "AntiKt4EMPFlowJets",
    "PrimaryVertices",
    "InDetTrackParticles"
]

EXOT22SmartCollectionsTruth = [
    "AntiKt4TruthJets",
    "AntiKt4TruthWZJets",
]

EXOT22ExtraVariables = []

EXOT22ExtraVariablesTruth = [
    "Electrons.bkgTruthType.bkgTruthOrigin.bkgMotherPdgId.bkgTruthParticleLink.firstEgMotherTruthType.firstEgMotherTruthOrigin.firstEgMotherPdgId.firstEgMotherTruthParticleLink",
    "TauJets.IsTruthMatched.truthOrigin.truthType.truthParticleLink.truthJetLink.DFCommonTausLoose",
    "AntiKt4TruthJets.PartonTruthLabelID.GhostWBosons.GhostWBosonsCount.GhostWBosonsPt.GhostZBosons.GhostZBosonsCount.GhostZBosonsPt",
    "AntiKt4TruthWZJets.PartonTruthLabelID.GhostWBosons.GhostWBosonsCount.GhostWBosonsPt.GhostZBosons.GhostZBosonsCount.GhostZBosonsPt"
]
