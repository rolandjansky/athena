"""Configure Truth output for digitization with ComponentAccumulator style

Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.Enums import ProductionStep
from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg


def TruthDigitizationOutputCfg(flags):
    """Return ComponentAccumulator with Truth output items"""
    ItemList = [
        "McEventCollection#*",
        "TrackRecordCollection#*",
    ]

    prefix = ''
    if flags.Common.ProductionStep == ProductionStep.PileUpPresampling:
        prefix = flags.Overlay.BkgPrefix

    from Digitization.PileUpUtils import pileupInputCollections
    puCollections = pileupInputCollections(flags.Digitization.PU.LowPtMinBiasInputCols)
    if "AntiKt4TruthJets" in puCollections:
        ItemList += [
            f"xAOD::JetContainer#{prefix}InTimeAntiKt4TruthJets",
            f"xAOD::AuxContainerBase!#{prefix}InTimeAntiKt4TruthJetsAux.-constituentLinks.-constituentWeights",
            f"xAOD::JetContainer#{prefix}OutOfTimeAntiKt4TruthJets",
            f"xAOD::AuxContainerBase!#{prefix}OutOfTimeAntiKt4TruthJetsAux.-constituentLinks.-constituentWeights",
        ]
    if "AntiKt6TruthJets" in puCollections:
        ItemList += [
            f"xAOD::JetContainer#{prefix}InTimeAntiKt6TruthJets",
            f"xAOD::AuxContainerBase!#{prefix}InTimeAntiKt6TruthJetsAux.-constituentLinks.-constituentWeights",
            f"xAOD::JetContainer#{prefix}OutOfTimeAntiKt6TruthJets",
            f"xAOD::AuxContainerBase!#{prefix}OutOfTimeAntiKt6TruthJetsAux.-constituentLinks.-constituentWeights",
        ]
    if "TruthPileupParticles" in puCollections:
        ItemList += [
            f"xAOD::TruthParticleContainer#{prefix}TruthPileupParticles",
            f"xAOD::TruthParticleAuxContainer#{prefix}TruthPileupParticlesAux.",
        ]

    return OutputStreamCfg(flags, "RDO", ItemList)
