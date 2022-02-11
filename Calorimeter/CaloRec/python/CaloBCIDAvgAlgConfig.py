# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

# File: CaloRec/python/CaloBCIDAvgAlgConfig.py
# Created: Mar 2019, sss
# Purpose: Configure CaloBCIDAvgAlg.

from __future__ import print_function
from AthenaConfiguration.ComponentFactory import CompFactory


from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

def CaloBCIDAvgAlgCfg (flags):
    CaloBCIDAvgAlg=CompFactory.CaloBCIDAvgAlg

    result = ComponentAccumulator()

    from LArRecUtils.LArRecUtilsConfig import LArMCSymCondAlgCfg
    result.merge (LArMCSymCondAlgCfg (flags))

    from CaloRec.CaloBCIDLumiCondAlgConfig import CaloBCIDLumiCondAlgCfg
    result.merge (CaloBCIDLumiCondAlgCfg (flags))

    result.addEventAlgo (CaloBCIDAvgAlg())
    return result



if __name__ == "__main__":
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    ConfigFlags.loadAllDynamicFlags()

    only = ['CaloBCIDAvgAlg',
            'CaloBCIDCoeffsCondAlg',
            'CaloBCIDLumiCondAlg',
            'CondInputLoader',
            'LuminosityCondAlg-',
            'LArPileUpAvgSymCondAlg',
            'LArPileUpShapeSymCondAlg',
            'LArADC2MeVCondAlg-',
            'LArOFCCondAlg-',
            'LArAutoCorrTotalCondAlg-',
            'BunchCrossingCondAlg',
            ]
    print ('--- data')
    flags1 = ConfigFlags.clone()
    flags1.Input.Files = []
    flags1.lock()
    acc1 = CaloBCIDAvgAlgCfg (flags1)
    acc1.printConfig(summariseProps=True, onlyComponents=only)
    acc1.wasMerged()

    print ('--- mc')
    flags2 = ConfigFlags.clone()
    flags2.Input.Files = []
    flags2.Input.isMC = True
    flags2.lock()
    acc2 = CaloBCIDAvgAlgCfg (flags2)
    acc2.printConfig(summariseProps=True, onlyComponents=only)
    acc2.wasMerged()

    print ('--- online')
    flags3 = ConfigFlags.clone()
    flags3.Input.Files = []
    flags3.Common.isOnline = True
    flags3.lock()
    acc3 = CaloBCIDAvgAlgCfg (flags3)
    acc3.printConfig(summariseProps=True, onlyComponents=only)
    acc3.wasMerged()
