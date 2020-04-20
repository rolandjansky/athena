# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
SimpleFastKillerTool = CompFactory.SimpleFastKillerTool
DeadMaterialShowerTool = CompFactory.DeadMaterialShowerTool

def SimpleFastKillerCfg(ConfigFlags, **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault("RegionNames" , ["BeampipeFwdCut"] )
    result.setPrivateTools(SimpleFastKillerTool(name="SimpleFastKiller", **kwargs))
    return result


def DeadMaterialShowerCfg(ConfigFlags, **kwargs):
    result = ComponentAccumulator()
    kwargs.setdefault("RegionNames",        ["DeadMaterial"])
    result.setPrivateTools(DeadMaterialShowerTool(name="DeadMaterialShower", **kwargs))
    return result

