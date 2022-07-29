# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentFactory import CompFactory

def FullLinearizedTrackFactoryCfg(flags, name='TrackToVertexIPEstimator', **kwargs):
    from TrkConfig.AtlasExtrapolatorConfig import InDetExtrapolatorCfg

    acc = InDetExtrapolatorCfg(flags)
    kwargs.setdefault("Extrapolator", acc.getPrimary()) #private
    # FIXME Configure AtlasFieldCacheCondObj
    acc.setPrivateTools(CompFactory.Trk.FullLinearizedTrackFactory( **kwargs))
    return acc

def TrackToVertexIPEstimatorCfg(flags, name='TrackToVertexIPEstimator', **kwargs):
    from TrkConfig.AtlasExtrapolatorConfig import InDetExtrapolatorCfg

    # All sub-tools are currently public
    acc = FullLinearizedTrackFactoryCfg(flags, 'FullLinearizedTrkFactory')
    linearizedTrackFactory = acc.popPrivateTools()
    acc.addPublicTool(linearizedTrackFactory)
    kwargs.setdefault("LinearizedTrackFactory", linearizedTrackFactory )
    if 'Extrapolator' not in kwargs:
        extrapolator = acc.getPrimaryAndMerge( InDetExtrapolatorCfg(flags) )
        kwargs.setdefault("Extrapolator", extrapolator)
    # Think it's okay not to configure VertexUpdator
    acc.setPrivateTools(CompFactory.Trk.TrackToVertexIPEstimator( **kwargs))
    return acc
