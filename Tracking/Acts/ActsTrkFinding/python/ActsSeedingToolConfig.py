#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator 
from AthenaConfiguration.ComponentFactory import CompFactory
from ActsInterop import UnitConstants


ActsSeedToolProperties = {
    'PixelSpacePoints' : {
    },
    'SCT_SpacePoints' : {
        'rMax' : 600 * UnitConstants.mm,
        'deltaRMax' : 400 * UnitConstants.mm
    },
    'ITkPixelSpacePoints' : {
        'rMax' : 320 * UnitConstants.mm,
        'deltaRMax' : 120 * UnitConstants.mm
    },
    'ITkStripSpacePoints' : {
        'rMax' : 1100 * UnitConstants.mm,
        'deltaRMax' : 500 * UnitConstants.mm
    }
}


def ActsSeedingToolCfg(ConfigFlags,
                       name: str = 'ActsSeedingTool',
                       inputCollection: str = ''):
    assert isinstance(name, str)
    assert isinstance(inputCollection, str)

    # Only get the properties that have to be modified w.r.t. the default values
    options = ActsSeedToolProperties.get(inputCollection, None)
    if options is None:
        raise ValueError(f"Acts Seed Tool has no-known configuration for the `{inputCollection}` input collection")

    return ActsSeedingToolBaseCfg(ConfigFlags, 
                                  f'{name}_{inputCollection}', 
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
    
    ActsTrk__ActsSeedingTool = CompFactory.getComp("ActsTrk::ActsSeedingTool")
    acc.setPrivateTools(ActsTrk__ActsSeedingTool(**options))
    return acc
