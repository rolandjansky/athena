# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from ActsTrkFinding.ActsSeedingToolConfig import ActsSeedingToolCfg

def ActsSeedingAlgorithmCfg(ConfigFlags, 
                            name: str = 'ActsSeedingAlgorithm', 
                            inputCollection: str = '',
                            outputCollection: str = '',
                            **options):
    assert isinstance(name, str)
    assert isinstance(inputCollection, str)
    assert isinstance(outputCollection, str)

    acc = ComponentAccumulator()

    # tools
    # check if options have already the seed tool defined 
    # and make sure it is not a None
    seedTool = None
    if options.get("SeedTool", None) is None:
        configuration = 'Pixel' if 'Pixel' in inputCollection else 'Strip' 
        seedTool = acc.popToolsAndMerge(ActsSeedingToolCfg(ConfigFlags, 
                                                           name = 'ActsSeedingTool', 
                                                           configuration = configuration))

    options['name'] = f"{name}_{inputCollection}"
    options['InputSpacePoints'] = inputCollection
    options['OutputSeeds'] = outputCollection
    options.setdefault('SeedTool', seedTool)
    options.setdefault('BeamSpotKey', 'BeamSpotData')
    options.setdefault('AtlasFieldCacheCondObj', 'fieldCondObj')

    ActsTrk__ActsSeedingAlgorithm = CompFactory.getComp(f"ActsTrk::{name}")
    acc.addEventAlgo(ActsTrk__ActsSeedingAlgorithm(**options))
    return acc
