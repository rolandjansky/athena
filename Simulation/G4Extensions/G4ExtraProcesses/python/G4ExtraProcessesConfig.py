# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

def G4EMProcessesPhysicsToolCfg(flags, name="G4EMProcessesPhysicsTool", **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault("ParticleList", [5132]) # In future this should be made configurable
    result.setPrivateTools(CompFactory.G4EMProcessesPhysicsTool(name, **kwargs))
    return result
