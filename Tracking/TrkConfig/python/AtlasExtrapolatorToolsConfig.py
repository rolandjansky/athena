# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Configuration of tools needed by the Extrapolator

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory


# Navigator used in order to  handle the
# navigation inside the Tracking Geometry
# i.e search for the next TrackingVolume
def AtlasNavigatorCfg(flags,
                      name='AtlasNavigator',
                      **kwargs):
    # get the correct TrackingGeometry setup
    result = ComponentAccumulator()
    if 'TrackingGeometryKey' not in kwargs:
        from TrackingGeometryCondAlg.AtlasTrackingGeometryCondAlgConfig import (
            TrackingGeometryCondAlgCfg)
        acc = TrackingGeometryCondAlgCfg(flags)
        geom_cond_key = acc.getPrimary().TrackingGeometryWriteKey
        result.merge(acc)
        kwargs.setdefault("TrackingGeometryKey", geom_cond_key)
    result.setPrivateTools(CompFactory.Trk.Navigator(name, **kwargs))
    return result


# Configured Energy Loss  uses the TrkUtils parametrization for
# energy loss due to ionization and radiation
def AtlasEnergyLossUpdatorCfg(flags,
                              name='AtlasEnergyLossUpdator',
                              **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault("UseTrkUtils", True)
    kwargs.setdefault("UseBetheBlochForElectrons", False)
    result.setPrivateTools(CompFactory.Trk.EnergyLossUpdator(name, **kwargs))
    return result


# Configured Updators for Layer based material effects
# by default applies Multiple Scattering and
# Energy Loss.
def AtlasMaterialEffectsUpdatorCfg(flags,
                                   name='AtlasMaterialEffectsUpdator',
                                   **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault("EnergyLossUpdator", result.popToolsAndMerge(
        AtlasEnergyLossUpdatorCfg(flags)))
    result.setPrivateTools(
        CompFactory.Trk.MaterialEffectsUpdator(name, **kwargs))
    return result


def AtlasMaterialEffectsUpdatorLandauCfg(flags,
                                         name='AtlasMaterialEffectsUpdatorLandau',
                                         **kwargs):
    # Landau most probable energy loss is not additive
    # special treatment on how we accumulate
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
