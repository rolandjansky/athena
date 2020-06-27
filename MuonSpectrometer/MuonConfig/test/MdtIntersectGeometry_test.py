#
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration.
#
# File: MuonStationIntersectSvc/share/MdtIntersectGeometry_test.py
# Author: scott snyder
# Date: Jul, 2019
# Brief: Test for MdtIntersectGeometry
#        For now, only tests the dead tube handling.
#


from __future__ import print_function

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaPython.PyAthenaComps import Alg, StatusCode
import ROOT


class TestAlg (Alg):
    def __init__ (self, name):
        Alg.__init__ (self, name)
        return

    def initialize (self):
        self.svc = ROOT.ServiceHandle(ROOT.IInterface)('MuonStationIntersectSvc', 'test')
        if not self.svc.retrieve():
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

    MuonStationIntersectSvc = CompFactory.MuonStationIntersectSvc
    result.addService (MuonStationIntersectSvc ('MuonStationIntersectSvc',
                                                OutputLevel = 1))
    
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

