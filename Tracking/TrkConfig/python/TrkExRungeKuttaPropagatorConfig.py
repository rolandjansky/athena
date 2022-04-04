# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
# Configuration of TrkExRungeKuttaPropagator package
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def RungeKuttaPropagatorCfg(flags, name='AtlasRungeKuttaPropagator', **kwargs):
    result = ComponentAccumulator()
    tool = CompFactory.Trk.RungeKuttaPropagator(name, **kwargs)
    result.setPrivateTools(tool)
    return result

def egRungeKuttaPropagatorCfg(flags, name='egTrkPropagator', **kwargs):
    kwargs.setdefault("AccuracyParameter", 0.0001)
    return RungeKuttaPropagatorCfg(flags, name, **kwargs)

def MuonCombinedPropagatorCfg(flags, name='MuonCombinedPropagator', **kwargs ):
    if not flags.Muon.MuonTrigger:
        kwargs.setdefault("AccuracyParameter",   .000001 )
        kwargs.setdefault("IncludeBgradients",   True )
        kwargs.setdefault("MaxHelixStep",        .001 )
        kwargs.setdefault("MaxStraightLineStep", .001 )
    return RungeKuttaPropagatorCfg(flags, name, **kwargs)

def ITkRKPropagatorCfg(flags, name='ITkPropagator', **kwargs):
    kwargs.setdefault("AccuracyParameter", 0.0001)
    kwargs.setdefault("MaxStraightLineStep", .004)  # Fixes a failed fit
    return RungeKuttaPropagatorCfg(flags, name, **kwargs)
