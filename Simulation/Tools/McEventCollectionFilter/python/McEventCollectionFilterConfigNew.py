# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# utilities
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from SGComps.AddressRemappingConfig import InputRenameCfg

def McEventCollectionFilterCfg(flags, **kwargs):
    acc = ComponentAccumulator()
    #acc.merge(LArGMCfg(flags))
    acc.merge(InputRenameCfg("McEventCollection","TruthEvent","TruthEventOLD"))
    acc.merge(InputRenameCfg("SiHitCollection","BCMHits","BCMHitsOLD"))
    acc.merge(InputRenameCfg("SiHitCollection","PixelHits","PixelHitsOLD"))
    acc.merge(InputRenameCfg("SiHitCollection","SCT_Hits","SCT_HitsOLD"))
    acc.merge(InputRenameCfg("TRTUncompressedHitCollection","TRTUncompressedHits","TRTUncompressedHitsOLD"))
    acc.merge(InputRenameCfg("CSCSimHitCollection","CSC_Hits","CSC_HitsOLD"))
    acc.merge(InputRenameCfg("MDTSimHitCollection","MDT_Hits","MDT_HitsOLD"))
    acc.merge(InputRenameCfg("RPCSimHitCollection","RPC_Hits","RPC_HitsOLD"))
    acc.merge(InputRenameCfg("TGCSimHitCollection","TGC_Hits","TGC_HitsOLD"))
    acc.merge(InputRenameCfg("sTGCSimHitCollection","sTGCSensitiveDetector","sTGCSensitiveDetectorOLD"))
    acc.merge(InputRenameCfg("MMSimHitCollection","MicromegasSensitiveDetector","MicromegasSensitiveDetectorOLD"))
    kwargs.setdefault("UseBCMHits", flags.Detector.EnableBCM)
    kwargs.setdefault("UseTRTHits", flags.Detector.EnableTRT)
    kwargs.setdefault("UseCSCHits", flags.Detector.EnableCSC)
    kwargs.setdefault("UseSTGCHits", flags.Detector.EnablesTGC)
    kwargs.setdefault("UseMMHits", flags.Detector.EnableMM)
    McEventCollectionFilter = CompFactory.McEventCollectionFilter
    acc.addEventAlgo(McEventCollectionFilter(name="McEventCollectionFilter", **kwargs))
    return acc
