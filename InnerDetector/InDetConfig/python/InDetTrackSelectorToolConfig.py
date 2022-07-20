# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
# Configuration of InDetTrackSelectorTool package

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory


def InDetConversionTrackSelectorToolCfg(flags, name="TrackSelector", **kwargs):
    acc = ComponentAccumulator()

    if "Extrapolator" not in kwargs:
        from TrkConfig.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg
        kwargs.setdefault("Extrapolator", acc.popToolsAndMerge(AtlasExtrapolatorCfg(flags)))

    kwargs.setdefault("RatioCut1", flags.InDet.SecVertex.TrkSel.RatioCut1)
    kwargs.setdefault("RatioCut2", flags.InDet.SecVertex.TrkSel.RatioCut2)
    kwargs.setdefault("RatioCut3", flags.InDet.SecVertex.TrkSel.RatioCut3)
    kwargs.setdefault("TRTTrksBinnedRatioTRT", flags.InDet.SecVertex.TrkSel.TRTTrksBinnedRatioTRT)
    kwargs.setdefault("TRTTrksEtaBins", flags.InDet.SecVertex.TrkSel.TRTTrksEtaBins)
    kwargs.setdefault("RatioTRT", flags.InDet.SecVertex.TrkSel.RatioTRT)
    kwargs.setdefault("RatioV0", flags.InDet.SecVertex.TrkSel.RatioV0)
    kwargs.setdefault("maxSiD0", flags.InDet.SecVertex.TrkSel.maxSiD0)
    kwargs.setdefault("maxSiZ0", flags.InDet.SecVertex.TrkSel.maxSiZ0)
    kwargs.setdefault("maxTrtD0", flags.InDet.SecVertex.TrkSel.maxTrtD0)
    kwargs.setdefault("maxTrtZ0", flags.InDet.SecVertex.TrkSel.maxTrtZ0)
    kwargs.setdefault("minPt", flags.InDet.SecVertex.TrkSel.minPt)
    kwargs.setdefault("significanceD0_Si", flags.InDet.SecVertex.TrkSel.significanceD0_Si)
    kwargs.setdefault("IsConversion", flags.InDet.SecVertex.TrkSel.IsConversion)

    acc.setPrivateTools(CompFactory.InDet.InDetConversionTrackSelectorTool(name, **kwargs))
    return acc

def InDetTRTDriftCircleCutToolCfg(flags, name='InDetTRTDriftCircleCutTool', **kwargs):
    from TRT_ConditionsAlgs.TRT_ConditionsAlgsConfig import TRTActiveCondAlgCfg
    result = TRTActiveCondAlgCfg(flags) # To produce the input TRTCond::ActiveFraction CondHandle
    kwargs.setdefault("MinOffsetDCs", 5)
    kwargs.setdefault("UseNewParameterization", flags.InDet.Tracking.ActivePass.useNewParameterizationTRT)
    kwargs.setdefault("UseActiveFractionSvc", flags.Detector.EnableTRT)
    result.setPrivateTools(CompFactory.InDet.InDetTrtDriftCircleCutTool(name, **kwargs))
    return result

def InDetTrigTRTDriftCircleCutToolCfg(flags, name='InDetTrigTRTDriftCircleCutTool', **kwargs):
    kwargs.setdefault("UseNewParameterization", True)
    kwargs.setdefault("UseActiveFractionSvc", True)
    return InDetTRTDriftCircleCutToolCfg(flags, name, **kwargs)
