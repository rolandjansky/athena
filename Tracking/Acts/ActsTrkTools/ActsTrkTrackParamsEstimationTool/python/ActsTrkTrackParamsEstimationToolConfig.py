#  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from ActsInterop import UnitConstants

def TrackParamsEstimationToolCfg(ConfigFlags,
                                 name: str = "TrackParamsEstimationTool",
                                 **options) -> ComponentAccumulator:
    assert isinstance(name, str)

    acc = ComponentAccumulator()

    options['name'] = name
    options.setdefault('bFieldMin', 0.1 * UnitConstants.T)
    options.setdefault('sigmaLoc0', 25 * UnitConstants.um)
    options.setdefault('sigmaLoc1', 100 * UnitConstants.um)
    options.setdefault('sigmaPhi', 0.02 * UnitConstants.degree)
    options.setdefault('sigmaTheta', 0.02 * UnitConstants.degree)
    options.setdefault('sigmaQOverP', 0.1 / UnitConstants.GeV)
    options.setdefault('sigmaT0', 1400 * UnitConstants.s)
    # eBoundLoc0, eBoundLoc1, eBoundPhi, eBoundTheta, eBoundQOverP, eBoundTime
    options.setdefault('initialVarInflation', [1., 1., 1., 1., 1., 1.])

    acc.setPrivateTools(CompFactory.ActsTrk.TrackParamsEstimationTool(**options))
    return acc
