# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#==============================================================================
# Provides configs for the LRT electron tools
#==============================================================================

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

# Standard + LRT electron collection merger
def ElectronMergerCfg(ConfigFlags, name, **kwargs):
    """Configure the standard and LRT electron merger tool"""
    acc = ComponentAccumulator()
    ElectronMerger = CompFactory.LRTElectronAnalysisTools.ElectronMerger
    acc.addPublicTool(ElectronMerger(name, **kwargs),
                      primary = True)
    return acc



# Standard + LRT electron collection merger
def ElectronLRTOverlapRemovalToolCfg(ConfigFlags, name, **kwargs):
    """Configure the standard and LRT electron overlap removal tool"""
    acc = ComponentAccumulator()
    ElectronLRTOverlapRemovalTool = CompFactory.LRTElectronAnalysisTools.ElectronLRTOverlapRemovalTool
    acc.addPublicTool(ElectronLRTOverlapRemovalTool(name, **kwargs),
                      primary = True)
    return acc