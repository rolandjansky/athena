# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Configurable import ConfigurableRun3Behavior
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentAccumulator import CAtoGlobalWrapper
from ActsTrkFinding.ActsSeedingAlgorithmConfig import ActsSeedingAlgorithmCfg
from ActsTrkFinding.ActsSequenceConfiguration import acts_sequence_configuration

@acts_sequence_configuration
def ActsTrackingSequenceFromAthenaCfg(ConfigFlags, 
                                      inputCollections: list = [],
                                      **kwargs):
    # prepare entire sequence
    with ConfigurableRun3Behavior():
        acc = ComponentAccumulator()
        for i_collection in inputCollections:
            o_collection = f"ActsSeeds_{i_collection}"

            # Additional write handle keys used by the Algorithm. This stores Acts space points and data 
            # for later use (i.e. performance-check algorithm)
            seedingOptions = { 'OutputSpacePoints' : f'ActsSpacePoint_{i_collection}',
                               'OutputSpacePointData' : f'ActsSpacePointData_{i_collection}' }
            seedingOptions.update(kwargs)

            acc.merge(ActsSeedingAlgorithmCfg(ConfigFlags,
                                              name = 'ActsSeedingFromAthenaAlgorithm',
                                              inputCollection = i_collection,
                                              outputCollection = o_collection,
                                              **seedingOptions))

    return acc

def ActsTrackingSequenceFromAthenaToAthena(ConfigFlags,
                                           inputCollections: list = []):
    options = {}
    options['inputCollections'] = inputCollections

    with ConfigurableRun3Behavior():
        CAtoGlobalWrapper(ActsTrackingSequenceFromAthenaCfg, ConfigFlags, **options)
    
def ActsTrackingSequenceITkPostInclude(flags):
    acc = ComponentAccumulator()
    acc.merge(ActsTrackingSequenceFromAthenaCfg(flags,
                                                inputCollections = [
                                                    "ITkPixelSpacePoints",
                                                    "ITkStripSpacePoints"
                                                ]))
    return acc
