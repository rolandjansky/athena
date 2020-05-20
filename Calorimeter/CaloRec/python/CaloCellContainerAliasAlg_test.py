#
# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
#
# File: CaloRec/python/CaloCellContainerAliasAlg_test.py
# Author: scott snyder
# Date: Nov, 2019
# Brief: Test for CaloCellContainerAliasAlg.
#

from __future__ import print_function


from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaPython.PyAthenaComps import Alg, StatusCode
import ROOT


class CreateDataAlg (Alg):
    def execute (self):
        ccc = ROOT.CaloCellContainer()
        self.evtStore.record (ccc, 'AllCalo', False)
        return StatusCode.Success


class CheckAliasAlg (Alg):
    def execute (self):
        ccc1 = self.evtStore['AllCalo']
        ccc2 = self.evtStore['CellAlias']
        assert (repr(ccc1) == repr(ccc2))
        return StatusCode.Success


def testCfg (configFlags):
    result = ComponentAccumulator()

    from LArGeoAlgsNV.LArGMConfig import LArGMCfg
    from TileGeoModel.TileGMConfig import TileGMCfg
    result.merge(LArGMCfg(configFlags))
    result.merge(TileGMCfg(configFlags))

    from LArCabling.LArCablingConfig import LArOnOffIdMappingCfg
    result.merge(LArOnOffIdMappingCfg(configFlags))

    result.addEventAlgo (CreateDataAlg ('CreateDataAlg'))

    CaloCellContainerAliasAlg=CompFactory.CaloCellContainerAliasAlg
    result.addEventAlgo (CaloCellContainerAliasAlg ('aliasAlg',
                                                    Cells = 'AllCalo',
                                                    Alias = 'CellAlias'))

    result.addEventAlgo (CheckAliasAlg ('CheckAliasAlg'))
    return result


from AthenaCommon.Configurable import Configurable
Configurable.configurableRun3Behavior=1
from AthenaConfiguration.AllConfigFlags import ConfigFlags
from AthenaConfiguration.TestDefaults import defaultTestFiles

ConfigFlags.Input.Files = defaultTestFiles.RDO
ConfigFlags.addFlag("Input.InitialTimeStamp", 1000)
ConfigFlags.Detector.GeometryLAr = True
ConfigFlags.Detector.GeometryTile = True
ConfigFlags.needFlagsCategory('Tile')
ConfigFlags.needFlagsCategory('LAr')

ConfigFlags.lock()
from AthenaConfiguration.MainServicesConfig import MainServicesCfg 
acc=MainServicesCfg(ConfigFlags)

from McEventSelector.McEventSelectorConfig import McEventSelectorCfg
acc.merge (McEventSelectorCfg (ConfigFlags))

acc.merge (testCfg (ConfigFlags))
acc.run(1)

    
