"""Configure Truth output for digitization with ComponentAccumulator style

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
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
            "xAOD::JetContainer#*InTimeAntiKt4TruthJets",
            "xAOD::JetAuxContainer#*InTimeAntiKt4TruthJetsAux.",
            "xAOD::JetContainer#*OutOfTimeAntiKt4TruthJets",
            "xAOD::JetAuxContainer#*OutOfTimeAntiKt4TruthJetsAux.",
        ]
    if "AntiKt6TruthJets" in puCollections:
        ItemList += [
            "xAOD::JetContainer#*InTimeAntiKt6TruthJets",
            "xAOD::JetAuxContainer#*InTimeAntiKt6TruthJetsAux.",
            "xAOD::JetContainer#*OutOfTimeAntiKt6TruthJets",
            "xAOD::JetAuxContainer#*OutOfTimeAntiKt6TruthJetsAux.",
        ]
    if "TruthPileupParticles" in puCollections:
        ItemList += [
            "xAOD::TruthParticleContainer#*TruthPileupParticles",
            "xAOD::TruthParticleAuxContainer#*TruthPileupParticlesAux.",
        ]

    return OutputStreamCfg(flags, "RDO", ItemList)
