# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Configurable import ConfigurableRun3Behavior
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentAccumulator import CAtoGlobalWrapper
from ActsTrkFinding.ActsSequenceConfiguration import acts_sequence_configuration
from ActsMonitoring.ActsSpacePointMonitoringConfig import ActsSpacePointMonitoringAlgorithmCfg
from ActsMonitoring.ActsSeedMonitoringConfig import ActsSeedMonitoringAlgorithmCfg

@acts_sequence_configuration
def ActsMonitoringCfg(ConfigFlags,
                      space_point_collections: list[str] = None,
                      seed_collections: list[str] = None) -> ComponentAccumulator:
    acc = ComponentAccumulator()

    with ConfigurableRun3Behavior():
        # Space Points
        if space_point_collections is not None:
            for i_collection in space_point_collections:
                acc.merge(ActsSpacePointMonitoringAlgorithmCfg(ConfigFlags,
                                                               name = 'ActsSpacePointMonitoringAlgorithm',
                                                               inputCollection = i_collection))
        # Seeds
        if seed_collections is not None:
            for i_collection in seed_collections:
                acc.merge(ActsSeedMonitoringAlgorithmCfg(ConfigFlags,
                                                         name = 'ActsSeedMonitoringAlgorithm',
                                                         inputCollection = i_collection))
                
    return acc


def ActsMonitoringConfiguration(ConfigFlags,
                                space_point_collections: list[str] = None,
                                seed_collections: list[str] = None):
    options = {}
    options['space_point_collections'] = space_point_collections
    options['seed_collections'] = seed_collections

    with ConfigurableRun3Behavior():
        CAtoGlobalWrapper(ActsMonitoringCfg, ConfigFlags, **options)
    
