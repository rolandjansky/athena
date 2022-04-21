# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def ElectronPhotonVariableCorrectionToolCfg(ConfigFlags, name, **kwargs):
    """Configure the e/gamma variable correction tool"""
    acc = ComponentAccumulator()
    ElectronPhotonVariableCorrectionTool = CompFactory.ElectronPhotonVariableCorrectionTool
    acc.setPrivateTools(ElectronPhotonVariableCorrectionTool(name, **kwargs))
    return acc

