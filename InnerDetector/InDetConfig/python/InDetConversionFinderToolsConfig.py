# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
# Configuration of InDetConversionFinderTools package

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def ConversionPostSelectorCfg(flags, name="ConversionPostSelector"):
    acc = ComponentAccumulator()
    kwargs = {}
    kwargs["MaxChi2Vtx"] = flags.InDet.SecVertex.SecVtxPost.MaxChi2Vtx
    kwargs["MaxInvariantMass"] = flags.InDet.SecVertex.SecVtxPost.MaxInvariantMass
    kwargs["MaxPhiVtxTrk"] = flags.InDet.SecVertex.SecVtxPost.MaxPhiVtxTrk
    kwargs["MaxdR"] = flags.InDet.SecVertex.SecVtxPost.MaxdR
    kwargs["MinFitMomentum"] = flags.InDet.SecVertex.SecVtxPost.MinFitMomentum
    kwargs["MinPt"] = flags.InDet.SecVertex.SecVtxPost.MinPt
    kwargs["MinRadius"] = flags.InDet.SecVertex.SecVtxPost.MinRadius

    tool = CompFactory.InDet.ConversionPostSelector(name, **kwargs)
    acc.setPrivateTools(tool)
    return acc

def SingleTrackConversionToolCfg(flags, name="SingleTrackConversionTool"):
    acc = ComponentAccumulator()
    kwargs = {}
    kwargs["MaxBLayerHits"] = flags.InDet.SecVertex.SingleTrk.MaxBLayerHits
    kwargs["MinInitialHitRadius"] = flags.InDet.SecVertex.SingleTrk.MinInitialHitRadius
    kwargs["MinInitialHitRadius_noBlay"] = flags.InDet.SecVertex.SingleTrk.MinInitialHitRadius_noBlay
    kwargs["MinRatioOfHLhits"] = flags.InDet.SecVertex.SingleTrk.MinRatioOfHLhits
    tool = CompFactory.InDet.SingleTrackConversionTool(name, **kwargs)
    acc.setPrivateTools(tool)
    return acc

def TrackPairsSelectorCfg(flags, name="TrackPairsSelector"):
    acc = ComponentAccumulator()
    kwargs = {}
    kwargs["ConversionFinderHelperTool"] = CompFactory.InDet.ConversionFinderUtils()
    kwargs["DistanceTool"] = CompFactory.Trk.SeedNewtonTrkDistanceFinder()
    kwargs["MaxDistBetweenTracks"] = flags.InDet.SecVertex.TrkPairSel.MaxDistBetweenTracks
    kwargs["MaxEta"] = flags.InDet.SecVertex.TrkPairSel.MaxEta
    kwargs["MaxFirstHitRadius"] = flags.InDet.SecVertex.TrkPairSel.MaxFirstHitRadius
    kwargs["MaxInitDistance"] = flags.InDet.SecVertex.TrkPairSel.MaxInitDistance
    kwargs["MinTrackAngle"] = flags.InDet.SecVertex.TrkPairSel.MinTrackAngle

    tool = CompFactory.InDet.TrackPairsSelector(name, **kwargs)
    acc.setPrivateTools(tool)
    return acc

def VertexPointEstimatorCfg(flags, name="VertexPointEstimator"):
    acc = ComponentAccumulator()
    kwargs = {}
    kwargs["MinDeltaR"] = flags.InDet.SecVertex.VtxPt.MinDeltaR
    kwargs["MaxDeltaR"] = flags.InDet.SecVertex.VtxPt.MaxDeltaR
    kwargs["MaxPhi"] = flags.InDet.SecVertex.VtxPt.MaxPhi
    tool = CompFactory.InDet.VertexPointEstimator(name, **kwargs)
    acc.setPrivateTools(tool)
    return acc

def InDetConversionFinderToolsCfg(flags, name="ConversionFinderTool"):
    """Configures InDet::InDetConversionFinderTools """

    acc = ComponentAccumulator()
    kwargs = {}

    kwargs["PostSelector"] = acc.getPrimaryAndMerge(
        ConversionPostSelectorCfg(flags))
    kwargs["SingleTrackConversionTool"] = acc.getPrimaryAndMerge(
        SingleTrackConversionToolCfg(flags))
    kwargs["TrackPairsSelector"] = acc.getPrimaryAndMerge(
        TrackPairsSelectorCfg(flags))
    kwargs["VertexPointEstimator"] = acc.getPrimaryAndMerge(
        VertexPointEstimatorCfg(flags))

    from InDetConfig.InDetTrackSelectorToolConfig import InDetConversionTrackSelectorToolCfg
    kwargs["TrackSelectorTool"] = acc.getPrimaryAndMerge(
        InDetConversionTrackSelectorToolCfg(flags))

    from TrkConfig.TrkVKalVrtFitterConfig import TrkVKalVrtFitterCfg
    kwargs["VertexFitterTool"] = acc.getPrimaryAndMerge(
        TrkVKalVrtFitterCfg(flags))
    kwargs["TrackParticleCollection"] = flags.Egamma.Keys.Output.GSFTrackParticles
    kwargs["IsConversion"] = True
    kwargs["MaxDistVtxHit"] = flags.InDet.SecVertex.Finder.MaxDistVtxHit
    kwargs["MinDistVtxHit"] = flags.InDet.SecVertex.Finder.MinDistVtxHit
    kwargs["MinFlightAngle"] = flags.InDet.SecVertex.Finder.MinFlightAngle
    kwargs["MinInitVtxR"] = flags.InDet.SecVertex.Finder.MinInitVtxR
    kwargs["RemoveTrtTracks"] = flags.InDet.SecVertex.Finder.RemoveTrtTracks
    tool = CompFactory.InDet.InDetConversionFinderTools(name, **kwargs)
    acc.setPrivateTools(tool)
    return acc
