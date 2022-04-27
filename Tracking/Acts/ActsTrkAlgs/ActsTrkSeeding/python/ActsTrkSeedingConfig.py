# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from ActsTrkSeedingTool.ActsTrkSeedingToolConfig import ActsTrkSeedingToolCfg
from ActsTrkTrackParamsEstimationTool.ActsTrkTrackParamsEstimationToolConfig import TrackParamsEstimationToolCfg
from ActsGeometry.ActsGeometryConfig import ActsTrackingGeometryToolCfg

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
    acc = ComponentAccumulator()
    for i_collection in inputCollections:
        o_collection = f"ActsSeeds_{i_collection}"
        acc.merge(ActsTrkSeedingAlgorithmCfg(ConfigFlags,
                                             inputCollection = i_collection,
                                             outputCollection = o_collection))
        
    return acc
        
def ActsTrackingSequenceFromAthenaCfg(ConfigFlags,
                                      inputCollections: list = [],
                                      **options):
    # prepare entire sequence
    acc = ComponentAccumulator()

    # Need To add additional tool(s)
    # Tracking Geometry Tool
    acc_geo, geoTool = ActsTrackingGeometryToolCfg(ConfigFlags)
    acc.merge(acc_geo)

    # ATLAS Converter Tool
    converterTool = CompFactory.ActsATLASConverterTool(TrackingGeometryTool=geoTool)
    
    # Track Param Estimation Tool
    trackEstimationTool = acc.popToolsAndMerge(TrackParamsEstimationToolCfg(ConfigFlags))

    for i_collection in inputCollections:
        o_collection = 'ITkPixelSeeds' if 'Pixel' in i_collection else 'ITkStripSeeds'

        seedingOptions = { 'TrackingGeometryTool' : geoTool,
                           'ATLASConverterTool' : converterTool,
                           'TrackParamsEstimationTool' : trackEstimationTool,
                           'OutputEstimatedTrackParameters' : 'ITkPixelEstimatedTrackParams' if 'Pixel' in i_collection else 'ITkStripEstimatedTrackParams',
                           'UsePixel' : 'Pixel' in i_collection,
                           'DetectorElements' : 'ITkPixelDetectorElementCollection' if 'Pixel' in i_collection else 'ITkStripDetectorElementCollection'}
        seedingOptions.update(options)
        
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
