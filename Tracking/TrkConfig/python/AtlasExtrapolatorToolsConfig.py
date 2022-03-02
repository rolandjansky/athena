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

def AtlasRKPropagatorCfg(flags,
                         name='AtlasRungeKuttaPropagator',
                         **kwargs):
    result = ComponentAccumulator()
    AtlasRungeKuttaPropagator = CompFactory.Trk.RungeKuttaPropagator(name, **kwargs)
    result.setPrivateTools(AtlasRungeKuttaPropagator)
    return result

def AtlasSTEP_PropagatorCfg(flags,
                            name='AtlasSTEP_Propagator',
                            **kwargs):
    result = ComponentAccumulator()
    AtlasSTEP_Propagator = CompFactory.Trk.STEP_Propagator(name, **kwargs)
    result.setPrivateTools(AtlasSTEP_Propagator)
    return result

def AtlasNoMatSTEP_PropagatorCfg(flags,
                            name='NoMatSTEP_Propagator',
                            **kwargs):
    kwargs.setdefault("MaterialEffects", False)
    return AtlasSTEP_PropagatorCfg(flags, name, **kwargs)

def MuonSTEP_PropagatorCfg(flags, name='MuonSTEP_Propagator', **kwargs):
    # In the old ConfigDb this was named MuonStraightLinePropagator (!)
    from MagFieldServices.MagFieldServicesConfig import MagneticFieldSvcCfg
    result = ComponentAccumulator()

    acc  = MagneticFieldSvcCfg(flags)
    result.merge(acc)

    kwargs.setdefault("Tolerance", 0.00001 )
    kwargs.setdefault("MaterialEffects", True  )
    kwargs.setdefault("IncludeBgradients", True  )

    propagator = CompFactory.Trk.STEP_Propagator(name=name, **kwargs)
    result.setPrivateTools(propagator)
    return result

def MuonCombinedPropagatorCfg(flags, name='MuonCombinedPropagator', **kwargs ):
    if not flags.Muon.MuonTrigger:
        kwargs.setdefault("AccuracyParameter",   .000001 )
        kwargs.setdefault("IncludeBgradients",   True )
        kwargs.setdefault("MaxHelixStep",        .001 )
        kwargs.setdefault("MaxStraightLineStep", .001 )
    return AtlasRKPropagatorCfg(flags, name, **kwargs)

def InDetPropagatorCfg(flags,
                       name='InDetPropagator',
                       **kwargs):
    result = ComponentAccumulator()

    InDetPropagator = None
    if flags.InDet.Tracking.propagatorType == "STEP":
        InDetPropagator = result.popToolsAndMerge(
            AtlasSTEP_PropagatorCfg(flags, name, **kwargs))
    elif flags.InDet.Tracking.propagatorType == "RungeKutta":
        kwargs.setdefault("AccuracyParameter", 0.0001)
        kwargs.setdefault("MaxStraightLineStep", .004)  # Fixes a failed fit
        InDetPropagator = result.popToolsAndMerge(
            AtlasRKPropagatorCfg(flags, name, **kwargs))

    result.setPrivateTools(InDetPropagator)
    return result


def ITkPropagatorCfg(flags,
                     name='ITkPropagator',
                     **kwargs):
    kwargs.setdefault("AccuracyParameter", 0.0001)
    kwargs.setdefault("MaxStraightLineStep", .004)  # Fixes a failed fit
    return AtlasRKPropagatorCfg(flags, name, **kwargs)

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
