#
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration.
#
# File: TrigL2MuonSA/share/MdtDataPreparator_test.py
# Author: scott snyder
# Date: Jul, 2019
# Brief: Test for MdtDataPreparator
#        For now, only tests the dead tube handling.
#


from __future__ import print_function
from AthenaConfiguration.ComponentFactory import CompFactory

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaPython.PyAthenaComps import Alg, StatusCode
import ROOT


class TestAlg (Alg):
    def __init__ (self, name):
        Alg.__init__ (self, name)
        return

    def initialize (self):
        #ROOT.Muon.IMuonRdoToPrepDataTool

        self.tool = ROOT.ToolHandle(ROOT.AthAlgTool)('TrigL2MuonSA::MdtDataPreparator')
        if not self.tool.retrieve():
            assert 0
        return StatusCode.Success


    def execute (self):
        return StatusCode.Success
        

def testCfg (configFlags):
    result = ComponentAccumulator()

    from MuonConfig.MuonGeometryConfig import MuonGeoModelCfg
    result.merge (MuonGeoModelCfg(configFlags))

    from MagFieldServices.MagFieldServicesConfig import MagneticFieldSvcCfg
    result.merge (MagneticFieldSvcCfg(configFlags, UseDCS = False))

    TrigL2MuonSA__MdtDataPreparator=CompFactory.TrigL2MuonSA.MdtDataPreparator
    result.addPublicTool (TrigL2MuonSA__MdtDataPreparator ('TrigL2MuonSA::MdtDataPreparator', OutputLevel = 1)) # noqa: ATL900
    
    result.addEventAlgo (TestAlg ('TestAlg'))
    return result


from AthenaCommon.Configurable import Configurable
Configurable.configurableRun3Behavior=1
from AthenaConfiguration.AllConfigFlags import ConfigFlags
from AthenaConfiguration.TestDefaults import defaultTestFiles

ConfigFlags.Input.Files = defaultTestFiles.RAW

ConfigFlags.lock()
from AthenaConfiguration.MainServicesConfig import MainServicesCfg 
acc=MainServicesCfg(ConfigFlags)

acc.merge (testCfg (ConfigFlags))
acc.run(1)

