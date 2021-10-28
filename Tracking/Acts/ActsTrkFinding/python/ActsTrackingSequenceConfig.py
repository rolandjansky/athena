# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Configurable import ConfigurableRun3Behavior
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentAccumulator import CAtoGlobalWrapper
from ActsTrkFinding.ActsSeedingAlgorithmConfig import ActsSeedingAlgorithmCfg

def ActsTrackingSequenceCfg(ConfigFlags, 
                            inputCollections: list = []):
    # prepare entire sequence
    with ConfigurableRun3Behavior():
        acc = ComponentAccumulator()
        for i_collection in inputCollections:
            o_collection = f"ActsSeeds_{i_collection}"
            acc.merge(ActsSeedingAlgorithmCfg(ConfigFlags, 
                                              name = 'ActsSeedingAlgorithm', 
                                              inputCollection = i_collection,
                                              outputCollection = o_collection))
            
    # Set OutputLevel can be set with the following piece of code
    # This will set the Outputl Level of all the algorithms and Tools to
    # a defined value
    #
    # from AthenaCommon.Constants import DEBUG
    # for el in acc._allSequences:
    #     for member in el.Members:
    #         member.OutputLevel = DEBUG
            
    # the following is needed to reliably determine whether we're really being steered from an old-style job option
    # assume we're running CPython
    import inspect
    stack = inspect.stack()
    if len(stack) >= 2 and stack[1].function == 'CAtoGlobalWrapper':
        for el in acc._allSequences:
            el.name = "TopAlg"

    return acc

def ActsTrackingSequenceToAthena(ConfigFlags,
                                 inputCollections: list = []):
    options = {}
    options['inputCollections'] = inputCollections

    with ConfigurableRun3Behavior():
        CAtoGlobalWrapper(ActsTrackingSequenceCfg, ConfigFlags, **options)
    

