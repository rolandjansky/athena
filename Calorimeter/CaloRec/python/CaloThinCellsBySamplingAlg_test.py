#
# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
#
# File: CaloRec/python/CaloThinCellsBySamplingAlg_test.py
# Author: scott snyder
# Date: Nov, 2019
# Brief: Test for CaloThinCellsBySamplingAlg.
#

from __future__ import print_function


from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaPython.PyAthenaComps import Alg, StatusCode
import ROOT


def make_calo_cells (detStore):
    mgr = detStore['CaloMgr']
    ccc = ROOT.CaloCellContainer()
    for i in range (mgr.element_size()):
        elt = mgr.get_element (ROOT.IdentifierHash (i))
        if not elt: break
        cc = ROOT.CaloCell (elt, 0, 0, 0, 0)
        ccc.push_back (cc)
        ROOT.SetOwnership (cc, False)
    ccc.order()
    ccc.updateCaloIterators()
    return ccc


class CreateDataAlg (Alg):
    def execute (self):
        ccc = make_calo_cells (self.detStore)
        self.evtStore.record (ccc, 'AllCalo', False)
        return StatusCode.Success


class CheckThinningAlg (Alg):
    def execute (self):
        mgr = self.detStore['CaloMgr']
        dec = self.evtStore['AllCalo_THINNED_StreamAOD.thinAlg']

        for i in range (dec.size()):
            elt = mgr.get_element (ROOT.IdentifierHash (i))
            if elt.getSampling() == 3 or elt.getSampling() == 17:
                assert not dec.thinned(i)
            else:
                assert dec.thinned(i)
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

    CaloThinCellsBySamplingAlg=CompFactory.CaloThinCellsBySamplingAlg
    result.addEventAlgo (CaloThinCellsBySamplingAlg ('thinAlg',
                                                     StreamName = 'StreamAOD',
                                                     SamplingCellsName = ['EMB3',
                                                                          'TileGap3']))

    result.addEventAlgo (CheckThinningAlg ('CheckThinningAlg'))
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

    
