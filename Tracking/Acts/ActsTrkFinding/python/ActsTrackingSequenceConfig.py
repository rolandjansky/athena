# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Configurable import ConfigurableRun3Behavior
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentAccumulator import CAtoGlobalWrapper
from ActsTrkFinding.ActsSeedingAlgorithmConfig import ActsSeedingAlgorithmCfg
from ActsTrkFinding.ActsSequenceConfiguration import acts_sequence_configuration

@acts_sequence_configuration
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

    return acc

def ActsTrackingSequenceToAthena(ConfigFlags,
                                 inputCollections: list = []):
    options = {}
    options['inputCollections'] = inputCollections

    with ConfigurableRun3Behavior():
        CAtoGlobalWrapper(ActsTrackingSequenceCfg, ConfigFlags, **options)
    

