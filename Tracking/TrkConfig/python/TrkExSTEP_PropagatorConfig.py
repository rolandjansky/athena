# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
# Configuration of TrkExSTEP_Propagator package
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def AtlasSTEP_PropagatorCfg(flags, name='AtlasSTEP_Propagator', **kwargs):
    result = ComponentAccumulator()
    AtlasSTEP_Propagator = CompFactory.Trk.STEP_Propagator(name, **kwargs)
    result.setPrivateTools(AtlasSTEP_Propagator)
    return result

def AtlasNoMatSTEP_PropagatorCfg(flags, name='NoMatSTEP_Propagator', **kwargs):
    kwargs.setdefault("MaterialEffects", False)
    return AtlasSTEP_PropagatorCfg(flags, name, **kwargs)

def fatrasSTEP_PropagatorCfg(flags, name="ISF_FatrasSTEP_Propagator", **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault("MomentumCutOff", flags.Sim.Fatras.MomCutOffSec)
    kwargs.setdefault("SimulationMode", True)

    from ISF_FatrasServices.ISF_FatrasConfig import fatrasMaterialUpdatorCfg
    material_updator = result.popToolsAndMerge(fatrasMaterialUpdatorCfg(flags))
    kwargs.setdefault("SimMatEffUpdator", material_updator)

    # the step propagaor
    StepPropagator = CompFactory.Trk.STEP_Propagator
    result.setPrivateTools(StepPropagator(name=name, **kwargs))
    return result
