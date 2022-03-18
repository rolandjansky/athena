#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator 
from AthenaConfiguration.ComponentFactory import CompFactory
from ActsInterop import UnitConstants
import math

# The first two entries (i.e. non ITk) will be removed in the future
# InDetStrip = InDetSCT
ActsSeedToolProperties = {
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


def ActsSeedingToolCfg(ConfigFlags,
                       name: str = 'ActsSeedingTool',
                       configuration: str = ''):
    assert isinstance(name, str)
    assert isinstance(configuration, str)

    # Get geometry and attach to configuration
    if ConfigFlags.Detector.GeometryITk:
        configuration = 'ITk' + configuration
    elif ConfigFlags.Detector.GeometryID:
        configuration = 'InDet' + configuration

    # Only get the properties that have to be modified w.r.t. the default values
    options = ActsSeedToolProperties.get(configuration, None)
    if options is None:
        raise ValueError(f"Requested not-known Acts Seed Tool configuration: `{configuration}'")

    return ActsSeedingToolBaseCfg(ConfigFlags, 
                                  f'{name}_{configuration}', 
                                  **options)


def ActsSeedingToolBaseCfg(ConfigFlags, 
                           name: str = 'ActsSeedingTool', 
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

    ActsTrk__ActsSeedingTool = CompFactory.getComp("ActsTrk::ActsSeedingTool")
    acc.setPrivateTools(ActsTrk__ActsSeedingTool(**options))
    return acc
