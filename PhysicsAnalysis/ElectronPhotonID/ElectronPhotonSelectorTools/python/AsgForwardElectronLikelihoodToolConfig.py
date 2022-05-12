# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

def AsgForwardElectronLikelihoodToolCfg(ConfigFlags, name, **kwargs):
    """Configure the electron charge ID selector tool"""
    acc = ComponentAccumulator()
    AsgForwardElectronLikelihoodTool = CompFactory.AsgForwardElectronLikelihoodTool
    acc.setPrivateTools(AsgForwardElectronLikelihoodTool(name, **kwargs))
    return acc

