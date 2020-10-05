#
# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration.
#
# File: CaloTools/python/CaloEstimatedGainTool_test.py
# Author: scott snyder
# Date: Aug, 2019
# Brief: Test for CaloEstimatedGainTool.
#

from __future__ import print_function


from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaPython.PyAthenaComps import Alg, StatusCode
from AthenaCommon.Logging import logging
import ROOT


def i32(x):
    if x >= (1<<31):
        x -= (1<<32)
    return x


class TestAlg (Alg):
    def __init__ (self, name):
        Alg.__init__ (self, name)
        return

    def initialize (self):
        ROOT.ICaloEstimatedGainTool

        self.tool = ROOT.ToolHandle(ROOT.ICaloEstimatedGainTool)('CaloEstimatedGainTool')
        if not self.tool.retrieve():
            assert 0
        return StatusCode.Success


    def testcell (self, elt, elist):
        ctx = self.getContext()
        for (e, gain) in elist:
            if isinstance(e, type(())):
                cell = ROOT.TileCell(elt, e[0])
                cell.setEnergy (e[0], e[1], 0, 0)
            else:
                cell = ROOT.CaloCell (elt, e, 0, 0, 0)
                g = i32(self.tool.estimatedGain (ctx, elt, e, 0))
                assert g == gain, (e, g, gain)
                g = i32(self.tool.estimatedGain (ctx, elt, e, 1))
                assert g == gain, (e, g, gain)

            g = i32(self.tool.estimatedGain (ctx, cell, 0))
            assert g == gain, (e, g, gain)
            g = i32(self.tool.estimatedGain (ctx, cell, elt, 0))
            assert g == gain, (e, g, gain)
            g = i32(self.tool.estimatedGain (ctx, cell, 1))
            assert g == gain, (e, g, gain)
            g = i32(self.tool.estimatedGain (ctx, cell, elt, 1))
            assert g == gain, (e, g, gain)
        return
            
    def execute (self):
        log = logging.getLogger ('TestAlg')
        mgr = self.detStore['CaloMgr']
        elt1 = mgr.get_element (ROOT.CaloCell_ID.LAREM,
                                2, True, 0.5, 0.1)
        self.testcell (elt1, [(1000, 0), (50000, 1), (300000, 2)])

        elt2 = mgr.get_element (ROOT.CaloCell_ID.LARHEC,
                                2, False, 2.1, 0.1)
        self.testcell (elt2, [(1000, 1), (50000, 1), (300000, 2)])

        elt3 = mgr.get_element (ROOT.CaloCell_ID.LARFCAL,
                                1, False, 4.9, 0.1)
        self.testcell (elt3, [(1000, 0), (50000, 1), (800000, 2)])

        elt4 = mgr.get_element (ROOT.CaloCell_ID.TileBar1, 0.3, 0.1)
        self.testcell (elt4, [((1000, 1000), i32(ROOT.CaloGain.TILEHIGHHIGH)),
                              ((1000, 50000), i32(ROOT.CaloGain.TILEHIGHLOW)),
                              ((50000, 1000), i32(ROOT.CaloGain.TILEHIGHLOW)),
                              ((50000, 50000), i32(ROOT.CaloGain.TILELOWLOW)),
                             ])
        log.info ('finished')
        return StatusCode.Success


def testCfg (configFlags):
    result = ComponentAccumulator()

    from LArGeoAlgsNV.LArGMConfig import LArGMCfg
    from TileGeoModel.TileGMConfig import TileGMCfg
    result.merge(LArGMCfg(configFlags))
    result.merge(TileGMCfg(configFlags))

    from CaloTools.CaloEstimatedGainToolConfig import CaloEstimatedGainToolCfg
    acc = CaloEstimatedGainToolCfg (configFlags)
    acc.popPrivateTools()
    result.merge (acc)

    result.addEventAlgo (TestAlg ('TestAlg'))
    return result


from AthenaCommon.Configurable import Configurable
Configurable.configurableRun3Behavior=1
from AthenaConfiguration.AllConfigFlags import ConfigFlags
from AthenaConfiguration.TestDefaults import defaultTestFiles

ConfigFlags.Input.Files = defaultTestFiles.RDO
ConfigFlags.Detector.GeometryLAr = True
ConfigFlags.Detector.GeometryTile = True
ConfigFlags.needFlagsCategory('Tile')
ConfigFlags.needFlagsCategory('LAr')

ConfigFlags.lock()
from AthenaConfiguration.MainServicesConfig import MainServicesCfg 
acc=MainServicesCfg(ConfigFlags)

acc.merge (testCfg (ConfigFlags))
acc.run(1)

