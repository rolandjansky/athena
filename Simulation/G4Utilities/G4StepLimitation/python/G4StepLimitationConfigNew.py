# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

def G4StepLimitationToolCfg(flags, name="G4StepLimitationTool", **kwargs):
    result = ComponentAccumulator()
    #Add any future configuration here
    result.setPrivateTools(CompFactory.G4StepLimitationTool(name, **kwargs))
    return result
