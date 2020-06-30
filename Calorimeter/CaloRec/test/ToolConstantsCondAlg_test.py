#!/usr/bin/env python
#
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration.
#
# File: CaloRec/python/ToolConstantsCondALg_test.py
# Author: scott snyder
# Date: Jun, 2020
# Brief: Test for ToolConstantsCondAlg.
#


from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
import ROOT


def testCfg (configFlags):
    result = ComponentAccumulator()

    from IOVDbSvc.IOVDbSvcConfig import addFolders
    result.merge (addFolders (configFlags,
                              '/LAR/CellCorrOfl/deadOTX',
                              detDb = 'LAR_OFL',
                              className = 'CondAttrListCollection'))

    from CaloRec.ToolConstantsCondAlgConfig import ToolConstantsCondAlgCfg
    result.merge (ToolConstantsCondAlgCfg (configFlags,
                                           'deadOTXCorrCtes',
                                           COOLFolder='/LAR/CellCorrOfl/deadOTX'))

    from EventSelectorAthenaPool.CondProxyProviderConfig import CondProxyProviderCfg
    from CaloClusterCorrection.poolfiles import poolfiles
    result.merge (CondProxyProviderCfg (configFlags,
                                        poolFiles = [poolfiles['caloswcorr_pool_v22']]))
    result.merge (ToolConstantsCondAlgCfg (configFlags,
                                           'CaloSwClusterCorrections.rfac-v5',
                                           DetStoreKey='CaloSwClusterCorrections.rfac-v5'))

    CaloClusterCorrDumper = CompFactory.CaloClusterCorrDumper # CaloRec
    alg = CaloClusterCorrDumper ('dumper1',
                                 Constants = ['deadOTXCorrCtes',
                                              'CaloSwClusterCorrections.rfac-v5'])
    result.addEventAlgo (alg)
    return result


from AthenaCommon.Configurable import Configurable
Configurable.configurableRun3Behavior=1
from AthenaConfiguration.AllConfigFlags import ConfigFlags
from AthenaConfiguration.TestDefaults import defaultTestFiles

ConfigFlags.Input.Files = defaultTestFiles.RDO
ConfigFlags.addFlag("Input.InitialTimeStamp", 1000)

ConfigFlags.lock()
from AthenaConfiguration.MainServicesConfig import MainServicesCfg 
acc=MainServicesCfg (ConfigFlags)

from McEventSelector.McEventSelectorConfig import McEventSelectorCfg
acc.merge (McEventSelectorCfg (ConfigFlags))

acc.merge (testCfg (ConfigFlags))
acc.run(1)
