# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# File: EventSelectorAthenaPool/python/CondProxyProviderConfig.py
# Created: Jun 2020, sss
# Purpose: Configure CondProxyProvider.

from __future__ import print_function

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory


def CondProxyProviderCfg (flags, poolFiles):
    if not isinstance (poolFiles, list):
        poolFiles = [poolFiles]

    result = ComponentAccumulator()

    ProxyProviderSvc = CompFactory.ProxyProviderSvc # SGComps
    pps = ProxyProviderSvc (ProviderNames = ['CondProxyProvider'])
    result.addService (pps)
    
    CondProxyProvider = CompFactory.CondProxyProvider # EventSelectorAthenaPool
    cpp = CondProxyProvider (InputCollections = poolFiles)
    result.addService (cpp)

    return result
    

if __name__ == "__main__":
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    ConfigFlags.loadAllDynamicFlags()

    flags1 = ConfigFlags.clone()
    flags1.lock()
    acc1 = ComponentAccumulator()
    acc1.merge (CondProxyProviderCfg (flags1, 'file1'))
    acc1.merge (CondProxyProviderCfg (flags1, ['file2', 'file3']))
    acc1.printConfig (summariseProps=True)
    print ('CondProxyProvider:', acc1.getService('CondProxyProvider'))
    acc1.wasMerged()

