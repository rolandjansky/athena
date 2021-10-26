# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# conversion/secondary vertex finding (for egamma)
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


def InDetConversionTrackSelectorToolCfg(flags, name="InDetConversionTrackSelectorTool"):
    acc = ComponentAccumulator()
    kwargs = {}
    from TrkConfig.AtlasExtrapolatorConfig import InDetExtrapolatorCfg
    kwargs["Extrapolator"] = acc.getPrimaryAndMerge(
        InDetExtrapolatorCfg(flags))
    kwargs["RatioCut1"] = flags.InDet.SecVertex.TrkSel.RatioCut1
    kwargs["RatioCut3"] = flags.InDet.SecVertex.TrkSel.RatioCut3
    kwargs["RatioTRT"] = flags.InDet.SecVertex.TrkSel.RatioTRT
    kwargs["RatioV0"] = flags.InDet.SecVertex.TrkSel.RatioV0
    kwargs["maxSiD0"] = flags.InDet.SecVertex.TrkSel.maxSiD0
    kwargs["maxSiZ0"] = flags.InDet.SecVertex.TrkSel.maxSiZ0
    kwargs["maxTrtD0"] = flags.InDet.SecVertex.TrkSel.maxTrtD0
    kwargs["maxTrtZ0"] = flags.InDet.SecVertex.TrkSel.maxTrtZ0
    kwargs["minPt"] = flags.InDet.SecVertex.TrkSel.minPt
    kwargs["significanceD0_Si"] = flags.InDet.SecVertex.TrkSel.significanceD0_Si

    tool = CompFactory.InDet.InDetConversionTrackSelectorTool(name, **kwargs)
    acc.setPrivateTools(tool)
    return acc


def TrkVKalVrtFitterCfg(flags, name="TrkVKalVrtFitter"):
    acc = ComponentAccumulator()
    kwargs = {}
    from TrkConfig.AtlasExtrapolatorConfig import InDetExtrapolatorCfg
    kwargs["Extrapolator"] = acc.getPrimaryAndMerge(
        InDetExtrapolatorCfg(flags))
    kwargs["FirstMeasuredPoint"] = flags.InDet.SecVertex.Fitter.FirstMeasuredPoint
    kwargs["FirstMeasuredPointLimit"] = flags.InDet.SecVertex.Fitter.FirstMeasuredPointLimit
    kwargs["InputParticleMasses"] = flags.InDet.SecVertex.Fitter.InputParticleMasses
    kwargs["IterationNumber"] = flags.InDet.SecVertex.Fitter.IterationNumber
    kwargs["MakeExtendedVertex"] = flags.InDet.SecVertex.Fitter.MakeExtendedVertex
    kwargs["Robustness"] = flags.InDet.SecVertex.Fitter.Robustness
    kwargs["usePhiCnst"] = flags.InDet.SecVertex.Fitter.usePhiCnst
    kwargs["useThetaCnst"] = flags.InDet.SecVertex.Fitter.useThetaCnst

    tool = CompFactory.Trk.TrkVKalVrtFitter(name, **kwargs)
    acc.setPrivateTools(tool)
    return acc


def VertexPointEstimatorCfg(flags, name="VertexPointEstimator"):
    acc = ComponentAccumulator()
    kwargs = {}
    # TODO find out where this setting comes from (flags?)
    kwargs["MaxPhi"] = [0.05, 0.2, 0.2]
    tool = CompFactory.InDet.VertexPointEstimator(name, **kwargs)
    acc.setPrivateTools(tool)
    return acc


def ConversionFinderCfg(flags, name="ConversionFinderTool"):
    """Configures InDet::InDetConversionFinderTools """

    acc = ComponentAccumulator()
    kwargs = {}
    from TrkConfig.AtlasExtrapolatorConfig import InDetExtrapolatorCfg
    kwargs["Extrapolator"] = acc.getPrimaryAndMerge(
        InDetExtrapolatorCfg(flags))
    kwargs["PostSelector"] = acc.getPrimaryAndMerge(
        ConversionPostSelectorCfg(flags))
    kwargs["SingleTrackConversionTool"] = acc.getPrimaryAndMerge(
        SingleTrackConversionToolCfg(flags))
    kwargs["TrackPairsSelector"] = acc.getPrimaryAndMerge(
        TrackPairsSelectorCfg(flags))
    kwargs["TrackSelectorTool"] = acc.getPrimaryAndMerge(
        InDetConversionTrackSelectorToolCfg(flags))
    kwargs["VertexFitterTool"] = acc.getPrimaryAndMerge(
        TrkVKalVrtFitterCfg(flags))
    kwargs["VertexPointEstimator"] = acc.getPrimaryAndMerge(
        VertexPointEstimatorCfg(flags))
    kwargs["TrackParticleCollection"] = flags.Egamma.Keys.Output.GSFTrackParticles
    kwargs["IsConversion"] = True
    tool = CompFactory.InDet.InDetConversionFinderTools(name, **kwargs)
    acc.setPrivateTools(tool)
    return acc
