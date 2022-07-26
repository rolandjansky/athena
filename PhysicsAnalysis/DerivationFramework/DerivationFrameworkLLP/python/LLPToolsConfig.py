# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#==============================================================================
# Provides configs for the tools used for LLP Derivations
#==============================================================================

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory

# Thinning for VSI tracks
def VSITrackParticleThinningCfg(ConfigFlags, name, **kwargs):
    """Configure the track particle merger tool"""
    acc = ComponentAccumulator()
    VSITrackParticleThinning = CompFactory.DerivationFramework.VSITrackParticleThinning
    acc.addPublicTool(VSITrackParticleThinning(name, **kwargs),
                      primary = True)
    return acc

# Thinning for Std tracks in jets
def JetTrackParticleThinningCfg(ConfigFlags, name, **kwargs):
    """Configure the track particle merger tool"""
    acc = ComponentAccumulator()
    JetTrackParticleThinning = CompFactory.DerivationFramework.JetTrackParticleThinning
    acc.addPublicTool(JetTrackParticleThinning(name, **kwargs),
                      primary = True)
    return acc

# Thinning for LRT tracks in jets
def JetLargeD0TrackParticleThinningCfg(ConfigFlags, name, **kwargs):
    """Configure the track particle merger tool"""
    acc = ComponentAccumulator()
    JetLargeD0TrackParticleThinning = CompFactory.DerivationFramework.JetLargeD0TrackParticleThinning
    acc.addPublicTool(JetLargeD0TrackParticleThinning(name, **kwargs),
                      primary = True)
    return acc


def LLP1TriggerSkimmingToolCfg(ConfigFlags, name, **kwargs):

    from TriggerMenuMT.TriggerAPI.TriggerAPI import TriggerAPI
    from TriggerMenuMT.TriggerAPI.TriggerEnums import TriggerPeriod, TriggerType

    allperiods = TriggerPeriod.y2015 | TriggerPeriod.y2016 | TriggerPeriod.y2017 | TriggerPeriod.y2018 | TriggerPeriod.future2e34
    trig_el  = TriggerAPI.getLowestUnprescaledAnyPeriod(allperiods, triggerType=TriggerType.el,  livefraction=0.8)
    trig_mu  = TriggerAPI.getLowestUnprescaledAnyPeriod(allperiods, triggerType=TriggerType.mu,  livefraction=0.8)
    trig_g   = TriggerAPI.getLowestUnprescaledAnyPeriod(allperiods, triggerType=TriggerType.g,   livefraction=0.8)
    trig_elmu = TriggerAPI.getLowestUnprescaledAnyPeriod(allperiods, triggerType=TriggerType.el, additionalTriggerType=TriggerType.mu,  livefraction=0.8)
    trig_mug = TriggerAPI.getLowestUnprescaledAnyPeriod(allperiods, triggerType=TriggerType.mu, additionalTriggerType=TriggerType.g,  livefraction=0.8)

    triggers = trig_el + trig_mu + trig_g + trig_elmu + trig_mug
    #remove duplicates
    triggers = sorted(list(set(triggers)))

    acc = ComponentAccumulator()
    TriggerSkimmingTool = CompFactory.DerivationFramework.TriggerSkimmingTool
    acc.addPublicTool(TriggerSkimmingTool(name, 
                                          TriggerListAND = [],
                                          TriggerListOR  = triggers,
                                          **kwargs),
                                          primary = True)
    return acc


def LLP1TriggerMatchingToolRun2Cfg(ConfigFlags, name, **kwargs):
    """Configure the common trigger matching for run 2 DAODs"""

    triggerList = kwargs['TriggerList']
    outputContainerPrefix = kwargs['OutputContainerPrefix']
    
    kwargs.setdefault('InputElectrons', 'LRTElectrons')
    kwargs.setdefault('InputMuons', 'MuonsLRT')
    kwargs.setdefault('DRThreshold', None) 

    acc = ComponentAccumulator()

    # Create trigger matching decorations
    from DerivationFrameworkTrigger.TriggerMatchingToolConfig import TriggerMatchingToolCfg
    if kwargs['DRThreshold'] is None:
        PhysCommonTriggerMatchingTool = acc.getPrimaryAndMerge(TriggerMatchingToolCfg(
            ConfigFlags,
            name=name,
            ChainNames = triggerList,
            OutputContainerPrefix = outputContainerPrefix,
            InputElectrons = kwargs['InputElectrons'],
            InputMuons =  kwargs['InputMuons'])) 
    else:
        PhysCommonTriggerMatchingTool = acc.getPrimaryAndMerge(TriggerMatchingToolCfg(
            ConfigFlags,
            name=name,
            ChainNames = triggerList,
            OutputContainerPrefix = outputContainerPrefix,  
            DRThreshold = kwargs['DRThreshold'],
            InputElectrons = kwargs['InputElectrons'],
            InputMuons =  kwargs['InputMuons'])) 
    CommonAugmentation = CompFactory.DerivationFramework.CommonAugmentation
    acc.addEventAlgo(CommonAugmentation(f"{outputContainerPrefix}TriggerMatchingKernel",
                                        AugmentationTools=[PhysCommonTriggerMatchingTool]))
    return(acc)
