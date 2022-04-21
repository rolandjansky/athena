# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def AsgElectronChargeIDSelectorToolCfg(ConfigFlags, name, **kwargs):
    """Configure the electron charge ID selector tool"""
    acc = ComponentAccumulator()
    AsgElectronChargeIDSelectorTool = CompFactory.AsgElectronChargeIDSelectorTool
    acc.setPrivateTools(AsgElectronChargeIDSelectorTool(name, **kwargs))
    return acc

