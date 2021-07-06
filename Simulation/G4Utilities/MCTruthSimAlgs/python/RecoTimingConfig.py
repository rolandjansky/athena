"""Configuration for RecoTimingObj with ComponentAccumulator/ConfigFlags
Based on a fragment from MCTruthSimAlgsConfig.py
from AthenaConfiguration.ComponentFactory import CompFactory

Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import ProductionStep
from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
from Digitization.PileUpToolsConfig import PileUpToolsCfg


# The earliest and latest bunch crossing time for which interactions will be sent
# to the RecoTimingObj merging code.
def TimingObj_FirstXing():
    return -1


def TimingObj_LastXing():
    return 1


def MergeRecoTimingObjCfg(flags, name="MergeRecoTimingObjTool", **kwargs):
    """Return ComponentAccumulator configured for MergeRecoTimingObjTool"""
    acc = OutputStreamCfg(flags, "RDO", ["RecoTimingObj#*EVNTtoHITS_timings"])
    
    if flags.Digitization.DoXingByXingPileUp:
        kwargs.setdefault("FirstXing", TimingObj_FirstXing())
        kwargs.setdefault("LastXing",  TimingObj_LastXing())

    kwargs.setdefault("RecoTimingObjInputKey", "EVNTtoHITS_timings")
    if flags.Common.ProductionStep == ProductionStep.PileUpPresampling:
        kwargs.setdefault("RecoTimingObjOutputKey", flags.Overlay.BkgPrefix + "EVNTtoHITS_timings")
    else:
        kwargs.setdefault("RecoTimingObjOutputKey", "EVNTtoHITS_timings")    
    tool = CompFactory.MergeRecoTimingObjTool(name, **kwargs)
    acc.merge(PileUpToolsCfg(flags, PileUpTools=tool))
    return acc
