#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator 
from AthenaConfiguration.ComponentFactory import CompFactory
from ActsInterop import UnitConstants

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
    options.setdefault('rMax', 200. * UnitConstants.mm)
    options.setdefault('deltaRMin', 5. * UnitConstants.mm)
    options.setdefault('deltaRMax', 60. * UnitConstants.mm)
    options.setdefault('collisionRegionMin', -250. * UnitConstants.mm)
    options.setdefault('collisionRegionMax', 250. * UnitConstants.mm)
    options.setdefault('zMin', -2000 * UnitConstants.mm)
    options.setdefault('zMax', 2000 * UnitConstants.mm)
    options.setdefault('maxSeedsPerSpM', 1)
    options.setdefault('cotThetaMax', 7.40627)
    options.setdefault('sigmaScattering', 5)
    options.setdefault('radLengthPerSeed', 0.5)
    options.setdefault('minPt', 500. * UnitConstants.MeV)
    options.setdefault('impactMax', 3 * UnitConstants.mm)
    options.setdefault('zBinNeighborsTop', [])
    options.setdefault('zBinNeighborsBottom', [])
    
    ActsTrk__ActsSeedingTool = CompFactory.getComp("ActsTrk::ActsSeedingTool")
    acc.setPrivateTools(ActsTrk__ActsSeedingTool(**options))
    return acc
