# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr
def getCombinedScintillatorSD(name="CombinedScintillatorSD", **kwargs):
    kwargs.setdefault("LogicalVolumeNames", ['CombinedScintillator::CScintillatorLayer'])
    kwargs.setdefault("OutputCollectionNames", ["TileTBHits"])
    return CfgMgr.CombinedScintillatorSDTool(name, **kwargs)

def getCombinedScintillatorTool(name="CombinedScintillator", **kwargs):
    kwargs.setdefault("DectectorName", "CScintillatorLayer")
    kwargs.setdefault("RMin", 2266.5, "");
    kwargs.setdefault("RMax", 2281.5, "");
    kwargs.setdefault("DZSci",  220., "");
    kwargs.setdefault("PhiPos", 2.50, "");
    kwargs.setdefault("PhiNeg", -19.3,"");
    return CfgMgr.CombinedScintillatorTool(name, **kwargs)
