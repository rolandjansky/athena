# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
# Configuration of InDetTrackSelectorTool package

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory


def InDetConversionTrackSelectorToolCfg(flags, name="TrackSelector"):
    acc = ComponentAccumulator()
    kwargs = {}
    from TrkConfig.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg
    kwargs["Extrapolator"] = acc.popToolsAndMerge(AtlasExtrapolatorCfg(flags))
    kwargs["RatioCut1"] = flags.InDet.SecVertex.TrkSel.RatioCut1
    kwargs["RatioCut2"] = flags.InDet.SecVertex.TrkSel.RatioCut2
    kwargs["RatioCut3"] = flags.InDet.SecVertex.TrkSel.RatioCut3
    kwargs["TRTTrksBinnedRatioTRT"] = flags.InDet.SecVertex.TrkSel.TRTTrksBinnedRatioTRT
    kwargs["TRTTrksEtaBins"] = flags.InDet.SecVertex.TrkSel.TRTTrksEtaBins
    kwargs["RatioTRT"] = flags.InDet.SecVertex.TrkSel.RatioTRT
    kwargs["RatioV0"] = flags.InDet.SecVertex.TrkSel.RatioV0
    kwargs["maxSiD0"] = flags.InDet.SecVertex.TrkSel.maxSiD0
    kwargs["maxSiZ0"] = flags.InDet.SecVertex.TrkSel.maxSiZ0
    kwargs["maxTrtD0"] = flags.InDet.SecVertex.TrkSel.maxTrtD0
    kwargs["maxTrtZ0"] = flags.InDet.SecVertex.TrkSel.maxTrtZ0
    kwargs["minPt"] = flags.InDet.SecVertex.TrkSel.minPt
    kwargs["significanceD0_Si"] = flags.InDet.SecVertex.TrkSel.significanceD0_Si
    kwargs["IsConversion"] = flags.InDet.SecVertex.TrkSel.IsConversion

    tool = CompFactory.InDet.InDetConversionTrackSelectorTool(name, **kwargs)
    acc.setPrivateTools(tool)
    return acc
