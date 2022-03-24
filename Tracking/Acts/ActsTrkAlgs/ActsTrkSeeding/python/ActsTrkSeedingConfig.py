# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaCommon.Configurable import ConfigurableRun3Behavior
from ActsTrkSeedingTool.ActsTrkSeedingToolConfig import ActsTrkSeedingToolCfg

def ActsTrkSeedingAlgorithmCfg(ConfigFlags,
                               name: str = 'SeedingAlgorithm',
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
        seedTool = acc.popToolsAndMerge(ActsTrkSeedingToolCfg(ConfigFlags,
                                                              name = 'SeedingTool',
                                                              configuration = configuration))

    options['name'] = f"{name}_{inputCollection}"
    options['InputSpacePoints'] = inputCollection
    options['OutputSeeds'] = outputCollection
    options.setdefault('SeedTool', seedTool)
    options.setdefault('BeamSpotKey', 'BeamSpotData')
    options.setdefault('AtlasFieldCacheCondObj', 'fieldCondObj')

    ActsTrk__SeedingAlgorithm = CompFactory.getComp(f"ActsTrk::{name}")
    acc.addEventAlgo(ActsTrk__SeedingAlgorithm(**options))
    return acc

def ActsTrackingSequenceCfg(ConfigFlags,
                            inputCollections: list = []):
    # prepare entire sequence
    with ConfigurableRun3Behavior():
        acc = ComponentAccumulator()
        for i_collection in inputCollections:
            o_collection = f"ActsSeeds_{i_collection}"
            acc.merge(ActsTrkSeedingAlgorithmCfg(ConfigFlags,
                                                 name = 'SeedingAlgorithm',
                                                 inputCollection = i_collection,
                                                 outputCollection = o_collection))

    return acc

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

            acc.merge(ActsTrkSeedingAlgorithmCfg(ConfigFlags,
                                                 name = 'SeedingFromAthenaAlgorithm',
                                                 inputCollection = i_collection,
                                                 outputCollection = o_collection,
                                                 **seedingOptions))

    return acc

def ActsTrackingSequenceITkPostInclude(flags):
    acc = ComponentAccumulator()
    acc.merge(ActsTrackingSequenceFromAthenaCfg(flags,
                                                inputCollections = [
                                                    "ITkPixelSpacePoints",
                                                    "ITkStripSpacePoints"
                                                ]))
    return acc
