"""Configure Truth output for digitization with ComponentAccumulator style

Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
"""
from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg


def TruthDigitizationOutputCfg(flags):
    """Return ComponentAccumulator with Truth output items"""
    ItemList = [
        "McEventCollection#*",
        "TrackRecordCollection#*",
    ]

    from Digitization.PileUpUtils import pileupInputCollections
    puCollections = pileupInputCollections(flags.Digitization.PU.LowPtMinBiasInputCols)
    if "AntiKt4TruthJets" in puCollections:
        ItemList += [
            "xAOD::JetContainer#InTimeAntiKt4TruthJets",
            "xAOD::AuxContainerBase!#InTimeAntiKt4TruthJetsAux.-constituentLinks.-constituentWeights",
            "xAOD::JetContainer#OutOfTimeAntiKt4TruthJets",
            "xAOD::AuxContainerBase!#OutOfTimeAntiKt4TruthJetsAux.-constituentLinks.-constituentWeights",
        ]
    if "AntiKt6TruthJets" in puCollections:
        ItemList += [
            "xAOD::JetContainer#InTimeAntiKt6TruthJets",
            "xAOD::AuxContainerBase!#InTimeAntiKt6TruthJetsAux.-constituentLinks.-constituentWeights",
            "xAOD::JetContainer#OutOfTimeAntiKt6TruthJets",
            "xAOD::AuxContainerBase!#OutOfTimeAntiKt6TruthJetsAux.-constituentLinks.-constituentWeights",
        ]
    if "TruthPileupParticles" in puCollections:
        ItemList += [
            "xAOD::TruthParticleContainer#TruthPileupParticles",
            "xAOD::TruthParticleAuxContainer#TruthPileupParticlesAux.",
        ]

    return OutputStreamCfg(flags, "RDO", ItemList)
