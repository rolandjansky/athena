# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#==============================================================================
# Provides configs for the tools used for e-gamma decorations used in DAOD
# building
#==============================================================================

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

# PhotonsDirectionTool
def PhotonsDirectionToolCfg(ConfigFlags, name, **kwargs):
    """Configure the PhotonsDirectionTool"""
    acc = ComponentAccumulator()
    PhotonsDirectionTool = CompFactory.DerivationFramework.PhotonsDirectionTool
    acc.addPublicTool(PhotonsDirectionTool(name, **kwargs),
                      primary = True)
    return acc

# E-gamma selection tool wrapper
def EGSelectionToolWrapperCfg(ConfigFlags, name, **kwargs):
    """Configure the E-gamma selection tool wrapper"""
    acc = ComponentAccumulator()
    EGSelectionToolWrapper = CompFactory.DerivationFramework.EGSelectionToolWrapper
    acc.addPublicTool(EGSelectionToolWrapper(name, **kwargs),
                      primary = True)
    return acc

# Electron likelihood tool wrapper
def EGElectronLikelihoodToolWrapperCfg(ConfigFlags, name, **kwargs):
    """Configure the electron likelihood tool wrapper"""
    acc = ComponentAccumulator()
    EGElectronLikelihoodToolWrapper = CompFactory.DerivationFramework.EGElectronLikelihoodToolWrapper
    acc.addPublicTool(EGElectronLikelihoodToolWrapper(name, **kwargs),
                      primary = True)
    return acc

# Photon cleaning tool wrapper
def EGPhotonCleaningWrapperCfg(ConfigFlags, name, **kwargs):
    """Configure the photon cleaning tool wrapper"""
    acc = ComponentAccumulator()
    EGPhotonCleaningWrapper = CompFactory.DerivationFramework.EGPhotonCleaningWrapper
    acc.addPublicTool(EGPhotonCleaningWrapper(name, **kwargs),
                      primary = True)
    return acc

# Crack veto cleaning tool
def EGCrackVetoCleaningToolCfg(ConfigFlags, name, **kwargs):
    """Configure the crack veto cleaning tool"""
    acc = ComponentAccumulator()
    EGCrackVetoCleaningTool = CompFactory.DerivationFramework.EGCrackVetoCleaningTool
    acc.addPublicTool(EGCrackVetoCleaningTool(name, **kwargs),
                      primary = True)
    return acc

# Electron ambiguity tool
def EGElectronAmbiguityToolCfg(ConfigFlags, name, **kwargs):
    """Configure the electron ambiguity tool"""
    acc = ComponentAccumulator()
    EGElectronAmbiguityTool = CompFactory.DerivationFramework.EGElectronAmbiguityTool
    acc.addPublicTool(EGElectronAmbiguityTool(name, **kwargs),
                      primary = True)
    return acc

# Background electron classification tool
def BkgElectronClassificationCfg(ConfigFlags, name, **kwargs):
    """Configure the background electron classification tool""" 
    acc = ComponentAccumulator()
    BkgElectronClassification = CompFactory.DerivationFramework.BkgElectronClassification
    acc.addPublicTool(BkgElectronClassification(name, **kwargs),
                      primary = True)
    return acc

# Standard + LRT electron collection merger
def ElectronMergerCfg(ConfigFlags, name, **kwargs):
    """Configure the track particle merger tool"""
    acc = ComponentAccumulator()
    ElectronMerger = CompFactory.DerivationFramework.ElectronMergerTool
    acc.addPublicTool(ElectronMerger(name, **kwargs),
                      primary = True)
    return acc
