# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

#====================================================================
# TriggerMatchingCommonConfig.py
# This defines the common trigger matching shared by PHYS and PHYSLITE
# Using it avoids name clashes when running in trains
# In principle it can also be used by other formats who want to take
# advantage of PHYS/PHYSLITE containers
# Component accumulator version
#====================================================================
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from PathResolver import PathResolver

def read_trig_list_file(fname):
   """Read a text file containing a list of triggers
   
   Returns the list of triggers held in the file
   """
   triggers = []
   with open(PathResolver.FindCalibFile(fname)) as fp:
      for line in fp:
         line = line.strip()
         if line == "" or line.startswith("#"):
            continue
         triggers.append(line)
   return triggers


def AddRun2TriggerMatchingToSlimmingHelper(**kwargs):
    """Adds the trigger matching info to the slimming helper"""

    slimmingHelper = kwargs['SlimmingHelper']
    triggerList = kwargs['TriggerList']
    containerPrefix = kwargs['OutputContainerPrefix']

    outputContainers = [
        "{0}{1}".format(containerPrefix, chain).replace(".", "_")
        for chain in triggerList
    ]

    slimmingHelper.AllVariables += outputContainers
    for cont in outputContainers:
        slimmingHelper.AppendToDictionary.update(
            {
                cont: "xAOD::TrigCompositeContainer",
                cont + "Aux": "AOD::AuxContainerBase!",
            }
        )


def TriggerMatchingCommonRun2Cfg(ConfigFlags, name, **kwargs):
    """Configure the common trigger matching for run 2 DAODs"""

    triggerList = kwargs['TriggerList']
    outputContainerPrefix = kwargs['OutputContainerPrefix']
    kwargs.setdefault('DRThreshold', None) 

    acc = ComponentAccumulator()

    # Create trigger matching decorations
    from DerivationFrameworkTrigger.TriggerMatchingToolConfig import TriggerMatchingToolCfg
    if kwargs['DRThreshold'] is None:
        PhysCommonTriggerMatchingTool = acc.getPrimaryAndMerge(TriggerMatchingToolCfg(
            ConfigFlags,
            name=name,
            ChainNames = triggerList,
            OutputContainerPrefix = outputContainerPrefix)) 
    else:
        PhysCommonTriggerMatchingTool = acc.getPrimaryAndMerge(TriggerMatchingToolCfg(
            ConfigFlags,
            name=name,
            ChainNames = triggerList,
            OutputContainerPrefix = outputContainerPrefix,  
            DRThreshold = kwargs['DRThreshold']))
    CommonAugmentation = CompFactory.DerivationFramework.CommonAugmentation
    acc.addEventAlgo(CommonAugmentation(f"{outputContainerPrefix}TriggerMatchingKernel",
                                        AugmentationTools=[PhysCommonTriggerMatchingTool]))
    return(acc)


def TriggerMatchingCommonRun3Cfg(ConfigFlags, **kwargs):
    """Configure the common trigger matching for run 3 DAODs"""
    
    if ConfigFlags.Trigger.EDMVersion != 3:
        raise ValueError('This configuration can only be used for Run 3 trigger navigation')

    triggerList = kwargs['TriggerList']

    acc = ComponentAccumulator()

    # Run 3 trigger navigation slimming proposal for in-DAOD trigger matching.
    from TrigNavSlimmingMT.TrigNavSlimmingMTConfig import TrigNavSlimmingMTDerivationCfg
    acc.merge(TrigNavSlimmingMTDerivationCfg(ConfigFlags,chainsFilter=triggerList))

    return(acc)


