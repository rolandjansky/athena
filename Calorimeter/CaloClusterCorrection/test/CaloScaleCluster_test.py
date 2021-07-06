#
# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration.
#
# File: CaloClusterCorrection/test/CaloScaleCluster_test.py
# Author: scott snyder
# Date: Jan, 2021
# Brief: Test for CaloScaleCluster
#


from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaPython.PyAthenaComps import Alg, StatusCode
import ROOT


class TestAlg (Alg):
    clusters = [
        [0.123, 0.23, 45000, 48461.515625],
        [0,     0,    45000, 74994.3671875],
        [1.490000, 0, 45000, 119415.4609375],
        [1.500000, 0, 45000, 106255.5390625],
        [1.500500, 0, 45000, 105361.65625],
        [1.5005310535430909, 0, 45000, 105306.515625],
        [1.501000, 0, 45000, 104528.109375],
        [1.508000, 0, 45000, 90022.875],
        ]


    def __init__ (self, name):
        Alg.__init__ (self, name)
        return

    def initialize (self):
        self.tool = ROOT.ToolHandle(ROOT.CaloClusterCollectionProcessor)('CaloScaleCluster/rfac_v5_ele35')
        if not self.tool.retrieve():
            assert 0
        return StatusCode.Success


    def execute (self):
        ctx = self.getContext()
        clc, clc_store = self.makeClusters()
        if not self.tool.execute (ctx, clc):
            return StatusCode.Failure
        for (cl, desc) in zip (clc, TestAlg.clusters):
            exp = desc[3]
            if (abs(exp - cl.e())) / (exp + cl.e()) > 1e-3:
                print ('Bad compare: ', exp, cl.e(), cl.eta(), cl.phi())
                assert 0
        return StatusCode.Success


    def makeClusters (self):
        clc = ROOT.xAOD.CaloClusterContainer()
        clc_store = ROOT.xAOD.CaloClusterAuxContainer()
        clc.setStore (clc_store)

        for desc in TestAlg.clusters:
            cl = ROOT.xAOD.CaloCluster()
            clc.push_back (cl)
            ROOT.SetOwnership (cl, False)
            cl.setClusterSize (ROOT.xAOD.CaloCluster.SW_35ele)
            cl.setSamplingPattern (1<<2)
            cl.setEta (desc[0])
            cl.setPhi (desc[1])
            cl.setEta (2, desc[0])
            cl.setPhi (2, desc[1])
            cl.setEnergy (2, desc[2])
            cl.setE (desc[2])
        return (clc, clc_store)
        

def testCfg (configFlags):
    result = ComponentAccumulator()

    from LArGeoAlgsNV.LArGMConfig import LArGMCfg
    result.merge(LArGMCfg(configFlags))
    from LArCabling.LArCablingConfig import LArOnOffIdMappingCfg
    result.merge(LArOnOffIdMappingCfg(configFlags))

    from CaloClusterCorrection.CaloSwCorrections import rfac, make_CaloSwCorrectionsCfg
    from CaloClusterCorrection.constants import CALOCORR_JO
    corr = make_CaloSwCorrectionsCfg (configFlags,
                                      key = 'ele35',
                                      source = CALOCORR_JO,
                                      version = 'v12_calh',
                                      corrlist = [[rfac, 'v5']])
    tools = corr.popPrivateTools()
    result.merge (corr)
    result.addPublicTool (tools[0])

    result.addEventAlgo (TestAlg ('TestAlg'))
    return result


from AthenaCommon.Configurable import Configurable
Configurable.configurableRun3Behavior=1
from AthenaConfiguration.AllConfigFlags import ConfigFlags
from AthenaConfiguration.TestDefaults import defaultTestFiles

ConfigFlags.LAr.doAlign = False
ConfigFlags.Input.Files = defaultTestFiles.AOD_MC

ConfigFlags.lock()
from AthenaConfiguration.MainServicesConfig import MainServicesCfg 
acc=MainServicesCfg(ConfigFlags)

acc.merge (testCfg (ConfigFlags))
acc.run(1)

