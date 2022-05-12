# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration


from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

def AsgElectronSelectorToolCfg(ConfigFlags, name, **kwargs):
    """Configure the AsgElectronSelectorTool with the quality cuts
         and allow for (re-)setting of all provided cuts."""

    acc = ComponentAccumulator()
    AsgElectronSelectorTool = CompFactory.AsgElectronSelectorTool
    acc.setPrivateTools(AsgElectronSelectorTool(name, **kwargs))
    return acc

