# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration


from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def PrimaryVertexRefittingToolCfg(ConfigFlags, **kwargs):
    acc = ComponentAccumulator()
    from InDetConfig.TrackingCommonConfig import TrackToVertexIPEstimatorCfg
    kwargs.setdefault( "TrackToVertexIPEstimator", acc.popToolsAndMerge( TrackToVertexIPEstimatorCfg(ConfigFlags,**kwargs) ) )
    acc.setPrivateTools( CompFactory.Analysis.PrimaryVertexRefitter( **kwargs) )
    return acc