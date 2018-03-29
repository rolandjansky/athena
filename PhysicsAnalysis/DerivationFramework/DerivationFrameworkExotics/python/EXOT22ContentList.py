# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

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
    "BTagging_AntiKt4EMTopo",
    "AntiKt4EMTopoJets",
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
    "AntiKt4TruthJets.PartonTruthLabelID.GhostWBosons.GhostWBosonsCount.GhostWBosonsPt.GhostZBosons.GhostZBosonsCount.GhostZBosonsPt",
    "AntiKt4TruthWZJets.PartonTruthLabelID.GhostWBosons.GhostWBosonsCount.GhostWBosonsPt.GhostZBosons.GhostZBosonsCount.GhostZBosonsPt"
]
