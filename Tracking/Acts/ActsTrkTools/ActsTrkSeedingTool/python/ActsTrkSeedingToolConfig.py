#  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from ActsInterop import UnitConstants
import math

# The first two entries (i.e. non ITk) will be removed in the future
# InDetStrip = InDetSCT
SeedToolProperties = {
    'InDetPixel' : {
    },
    'InDetStrip' : {
        'rMax' : 600 * UnitConstants.mm,
        'deltaRMax' : 400 * UnitConstants.mm
    },
    'ITkPixel' : {
        'rMax' : 320 * UnitConstants.mm,
        'deltaRMax' : 120 * UnitConstants.mm
    },
    'ITkStrip' : {
        'rMax' : 1100 * UnitConstants.mm,
        'deltaRMax' : 500 * UnitConstants.mm
    }
}


def ActsTrkSeedingToolCfg(ConfigFlags,
                          name: str = 'ActsTrkSeedingTool',
                          configuration: str = ''):
    assert isinstance(name, str)
    assert isinstance(configuration, str)

    # Get geometry and attach to configuration
    if ConfigFlags.Detector.GeometryITk:
        configuration = 'ITk' + configuration
    elif ConfigFlags.Detector.GeometryID:
        configuration = 'InDet' + configuration

    # Only get the properties that have to be modified w.r.t. the default values
    options = SeedToolProperties.get(configuration, None)
    if options is None:
        raise ValueError(f"Requested not-known Acts Seed Tool configuration: `{configuration}'")

    return ActsTrkSeedingToolBaseCfg(ConfigFlags,
                                     f'{name}_{configuration}',
                                     **options)


def ActsTrkSeedingToolBaseCfg(ConfigFlags,
                              name: str = 'ActsTrkSeedingTool',
                              **options):
    acc = ComponentAccumulator()

    options['name'] = name

    # Used by SeedfinderConfig
    options.setdefault('minPt', 500. * UnitConstants.MeV) # Also used by SpacePointGridConfig
    options.setdefault('cotThetaMax', 7.40627) # Also used by SpacePointGridConfig
    options.setdefault('deltaRMin', 5. * UnitConstants.mm) # Also used by SeedFilterConfig
    options.setdefault('deltaRMax', 60. * UnitConstants.mm) # Also used by SpacePointGridConfig
    options.setdefault('deltaRMinTopSP', 5. * UnitConstants.mm)
    options.setdefault('deltaRMaxTopSP', 60. * UnitConstants.mm)
    options.setdefault('deltaRMinBottomSP', 5. * UnitConstants.mm)
    options.setdefault('deltaRMaxBottomSP', 60. * UnitConstants.mm)
    options.setdefault('impactMax', 3 * UnitConstants.mm) # Also used by SpacePointGridConfig
    options.setdefault('sigmaScattering', 5)
    options.setdefault('maxPtScattering', 10 * UnitConstants.mm)
    options.setdefault('maxSeedsPerSpM', 5) # Also used by SeedFilterConfig
    options.setdefault('collisionRegionMin', -250. * UnitConstants.mm)
    options.setdefault('collisionRegionMax', 250. * UnitConstants.mm)
    options.setdefault('phiMin', -math.pi)
    options.setdefault('phiMax', math.pi)
    options.setdefault('zMin', -2000 * UnitConstants.mm) # Also used by SpacePointGridConfig
    options.setdefault('zMax', 2000 * UnitConstants.mm) # Also used by SpacePointGridConfig
    options.setdefault('rMax', 200. * UnitConstants.mm) # Also used by SpacePointGridConfig
    options.setdefault('rMin', 33 * UnitConstants.mm)
    options.setdefault('radLengthPerSeed', 0.5)
    options.setdefault('zAlign', 0 * UnitConstants.mm)
    options.setdefault('rAlign', 0 * UnitConstants.mm)
    options.setdefault('sigmaError', 5)
    options.setdefault('useVariableMiddleSPRange', False)
    options.setdefault('deltaRMiddleSPRange', 10) # TOOD: Add ACTS units (currently not supported for this prop)
    options.setdefault('seedConfirmation', False) # no default for detailed conf, since confirmation is disabled by default
    options.setdefault('enableCutsForSortedSP', False)

    # Used by SeedFilterConfig
    options.setdefault('deltaInvHelixDiameter', 0.00003 * 1. / UnitConstants.mm)
    options.setdefault('impactWeightFactor', 1.)
    options.setdefault('compatSeedWeight', 200.)
    options.setdefault('compatSeedLimit', 2)

    # Used by SpacePointGridConfig
    options.setdefault('numPhiNeighbors', 1)
    options.setdefault('zBinEdges', [])

    # Used by others
    options.setdefault('zBinNeighborsTop', [])
    options.setdefault('zBinNeighborsBottom', [])

    ActsTrk__SeedingTool = CompFactory.getComp("ActsTrk::SeedingTool")
    acc.setPrivateTools(ActsTrk__SeedingTool(**options))
    return acc


def  ActsTrkSiSpacePointsSeedMakerCfg(ConfigFlags,
                                      name: str = 'ActsTrkSiSpacePointsSeedMaker',
                                      InputCollections: list = None,
                                      **options) -> ComponentAccumulator:
    assert isinstance(name, str)
    if InputCollections is not None:
        assert isinstance(InputCollections, list)

    acc = ComponentAccumulator()

    options['name'] = name

    # Main properties
    options.setdefault('SpacePointsPixelName', 'ITkPixelSpacePoints')
    options.setdefault('SpacePointsStripName', 'ITkStripSpacePoints')
    options.setdefault('SpacePointsOverlapName', 'ITkOverlapSpacePoints')
    options.setdefault('usePixel', True)
    options.setdefault('useStrip', True)
    options.setdefault('useOverlapSpCollection', True)

    if len(InputCollections) > 0 and ConfigFlags.ITk.Tracking.ActivePass.usePrdAssociationTool:
        # not all classes have that property !!!
        options.setdefault('PRDtoTrackMap', 'ITkPRDtoTrackMap'+ ConfigFlags.ITk.Tracking.ActivePass.extension)

    # Acts Seed Tools
    # Do not overwrite if already present in `options`
    seedTool_pixel = None
    if 'SeedToolPixel' not in options:
        seedTool_pixel = acc.popToolsAndMerge(ActsTrkSeedingToolCfg(ConfigFlags,
                                                                    configuration = 'Pixel'))

    seedTool_strip = None
    if 'SeedToolStrip' not in options:
        seedTool_strip = acc.popToolsAndMerge(ActsTrkSeedingToolCfg(ConfigFlags,
                                                                    configuration = 'Strip'))

    options.setdefault('SeedToolPixel', seedTool_pixel)
    options.setdefault('SeedToolStrip', seedTool_strip)

    # Validation
    if ConfigFlags.ITk.Tracking.writeSeedValNtuple:
        options.setdefault('WriteNtuple', True)
        HistService = CompFactory.THistSvc(Output = ["valNtuples DATAFILE='SeedMakerValidation.root' OPT='RECREATE'"])
        acc.addService(HistService)

    acc.setPrivateTools(CompFactory.ActsTrk.SiSpacePointsSeedMaker(**options))
    return acc
