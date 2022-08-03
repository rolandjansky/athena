# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
# Configuration of TrkVertexFitterUtils package
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def FullLinearizedTrackFactoryCfg(flags, name='FullLinearizedTrkFactory', **kwargs):
    from MagFieldServices.MagFieldServicesConfig import AtlasFieldCacheCondAlgCfg
    acc = AtlasFieldCacheCondAlgCfg(flags) # To produce AtlasFieldCacheCondObj

    if 'Extrapolator' not in kwargs:
        from TrkConfig.AtlasExtrapolatorConfig import InDetExtrapolatorCfg
        kwargs.setdefault("Extrapolator", acc.popToolsAndMerge(InDetExtrapolatorCfg(flags)))

    acc.setPrivateTools(CompFactory.Trk.FullLinearizedTrackFactory(name, **kwargs))
    return acc

def AtlasFullLinearizedTrackFactoryCfg(flags, name='AtlasFullLinearizedTrkFactory', **kwargs):
    from MagFieldServices.MagFieldServicesConfig import AtlasFieldCacheCondAlgCfg
    acc = AtlasFieldCacheCondAlgCfg(flags) # To produce AtlasFieldCacheCondObj

    if 'Extrapolator' not in kwargs:
        from TrkConfig.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg
        kwargs.setdefault("Extrapolator", acc.popToolsAndMerge(AtlasExtrapolatorCfg(flags)))

    acc.setPrivateTools(CompFactory.Trk.FullLinearizedTrackFactory(name, **kwargs))
    return acc

def DetAnnealingMakerCfg(flags, name='DetAnnealingMaker', **kwargs):
    acc = ComponentAccumulator()
    kwargs.setdefault("SetOfTemperatures", [1.0])
    acc.setPrivateTools(CompFactory.Trk.DetAnnealingMaker(name, **kwargs))
    return acc

def TauDetAnnealingMakerCfg(flags, name='tauRec_TauDetAnnealingMaker', **kwargs):
    kwargs.setdefault("SetOfTemperatures", [ 64, 32, 16, 8, 4, 2, 1 ])
    return DetAnnealingMakerCfg(flags, name, **kwargs)

def TrackToVertexIPEstimatorCfg(flags, name='TrackToVertexIPEstimator', **kwargs):
    acc = ComponentAccumulator()

    if 'Extrapolator' not in kwargs:
        from TrkConfig.AtlasExtrapolatorConfig import InDetExtrapolatorCfg
        kwargs.setdefault("Extrapolator", acc.popToolsAndMerge(InDetExtrapolatorCfg(flags)))

    if 'LinearizedTrackFactory' not in kwargs:
        kwargs.setdefault("LinearizedTrackFactory", acc.popToolsAndMerge(FullLinearizedTrackFactoryCfg(flags)))

    acc.setPrivateTools(CompFactory.Trk.TrackToVertexIPEstimator(name, **kwargs))
    return acc

def AtlasTrackToVertexIPEstimatorCfg(flags, name='AtlasTrackToVertexIPEstimator', **kwargs):
    acc = ComponentAccumulator()

    if 'Extrapolator' not in kwargs:
        from TrkConfig.AtlasExtrapolatorConfig import AtlasExtrapolatorCfg
        kwargs.setdefault("Extrapolator", acc.popToolsAndMerge(AtlasExtrapolatorCfg(flags)))

    if 'LinearizedTrackFactory' not in kwargs:
        kwargs.setdefault("LinearizedTrackFactory", acc.popToolsAndMerge(AtlasFullLinearizedTrackFactoryCfg(flags)))

    acc.setPrivateTools(CompFactory.Trk.TrackToVertexIPEstimator(name, **kwargs))
    return acc
