#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator 
from AthenaConfiguration.ComponentFactory import CompFactory
from ActsInterop import UnitConstants

def ActsSeedingToolCfg(ConfigFlags, 
                       name: str = 'ActsSeedingTool', 
                       inputCollection: str = "",
                       **options):
    assert isinstance(name, str)
    assert isinstance(inputCollection, str)

    acc = ComponentAccumulator()

    options['name'] = f"{name}_{inputCollection}"
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
    
    ActsTrk__ActsSeedingTool = CompFactory.getComp(f"ActsTrk::{name}")
    acc.setPrivateTools(ActsTrk__ActsSeedingTool(**options))
    return acc
