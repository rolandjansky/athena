# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# Configuration of tools for ATLAS extrapolator

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory


def AtlasNavigatorCfg(flags,
                      name='AtlasNavigator'):
    # get the correct TrackingGeometry setup
    result = ComponentAccumulator()
    from TrackingGeometryCondAlg.AtlasTrackingGeometryCondAlgConfig import (
        TrackingGeometryCondAlgCfg)
    acc = TrackingGeometryCondAlgCfg(flags)
    geom_cond_key = acc.getPrimary().TrackingGeometryWriteKey
    result.merge(acc)
    # the UNIQUE NAVIGATOR ( === UNIQUE GEOMETRY) ------------------------
    Trk__Navigator = CompFactory.Trk.Navigator
    AtlasNavigator = Trk__Navigator(name=name,
                                    TrackingGeometryKey=geom_cond_key)
    result.addPublicTool(AtlasNavigator, primary=True)
    return result


def AtlasRKPropagatorCfg(flags,
                         name='AtlasRungeKuttaPropagator',
                         **kwargs):
    result = ComponentAccumulator()
    RkPropagator = CompFactory.Trk.RungeKuttaPropagator
    AtlasRungeKuttaPropagator = RkPropagator(name, **kwargs)
    result.addPublicTool(AtlasRungeKuttaPropagator, primary=True)
    return result


def AtlasSTEP_PropagatorCfg(flags,
                            name='AtlasSTEP_Propagator',
                            **kwargs):
    result = ComponentAccumulator()
    STEP_Propagator = CompFactory.Trk.STEP_Propagator
    AtlasSTEP_Propagator = STEP_Propagator(name, **kwargs)
    result.addPublicTool(AtlasSTEP_Propagator, primary=True)
    return result


def AtlasNoMatSTEP_PropagatorCfg(flags,
                                 name='AtlasNoMatSTEP_Propagator',
                                 **kwargs):
    kwargs.setdefault("MaterialEffects", False)
    return AtlasSTEP_PropagatorCfg(flags, name, **kwargs)


def InDetPropagatorCfg(flags,
                       name='InDetPropagator',
                       **kwargs):
    result = ComponentAccumulator()

    InDetPropagator = None
    if flags.InDet.propagatorType == "STEP":
        InDetPropagator = result.getPrimaryAndMerge(
            AtlasSTEP_PropagatorCfg(flags, name, **kwargs))
    elif flags.InDet.propagatorType == "RungeKutta":
        kwargs.setdefault("AccuracyParameter", 0.0001)
        kwargs.setdefault("MaxStraightLineStep", .004)  # Fixes a failed fit
        InDetPropagator = result.getPrimaryAndMerge(
            AtlasRKPropagatorCfg(flags, name, **kwargs))

    result.addPublicTool(InDetPropagator, primary=True)
    return result


def ITkPropagatorCfg(flags,
                     name='ITkPropagator',
                     **kwargs):
    result = ComponentAccumulator()

    kwargs.setdefault("AccuracyParameter", 0.0001)
    kwargs.setdefault("MaxStraightLineStep", .004)  # Fixes a failed fit
    ITkPropagator = result.getPrimaryAndMerge(AtlasRKPropagatorCfg(flags, name, **kwargs))

    result.addPublicTool(ITkPropagator, primary=True)
    return result


def AtlasMaterialEffectsUpdatorCfg(flags,
                                   name='AtlasMaterialEffectsUpdator',
                                   **kwargs):
    result = ComponentAccumulator()
    MaterialEffectsUpdator = CompFactory.Trk.MaterialEffectsUpdator
    AtlasMaterialEffectsUpdator = MaterialEffectsUpdator(name, **kwargs)
    result.addPublicTool(AtlasMaterialEffectsUpdator, primary=True)
    return result


def AtlasMaterialEffectsUpdatorLandauCfg(flags,
                                         name='AtlasMaterialEffectsUpdatorLandau',
                                         **kwargs):
    kwargs.setdefault("LandauMode", True)
    return AtlasMaterialEffectsUpdatorCfg(flags, name, **kwargs)


def AtlasNoElossMaterialEffectsUpdatorCfg(flags,
                                          name='AtlasNoElossMaterialEffectsUpdator',
                                          **kwargs):
    kwargs.setdefault("EnergyLoss", False)
    return AtlasMaterialEffectsUpdatorCfg(flags, name, **kwargs)


def InDetMaterialEffectsUpdatorCfg(flags,
                                   name="InDetMaterialEffectsUpdator",
                                   **kwargs):
    if not flags.BField.solenoidOn:
        import AthenaCommon.SystemOfUnits as Units
        kwargs.setdefault("EnergyLoss", False)
        kwargs.setdefault("ForceMomentum", True)
        kwargs.setdefault("ForcedMomentumValue", 1000*Units.MeV)
    return AtlasMaterialEffectsUpdatorCfg(flags, name, **kwargs)


def ITkMaterialEffectsUpdatorCfg(flags,
                                 name="ITkMaterialEffectsUpdator",
                                 **kwargs):
    return InDetMaterialEffectsUpdatorCfg(flags, name, **kwargs)
