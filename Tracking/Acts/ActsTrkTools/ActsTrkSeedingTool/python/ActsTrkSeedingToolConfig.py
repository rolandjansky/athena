#  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from ActsInterop import UnitConstants

def ActsTrkITkPixelSeedingToolCfg(ConfigFlags):
    acc = ComponentAccumulator()
    ## For ITkPixel, use default values for ActsTrk::SeedingTool
    acc.setPrivateTools(CompFactory.ActsTrk.SeedingTool(name = "ActsSeedingTool_ITkPixel"))
    return acc

def ActsTrkITkStripSeedingToolCfg(ConfigFlags):
    acc = ComponentAccumulator()
    ## For ITkStrip, change properties that have to be modified w.r.t. the default values
    kwargs = {}
    # For SpacePointGridConfig
    kwargs.setdefault("gridRMax" , 1000. * UnitConstants.mm)
    kwargs.setdefault("deltaRMax" , 600. * UnitConstants.mm)
    kwargs.setdefault("impactMax" , 20. * UnitConstants.mm)
    # For SeedfinderConfig
    kwargs.setdefault("rMax" , 1200. * UnitConstants.mm)
    kwargs.setdefault("deltaRMinTopSP" , 20. * UnitConstants.mm)
    kwargs.setdefault("deltaRMaxTopSP" , 300. * UnitConstants.mm)
    kwargs.setdefault("deltaRMinBottomSP" , 20. * UnitConstants.mm)
    kwargs.setdefault("deltaRMaxBottomSP" , 300. * UnitConstants.mm)
    kwargs.setdefault("deltaZMax" , 900. * UnitConstants.mm)
    kwargs.setdefault("interactionPointCut" , False)
    kwargs.setdefault("arithmeticAverageCotTheta" , True)
    kwargs.setdefault("zBinsCustomLooping" , [6, 7, 5, 8, 4, 9, 3, 10, 2, 11, 1])
    kwargs.setdefault("skipPreviousTopSP" , False)
    kwargs.setdefault("deltaRMiddleMinSPRange" , 30 * UnitConstants.mm)
    kwargs.setdefault("deltaRMiddleMaxSPRange" , 150 * UnitConstants.mm)
    kwargs.setdefault("useDetailedDoubleMeasurementInfo" , True)
    # For SeedFilterConfig
    kwargs.setdefault("useDeltaRorTopRadius" , False)
    kwargs.setdefault("seedConfirmationInFilter" , False)
    kwargs.setdefault("impactWeightFactor" , 1.)
    kwargs.setdefault("compatSeedLimit" , 4)
    kwargs.setdefault("numSeedIncrement" , 1.)
    kwargs.setdefault("seedWeightIncrement" , 10100.)
    kwargs.setdefault("maxSeedsPerSpMConf" , 10e6)
    kwargs.setdefault("maxQualitySeedsPerSpMConf" , 10e6)
    # For seeding algorithm
    kwargs.setdefault("zBinNeighborsBottom" , [(0,1),(0,1),(0,1),(0,2),(0,1),(0,0),(-1,0),(-2,0),(-1,0),(-1,0),(-1,0)])

    acc.setPrivateTools(CompFactory.ActsTrk.SeedingTool(name = "ActsSeedingTool_ITkStrip", **kwargs))
    return acc

def  ActsTrkSiSpacePointsSeedMakerCfg(ConfigFlags,
                                      name: str = 'ActsTrkSiSpacePointsSeedMaker',
                                      InputCollections: list = None,
                                      **kwargs) -> ComponentAccumulator:
    assert isinstance(name, str)
    if InputCollections is not None:
        assert isinstance(InputCollections, list)

    acc = ComponentAccumulator()

    kwargs['name'] = name

    # Main properties
    kwargs.setdefault('SpacePointsPixelName', 'ITkPixelSpacePoints')
    kwargs.setdefault('SpacePointsStripName', 'ITkStripSpacePoints')
    kwargs.setdefault('SpacePointsOverlapName', 'ITkOverlapSpacePoints')
    kwargs.setdefault('usePixel', ConfigFlags.ITk.Tracking.ActivePass.useITkPixel and ConfigFlags.ITk.Tracking.ActivePass.useITkPixelSeeding)
    kwargs.setdefault('useStrip', ConfigFlags.ITk.Tracking.ActivePass.useITkStrip and ConfigFlags.ITk.Tracking.ActivePass.useITkStripSeeding)
    kwargs.setdefault('useOverlapSpCollection', ConfigFlags.ITk.Tracking.ActivePass.useITkStrip and ConfigFlags.ITk.Tracking.ActivePass.useITkStripSeeding)
    kwargs.setdefault('doSpacePointConversion', not (ConfigFlags.ITk.Tracking.convertInDetClusters and ConfigFlags.ITk.Tracking.produceNewSpacePointContainer))
    kwargs.setdefault('ActsTrkSpacePointsPixelName'    , "ITkPixelSpacePoints")
    kwargs.setdefault('ActsTrkSpacePointsStripName'    , "ITkStripSpacePoints")
    kwargs.setdefault('ActsTrkSpacePointsOverlapName'  , "ITkStripOverlapSpacePoints")
    kwargs.setdefault('PixelClusterContainerKey', "ITkPixelClusters")
    kwargs.setdefault('StripClusterContainerKey', "ITkStripClusters")

    if len(InputCollections) > 0 and ConfigFlags.ITk.Tracking.ActivePass.usePrdAssociationTool:
        # not all classes have that property !!!
        kwargs.setdefault('PRDtoTrackMap', 'ITkPRDtoTrackMap'+ ConfigFlags.ITk.Tracking.ActivePass.extension)

    # Acts Seed Tools
    # Do not overwrite if already present in `kwargs`
    seedTool_pixel = None
    if 'SeedToolPixel' not in kwargs:
        seedTool_pixel = acc.popToolsAndMerge(ActsTrkITkPixelSeedingToolCfg(ConfigFlags))

    seedTool_strip = None
    if 'SeedToolStrip' not in kwargs:
        seedTool_strip = acc.popToolsAndMerge(ActsTrkITkStripSeedingToolCfg(ConfigFlags))

    kwargs.setdefault('SeedToolPixel', seedTool_pixel)
    kwargs.setdefault('SeedToolStrip', seedTool_strip)

    # Validation
    if ConfigFlags.ITk.Tracking.writeSeedValNtuple:
        kwargs.setdefault('WriteNtuple', True)
        HistService = CompFactory.THistSvc(Output = ["valNtuples DATAFILE='SeedMakerValidation.root' OPT='RECREATE'"])
        acc.addService(HistService)

    acc.setPrivateTools(CompFactory.ActsTrk.SiSpacePointsSeedMaker(**kwargs))
    return acc
