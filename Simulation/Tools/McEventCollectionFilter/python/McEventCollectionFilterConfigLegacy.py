# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr

def getDecorateTruthPileupParticles(name="DecorateTruthPileupParticles", **kwargs):
    kwargs.setdefault("InputParticleContainer", "TruthPileupParticles")
    kwargs.setdefault("OutputDecoration", "TruthPileupParticles.PVz")
    return CfgMgr.PileUpTruthDecoration(name, **kwargs)


def getDecoratePileupAntiKt4TruthJets(name="DecoratePileupAntiKt4TruthJets", **kwargs):
    kwargs.setdefault("InputParticleContainer", "AntiKt4TruthJets")
    kwargs.setdefault("OutputDecoration", "AntiKt4TruthJets.PVz")
    return CfgMgr.PileUpTruthDecoration(name, **kwargs)


def getDecoratePileupAntiKt6TruthJets(name="DecoratePileupAntiKt6TruthJets", **kwargs):
    kwargs.setdefault("InputParticleContainer", "AntiKt6TruthJets")
    kwargs.setdefault("OutputDecoration", "AntiKt6TruthJets.PVz")
    return CfgMgr.PileUpTruthDecoration(name, **kwargs)
