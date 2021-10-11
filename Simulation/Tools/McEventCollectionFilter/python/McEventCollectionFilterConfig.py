# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# utilities
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from SGComps.AddressRemappingConfig import InputRenameCfg
AthSequencer=CompFactory.AthSequencer


def McEventCollectionFilterCfg(flags, **kwargs):
    acc = ComponentAccumulator()

    acc.merge(InputRenameCfg("McEventCollection", "TruthEvent", "TruthEventOLD"))

    if flags.Detector.EnableTRT:
        kwargs.setdefault("InputTRTHits", "TRTUncompressedHitsOLD")
    else:
        kwargs.setdefault("InputTRTHits", "")

    kwargs.setdefault("KeepElectronsLinkedToTRTHits", False)

    acc.addEventAlgo(CompFactory.McEventCollectionFilter(name="McEventCollectionFilter", **kwargs))
    return acc


def BCM_HitsTruthRelinkCfg(flags, **kwargs):
    acc = ComponentAccumulator()

    acc.merge(InputRenameCfg("SiHitCollection", "BCMHits", "BCMHitsOLD"))

    kwargs.setdefault("InputHits", "BCMHitsOLD")
    kwargs.setdefault("OutputHits", "BCMHits")

    acc.addEventAlgo(CompFactory.SiliconHitsTruthRelink(name="BCM_HitsTruthRelink", **kwargs))
    return acc


def PixelHitsTruthRelinkCfg(flags, **kwargs):
    acc = ComponentAccumulator()

    acc.merge(InputRenameCfg("SiHitCollection", "PixelHits", "PixelHitsOLD"))

    kwargs.setdefault("InputHits", "PixelHitsOLD")
    kwargs.setdefault("OutputHits", "PixelHits")

    acc.addEventAlgo(CompFactory.SiliconHitsTruthRelink(name="PixelHitsTruthRelink", **kwargs))
    return acc


def SCT_HitsTruthRelinkCfg(flags, **kwargs):
    acc = ComponentAccumulator()

    acc.merge(InputRenameCfg("SiHitCollection", "SCT_Hits", "SCT_HitsOLD"))

    kwargs.setdefault("InputHits", "SCT_HitsOLD")
    kwargs.setdefault("OutputHits", "SCT_Hits")

    acc.addEventAlgo(CompFactory.SiliconHitsTruthRelink(name="SCT_HitsTruthRelink", **kwargs))
    return acc


def TRT_HitsTruthRelinkCfg(flags, **kwargs):
    acc = ComponentAccumulator()

    acc.merge(InputRenameCfg("TRTUncompressedHitCollection", "TRTUncompressedHits", "TRTUncompressedHitsOLD"))

    kwargs.setdefault("InputHits", "TRTUncompressedHitsOLD")
    kwargs.setdefault("OutputHits", "TRTUncompressedHits")

    kwargs.setdefault("KeepElectronsLinkedToTRTHits", False)

    acc.addEventAlgo(CompFactory.TRT_HitsTruthRelink(name="TRT_HitsTruthRelink", **kwargs))
    return acc



def ITkPixelHitsTruthRelinkCfg(flags, **kwargs):
    acc = ComponentAccumulator()

    acc.merge(InputRenameCfg("SiHitCollection", "ITkPixelHits", "ITkPixelHitsOLD"))

    kwargs.setdefault("InputHits", "ITkPixelHitsOLD")
    kwargs.setdefault("OutputHits", "ITkPixelHits")

    acc.addEventAlgo(CompFactory.SiliconHitsTruthRelink(name="ITkPixelHitsTruthRelink", **kwargs))
    return acc


def ITkStripHitsTruthRelinkCfg(flags, **kwargs):
    acc = ComponentAccumulator()

    acc.merge(InputRenameCfg("SiHitCollection", "ITkStripHits", "ITkStripHitsOLD"))

    kwargs.setdefault("InputHits", "ITkStripHitsOLD")
    kwargs.setdefault("OutputHits", "ITkStripHits")

    acc.addEventAlgo(CompFactory.SiliconHitsTruthRelink(name="ITkStripHitsTruthRelink", **kwargs))
    return acc


def HGTD_HitsTruthRelinkCfg(flags, **kwargs):
    acc = ComponentAccumulator()

    acc.merge(InputRenameCfg("SiHitCollection", "HGTD_Hits", "HGTD_HitsOLD"))

    kwargs.setdefault("InputHits", "HGTD_HitsOLD")
    kwargs.setdefault("OutputHits", "HGTD_Hits")

    acc.addEventAlgo(CompFactory.SiliconHitsTruthRelink(name="HGTD_HitsTruthRelink", **kwargs))
    return acc


def CSC_HitsTruthRelinkCfg(flags, **kwargs):
    acc = ComponentAccumulator()

    acc.merge(InputRenameCfg("CSCSimHitCollection", "CSC_Hits", "CSC_HitsOLD"))

    kwargs.setdefault("InputHits", "CSC_HitsOLD")
    kwargs.setdefault("OutputHits", "CSC_Hits")

    acc.addEventAlgo(CompFactory.CSC_HitsTruthRelink(name="CSC_HitsTruthRelink", **kwargs))
    return acc


def MDT_HitsTruthRelinkCfg(flags, **kwargs):
    acc = ComponentAccumulator()

    acc.merge(InputRenameCfg("MDTSimHitCollection", "MDT_Hits", "MDT_HitsOLD"))

    kwargs.setdefault("InputHits", "MDT_HitsOLD")
    kwargs.setdefault("OutputHits", "MDT_Hits")

    acc.addEventAlgo(CompFactory.MDT_HitsTruthRelink(name="MDT_HitsTruthRelink", **kwargs))
    return acc


def MM_HitsTruthRelinkCfg(flags, **kwargs):
    acc = ComponentAccumulator()

    acc.merge(InputRenameCfg("MMSimHitCollection", "MicromegasSensitiveDetector", "MicromegasSensitiveDetectorOLD"))

    kwargs.setdefault("InputHits", "MicromegasSensitiveDetectorOLD")
    kwargs.setdefault("OutputHits", "MicromegasSensitiveDetector")

    acc.addEventAlgo(CompFactory.MM_HitsTruthRelink(name="MM_HitsTruthRelink", **kwargs))
    return acc


def RPC_HitsTruthRelinkCfg(flags, **kwargs):
    acc = ComponentAccumulator()

    acc.merge(InputRenameCfg("RPCSimHitCollection", "RPC_Hits", "RPC_HitsOLD"))

    kwargs.setdefault("InputHits", "RPC_HitsOLD")
    kwargs.setdefault("OutputHits", "RPC_Hits")

    acc.addEventAlgo(CompFactory.RPC_HitsTruthRelink(name="RPC_HitsTruthRelink", **kwargs))
    return acc


def TGC_HitsTruthRelinkCfg(flags, **kwargs):
    acc = ComponentAccumulator()

    acc.merge(InputRenameCfg("TGCSimHitCollection", "TGC_Hits", "TGC_HitsOLD"))

    kwargs.setdefault("InputHits", "TGC_HitsOLD")
    kwargs.setdefault("OutputHits", "TGC_Hits")

    acc.addEventAlgo(CompFactory.TGC_HitsTruthRelink(name="TGC_HitsTruthRelink", **kwargs))
    return acc


def sTGC_HitsTruthRelinkCfg(flags, **kwargs):
    acc = ComponentAccumulator()

    acc.merge(InputRenameCfg("sTGCSimHitCollection", "sTGCSensitiveDetector", "sTGCSensitiveDetectorOLD"))

    kwargs.setdefault("InputHits", "sTGCSensitiveDetectorOLD")
    kwargs.setdefault("OutputHits", "sTGCSensitiveDetector")

    acc.addEventAlgo(CompFactory.sTGC_HitsTruthRelink(name="sTGC_HitsTruthRelink", **kwargs))
    return acc


def DecorateTruthPileupParticlesCfg(flags, **kwargs):
    acc = ComponentAccumulator()

    kwargs.setdefault("InputParticleContainer", "TruthPileupParticles")
    kwargs.setdefault("OutputDecoration", "TruthPileupParticles.PVz")

    acc.addEventAlgo(CompFactory.PileUpTruthDecoration(name="DecorateTruthPileupParticles", **kwargs))
    return acc


def DecoratePileupAntiKt4TruthJetsCfg(flags, **kwargs):
    acc = ComponentAccumulator()

    kwargs.setdefault("InputParticleContainer", "AntiKt4TruthJets")
    kwargs.setdefault("OutputDecoration", "AntiKt4TruthJets.PVz")

    acc.addEventAlgo(CompFactory.PileUpTruthDecoration(name="DecoratePileupAntiKt4TruthJets", **kwargs))
    return acc


def DecoratePileupAntiKt6TruthJetsCfg(flags, **kwargs):
    acc = ComponentAccumulator()

    kwargs.setdefault("InputParticleContainer", "AntiKt6TruthJets")
    kwargs.setdefault("OutputDecoration", "AntiKt6TruthJets.PVz")

    acc.addEventAlgo(CompFactory.PileUpTruthDecoration(name="DecoratePileupAntiKt6TruthJets", **kwargs))
    return acc


def TruthResetAlgCfg(flags, **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault("InputMcEventCollection", "TruthEventOLD")
    kwargs.setdefault("OutputMcEventCollection", "BeamTruthEvent")
    result.addEventAlgo(CompFactory.TruthResetAlg(name="TruthResetAlg", **kwargs))
    return result
