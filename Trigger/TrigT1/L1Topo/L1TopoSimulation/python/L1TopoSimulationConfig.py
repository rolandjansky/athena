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

    #Grab the MUCTPI tool
    if flags.Trigger.enableL1Phase1:
        from TrigT1MuctpiPhase1.TrigT1MuctpiPhase1Config import MUCTPI_AthToolCfg
        muctpiTool = MUCTPI_AthToolCfg("MUCTPI_AthTool")
        acc.addPublicTool(muctpiTool, primary=True)
    else:
        from TrigT1Muctpi.TrigT1MuctpiConfig import L1MuctpiToolRDOCfg
        muctpiToolAcc = L1MuctpiToolRDOCfg(flags)
        muctpiTool = muctpiToolAcc.getPrimary()
        acc.merge(muctpiToolAcc)

    #Configure the MuonInputProvider
    muProvider = CompFactory.LVL1.MuonInputProvider("MuonInputProvider", 
                                                    ROIBResultLocation = "", #disable input from RoIBResult
                                                    MuctpiSimTool = muctpiTool,
                                                    MuonEncoding = 1 if flags.Input.isMC else 0, 
                                                    UseNewConfig = flags.Trigger.readLVL1FromJSON)

    #Configure the MuonRoiTools for the MIP
    from TrigT1MuonRecRoiTool.TrigT1MuonRecRoiToolConfig import getRun3RPCRecRoiTool, getRun3TGCRecRoiTool
    muProvider.RecRpcRoiTool = getRun3RPCRecRoiTool("RPCRecRoiTool", useRun3Config = flags.Trigger.enableL1Phase1)
    muProvider.RecTgcRoiTool = getRun3TGCRecRoiTool("TGCRecRoiTool", useRun3Config = flags.Trigger.enableL1Phase1)

    emtauProvider = CompFactory.LVL1.EMTauInputProvider("EMTauInputProvider")

    topoSimAlg = CompFactory.LVL1.L1TopoSimulation("L1TopoSimulation",
                                                    MuonInputProvider = muProvider,
                                                    EMTAUInputProvider = emtauProvider)
    acc.addEventAlgo(topoSimAlg)
    return acc
