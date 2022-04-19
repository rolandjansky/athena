# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Configuration of tools for ATLAS extrapolator

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory


def AtlasNavigatorCfg(flags,
                      name='AtlasNavigator',
                      **kwargs):
    # get the correct TrackingGeometry setup
    result = ComponentAccumulator()
    if 'TrackingGeometryKey' not in kwargs:
        from TrackingGeometryCondAlg.AtlasTrackingGeometryCondAlgConfig import TrackingGeometryCondAlgCfg
        acc = TrackingGeometryCondAlgCfg(flags)
        geom_cond_key = acc.getPrimary().TrackingGeometryWriteKey
        result.merge(acc)
        kwargs.setdefault("TrackingGeometryKey", geom_cond_key)

    # the UNIQUE NAVIGATOR ( === UNIQUE GEOMETRY) ------------------------
    result.setPrivateTools(CompFactory.Trk.Navigator(name,**kwargs))
    return result

def InDetPropagatorCfg(flags,
                       name='InDetPropagator',
                       **kwargs):
    result = ComponentAccumulator()

    InDetPropagator = None
    if flags.InDet.Tracking.propagatorType == "STEP":
        from TrkConfig.TrkExSTEP_PropagatorConfig import AtlasSTEP_PropagatorCfg
        InDetPropagator = result.popToolsAndMerge(
            AtlasSTEP_PropagatorCfg(flags, name, **kwargs))
    elif flags.InDet.Tracking.propagatorType == "RungeKutta":
        kwargs.setdefault("AccuracyParameter", 0.0001)
        kwargs.setdefault("MaxStraightLineStep", .004)  # Fixes a failed fit
        from TrkConfig.TrkExRungeKuttaPropagatorConfig import RungeKuttaPropagatorCfg
        InDetPropagator = result.popToolsAndMerge(
            RungeKuttaPropagatorCfg(flags, name, **kwargs))

    result.setPrivateTools(InDetPropagator)
    return result


def ITkPropagatorCfg(flags,
                     name='ITkPropagator',
                     **kwargs):
    from TrkConfig.TrkExRungeKuttaPropagatorConfig import ITkRKPropagatorCfg
    return ITkRKPropagatorCfg(flags, name, **kwargs)

def AtlasEnergyLossUpdatorCfg(flags,
                              name='AtlasEnergyLossUpdator',
                              **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault("UseBetheBlochForElectrons", False)
    result.setPrivateTools(CompFactory.Trk.EnergyLossUpdator(name, **kwargs))
    return result

def AtlasMaterialEffectsUpdatorCfg(flags,
                                   name='AtlasMaterialEffectsUpdator',
                                   **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault("EnergyLossUpdator", result.popToolsAndMerge(AtlasEnergyLossUpdatorCfg(flags)))
    result.setPrivateTools(CompFactory.Trk.MaterialEffectsUpdator(name, **kwargs))
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
