# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

# File: CaloRec/python/CaloBCIDLumiCondAlgSCConfig.py
# Purpose: Configure CaloBCIDLumiCondAlgSC.

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory


def CaloBCIDLumiCondAlgSCCfg (flags):
    result = ComponentAccumulator()

    from CaloRec.CaloBCIDCoeffsCondAlgSCConfig import CaloBCIDCoeffsCondAlgSCCfg
    result.merge (CaloBCIDCoeffsCondAlgSCCfg (flags))

    if not flags.Input.isMC:
        from LumiBlockComps.LuminosityCondAlgConfig import LuminosityCondAlgCfg
        result.merge (LuminosityCondAlgCfg (flags))

    else:
        from LumiBlockComps.BunchCrossingCondAlgConfig import BunchCrossingCondAlgCfg
        result.merge (BunchCrossingCondAlgCfg(flags))


    CaloBCIDLumiCondAlg = CompFactory.CaloBCIDLumiCondAlg # CaloRec
    alg = CaloBCIDLumiCondAlg ('CaloBCIDLumiCondAlgSC',
                               CoeffsKey = 'CaloBCIDCoeffsSC',
                               BunchCrossingCondDataKey = 'BunchCrossingData',
                               LuminosityCondDataKey = 'LuminosityCondData',
                               isMC = flags.Input.isMC,
                               OutputLumiKey = 'CaloBCIDLumiSC')
    result.addCondAlgo (alg)

    return result
