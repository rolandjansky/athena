# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
# Configuration of InDetTrackSelectionTool package
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def InDetTrackSelectionTool_TrackTools_Cfg(flags, name="InDetTrackSelectionTool", **kwargs):
    acc = ComponentAccumulator()

    if "Extrapolator" not in kwargs:
        from TrkConfig.AtlasExtrapolatorConfig import InDetExtrapolatorCfg
        kwargs.setdefault("Extrapolator", acc.popToolsAndMerge(InDetExtrapolatorCfg(flags)))

    if "TrackSummaryTool" not in kwargs:
        from TrkConfig.TrkTrackSummaryToolConfig import InDetTrackSummaryToolCfg
        kwargs.setdefault("TrackSummaryTool", acc.popToolsAndMerge(InDetTrackSummaryToolCfg(flags)))

    kwargs.setdefault("UseTrkTrackTools", True)

    acc.setPrivateTools(CompFactory.InDet.InDetTrackSelectionTool(name, **kwargs))
    return acc

#############################################
#####  Configs based on CutLevel Loose  #####
#############################################

def InDetTrackSelectionTool_Loose_Cfg(flags, name="InDetTrackSelectionTool_Loose", **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("CutLevel", "Loose")
    acc.setPrivateTools(CompFactory.InDet.InDetTrackSelectionTool(name, **kwargs))
    return acc

def isoTrackSelectionToolCfg(flags, name="isoTrackSelectionTool", **kwargs):
    kwargs.setdefault('minPt', 1000)
    return InDetTrackSelectionTool_Loose_Cfg(flags, name, **kwargs)

####################################################
#####  Configs based on CutLevel LoosePrimary  #####
####################################################

def InDetTrackSelectionTool_LoosePrimary_Cfg(flags, name="InDetTrackSelectionTool_LoosePrimary", **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("CutLevel", "LoosePrimary")
    acc.setPrivateTools(CompFactory.InDet.InDetTrackSelectionTool(name, **kwargs))
    return acc

####################################################
#####  Configs based on CutLevel TightPrimary  #####
####################################################

def InDetTrackSelectionTool_TightPrimary_Cfg(flags, name="InDetTrackSelectionTool_TightPrimary", **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("CutLevel", "TightPrimary")
    acc.setPrivateTools(CompFactory.InDet.InDetTrackSelectionTool(name, **kwargs))
    return acc

def InDetTrackSelectionTool_TightPrimary_TrackTools_Cfg(flags, name="InDetTrackSelectionTool_TightPrimary", **kwargs):
    kwargs.setdefault("CutLevel", "TightPrimary")
    return InDetTrackSelectionTool_TrackTools_Cfg(flags, name, **kwargs)

def PFTrackSelectionToolCfg(flags, name="PFTrackSelectionTool", **kwargs):
    kwargs.setdefault('minPt', 500.0)
    return InDetTrackSelectionTool_TightPrimary_Cfg(flags, name, **kwargs)

def IDAlignMonTrackSelectionToolCfg(flags, name="IDAlignMonTrackSelectionTool", **kwargs):
    kwargs.setdefault("TrackSummaryTool", None)
    kwargs.setdefault("maxNPixelHoles"             , 1)
    kwargs.setdefault("minNBothInnermostLayersHits", 0)
    kwargs.setdefault("minNInnermostLayerHits"     , 1)
    kwargs.setdefault("minPt"                      , 5000)
    kwargs.setdefault("maxD0"                      , 100000)
    kwargs.setdefault("maxZ0SinTheta"              , 150)
    return InDetTrackSelectionTool_TightPrimary_TrackTools_Cfg(flags, name, **kwargs)

###############################################
#####  Configs not based on any CutLevel  #####
###############################################

def VtxInDetTrackSelectionCfg(flags, name="VertexInDetTrackSelectionTool", **kwargs):
    if flags.Detector.GeometryITk:
        vtxFlags = flags.ITk.PriVertex
    else:
        vtxFlags = flags.InDet.PriVertex

    for key in (
        "maxAbsEta",
        "maxD0",
        "maxNPixelHoles",
        "maxSigmaD0",
        "maxSigmaZ0SinTheta",
        "maxZ0",
        "maxZ0SinTheta",
        "minNInnermostLayerHits",
        "minNPixelHits",
        "minNSctHits",
        "minNSiHits",
        "minNTrtHits",
        "minPt",
    ):
        kwargs.setdefault(key, getattr(vtxFlags, key))

    kwargs.setdefault("UseTrkTrackTools", False)
    return InDetTrackSelectionTool_TightPrimary_Cfg(flags, name, **kwargs)


def TrigVtxInDetTrackSelectionCfg(flags, name="InDetTrigDetailedTrackSelectionTool", **kwargs):
    from InDetTrigRecExample.TrigInDetConfiguredVtxCuts import ConfiguredTrigVtxCuts
    cuts = ConfiguredTrigVtxCuts()
    cuts.printInfo()

    signature = kwargs.pop("signature","")
    from TrigInDetConfig.ConfigSettings import getInDetTrigConfig
    config = getInDetTrigConfig(signature)

    acc = ComponentAccumulator()

    kwargs.setdefault("CutLevel", cuts.TrackCutLevel())
    kwargs.setdefault("minPt", cuts.minPT())
    kwargs.setdefault("maxD0", cuts.IPd0Max())
    kwargs.setdefault("maxZ0", cuts.z0Max())
    kwargs.setdefault("maxZ0SinTheta", cuts.IPz0Max())
    kwargs.setdefault("maxSigmaD0", cuts.sigIPd0Max())
    kwargs.setdefault("maxSigmaZ0SinTheta", cuts.sigIPz0Max())
    kwargs.setdefault("maxChiSqperNdf", cuts.fitChi2OnNdfMax())
    kwargs.setdefault("maxAbsEta", cuts.etaMax())
    kwargs.setdefault("minNInnermostLayerHits", cuts.nHitInnermostLayer())
    kwargs.setdefault("minNPixelHits", cuts.nHitPix())
    kwargs.setdefault("maxNPixelHoles", cuts.nHolesPix())
    kwargs.setdefault("minNSctHits", cuts.nHitSct())
    kwargs.setdefault("minNTrtHits", cuts.nHitTrt())
    kwargs.setdefault("minNSiHits", config.minNSiHits_vtx if config.minNSiHits_vtx is not None
                      else cuts.nHitSi())
    # N.B. Legacy config used to set extrapolator + trackSummary tools but since UseTrkTrackTools is not set to True, they're not used in the InDetTrackSelectionTool

    acc.setPrivateTools(CompFactory.InDet.InDetTrackSelectionTool(name+signature, **kwargs))
    return acc

def Tau_InDetTrackSelectionToolForTJVACfg(flags, name="tauRec_InDetTrackSelectionToolForTJVA", **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("minPt", 1000.)
    kwargs.setdefault("minNPixelHits", 2)
    kwargs.setdefault("minNSiHits", 7)
    acc.setPrivateTools(CompFactory.InDet.InDetTrackSelectionTool(name, **kwargs))
    return acc

def InDetGlobalLRTMonAlg_TrackSelectionToolCfg(flags, name="InDetGlobalLRTMonAlg_TrackSelectionTool", **kwargs):
    kwargs.setdefault("minPt", 1000.)
    kwargs.setdefault("maxNPixelHoles", 1)
    return InDetTrackSelectionTool_TrackTools_Cfg(flags, name, **kwargs)
