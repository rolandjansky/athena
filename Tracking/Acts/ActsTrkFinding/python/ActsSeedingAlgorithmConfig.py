# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from ActsTrkFinding.ActsSeedingToolConfig import ActsSeedingToolCfg

def ActsSeedingAlgorithmCfg(ConfigFlags, 
                            name: str = 'ActsSeedingAlgorithm', 
                            inputCollection: str = '',
                            outputCollection: str = '',
                            **options):
    assert isinstance(inputCollection, str)
    assert isinstance(outputCollection, str)

    acc = ComponentAccumulator()

    # tools
    # check if options have already the seed tool defined 
    # and make sure it is not a None
    seedTool = None
    if options.get("SeedTool", None) is None:
        seedingToolOptions = options.get('seedingToolOptions', {})
        seedTool = acc.popToolsAndMerge(ActsSeedingToolCfg(ConfigFlags, 
                                                           name = 'ActsSeedingTool', 
                                                           inputCollection = inputCollection,
                                                           **seedingToolOptions))

    # Remove keys not required by the Algorithm
    options.pop('seedingToolOptions', None)

    options['name'] = f"{name}_{inputCollection}"
    options.setdefault('SeedTool', seedTool)
    options.setdefault('BeamSpotKey', 'BeamSpotData')
    options.setdefault('AtlasFieldCacheCondObj', 'fieldCondObj')
    options.setdefault('InputSpacePoints', inputCollection)
    options.setdefault('OutputSeeds', outputCollection)

    ActsTrk__ActsSeedingAlgorithm = CompFactory.getComp(f"ActsTrk::{name}")
    acc.addEventAlgo(ActsTrk__ActsSeedingAlgorithm(**options))
    return acc
