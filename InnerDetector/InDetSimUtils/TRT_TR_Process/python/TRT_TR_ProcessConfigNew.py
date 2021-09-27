# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

def TRTPhysicsToolCfg(flags, name="TRTPhysicsTool", **kwargs):
    result = ComponentAccumulator()
    #Add any future configuration here
    result.setPrivateTools(CompFactory.TRTPhysicsTool(name, **kwargs))
    return result
