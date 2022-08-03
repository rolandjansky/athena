# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Configuration of RungeKuttaPropagator
# for track parameters propagation through magnetic field.
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from MagFieldServices.MagFieldServicesConfig import AtlasFieldCacheCondAlgCfg

def RungeKuttaPropagatorCfg(flags, name='AtlasRungeKuttaPropagator', **kwargs):
    result = ComponentAccumulator()
    acc = AtlasFieldCacheCondAlgCfg(flags)
    result.merge(acc)

    kwargs.setdefault("IncludeBgradients", False) # Just so this doesn't seem unconfigured. Real fix is to use default name. 
    tool = CompFactory.Trk.RungeKuttaPropagator(name, **kwargs)
    result.setPrivateTools(tool)
    return result


def InDetPropagatorCfg(flags, name='InDetPropagator', **kwargs):
    kwargs.setdefault("AccuracyParameter", 0.0001)
    kwargs.setdefault("MaxStraightLineStep", .004)  # Fixes a failed fit
    return RungeKuttaPropagatorCfg(flags, name, **kwargs)


def GSFRungeKuttaPropagatorCfg(flags, name='GSFTrkPropagator', **kwargs):
    kwargs.setdefault("AccuracyParameter", 0.0001)
    return RungeKuttaPropagatorCfg(flags, name, **kwargs)


def MuonCombinedPropagatorCfg(flags, name='MuonCombinedPropagator', **kwargs):
    if not flags.Muon.MuonTrigger:
        kwargs.setdefault("AccuracyParameter",   .000001)
        kwargs.setdefault("IncludeBgradients",   True)
        kwargs.setdefault("MaxHelixStep",        .001)
        kwargs.setdefault("MaxStraightLineStep", .001)
    return RungeKuttaPropagatorCfg(flags, name, **kwargs)


def ITkPropagatorCfg(flags, name='ITkPropagator', **kwargs):
    kwargs.setdefault("AccuracyParameter", 0.0001)
    kwargs.setdefault("MaxStraightLineStep", .004)  # Fixes a failed fit
    return RungeKuttaPropagatorCfg(flags, name, **kwargs)
