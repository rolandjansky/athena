# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from L1TopoSimulation.L1TopoSimulationConf import LVL1__L1TopoSimulation, LVL1__RoiB2TopoInputDataCnv

class L1TopoSimulation ( LVL1__L1TopoSimulation ):

    def __init__( self, name = "L1TopoSimulation" ):
        super( L1TopoSimulation, self ).__init__( name )

        enableDebugOutput = False
        if enableDebugOutput:
            from AthenaCommon.Constants import DEBUG
            self.OutputLevel = DEBUG
            self.TopoOutputLevel = DEBUG
            self.TopoSteeringOutputLevel = DEBUG

class RoiB2TopoInputDataCnv ( LVL1__RoiB2TopoInputDataCnv ):

    def __init__( self, name = "RoiB2TopoInputDataCnv" ):
        super( RoiB2TopoInputDataCnv, self ).__init__( name )

def L1TopoSimulationMCCfg(flags):
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    from AthenaConfiguration.ComponentFactory import CompFactory
    acc = ComponentAccumulator()

    #TODO - add varaint to support phase I
    from TrigT1Muctpi.TrigT1MuctpiConfig import L1MuctpiToolRDOCfg
    muctpiToolAcc = L1MuctpiToolRDOCfg(flags)
    muctpiTool = muctpiToolAcc.getPrimary()
    acc.merge(muctpiToolAcc)
    
    muProvider = CompFactory.LVL1.MuonInputProvider("MuonInputProvider", 
                                                    ROIBResultLocation = "", #disable input from RoIBResult
                                                    MuctpiSimTool = muctpiTool,
                                                    MuonEncoding = 1 if flags.Input.isMC else 0, 
                                                    UseNewConfig = flags.Trigger.readLVL1FromJSON)
    emtauProvider = CompFactory.LVL1.EMTauInputProvider("EMTauInputProvider")


    topoSimAlg = CompFactory.LVL1.L1TopoSimulation("L1TopoSimulation",
                                                    MuonInputProvider = muProvider,
                                                    EMTAUInputProvider = emtauProvider)
    acc.addEventAlgo(topoSimAlg)
    return acc
