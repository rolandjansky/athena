# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# Configuration of STEP_Propagator package
# The STEP (Simultaneous Track and Error Propagation)
# propagates track parameters through the magnetic field.
# Compared to the Runge Kutta Propagator it
# includes material effects in the equation of motion
# and applies corrections to the covariance matrices
# continuously.
# It is particularly suited for the propagation
# of a particle going through a dense block of material.
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from MagFieldServices.MagFieldServicesConfig import AtlasFieldCacheCondAlgCfg

def AtlasSTEP_PropagatorCfg(flags,
                            name='AtlasSTEP_Propagator',
                            **kwargs):
    result = ComponentAccumulator()
    acc = AtlasFieldCacheCondAlgCfg(flags)
    result.merge(acc)

    kwargs.setdefault("MaterialEffects", True) # Default value, set to allow non-default name for compatibility with legacy config
    result.setPrivateTools(CompFactory.Trk.STEP_Propagator(name, **kwargs))
    return result


def AtlasNoMatSTEP_PropagatorCfg(flags,
                                 name='NoMatSTEP_Propagator',
                                 **kwargs):
    kwargs.setdefault("MaterialEffects", False)
    return AtlasSTEP_PropagatorCfg(flags, name, **kwargs)


def fatrasSTEP_PropagatorCfg(flags,
                             name="ISF_FatrasSTEP_Propagator",
                             **kwargs):
    result = ComponentAccumulator()
    acc = AtlasFieldCacheCondAlgCfg(flags)
    result.merge(acc)

    kwargs.setdefault("MomentumCutOff", flags.Sim.Fatras.MomCutOffSec)
    kwargs.setdefault("SimulationMode", True)

    from ISF_FatrasServices.ISF_FatrasConfig import fatrasMaterialUpdatorCfg
    material_updator = result.popToolsAndMerge(fatrasMaterialUpdatorCfg(flags))
    kwargs.setdefault("SimMatEffUpdator", material_updator)

    # the step propagaor
    StepPropagator = CompFactory.Trk.STEP_Propagator
    result.setPrivateTools(StepPropagator(name=name, **kwargs))
    return result
