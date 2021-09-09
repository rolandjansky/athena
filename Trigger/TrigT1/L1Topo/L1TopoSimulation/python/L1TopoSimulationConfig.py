# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from L1TopoSimulation.L1TopoSimulationConf import LVL1__L1TopoSimulation, LVL1__RoiB2TopoInputDataCnv, LVL1__MuonInputProviderLegacy

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

class MuonInputProviderLegacy ( LVL1__MuonInputProviderLegacy ):

    def __init__( self, name = "MuonInputProviderLegacy" ):
        super( MuonInputProviderLegacy, self ).__init__( name )

def L1LegacyTopoSimulationCfg(flags):
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    from AthenaConfiguration.ComponentFactory import CompFactory
    
    acc = ComponentAccumulator()
    
    #Grab the MUCTPI tool
    if flags.Trigger.enableL1MuonPhase1:
        from TrigT1MuctpiPhase1.TrigT1MuctpiPhase1Config import MUCTPI_AthToolCfg
        muctpiTool = MUCTPI_AthToolCfg("MUCTPI_AthTool")
        acc.addPublicTool(muctpiTool, primary=True)
    else:
        from TrigT1Muctpi.TrigT1MuctpiConfig import L1MuctpiToolRDOCfg
        muctpiToolAcc = L1MuctpiToolRDOCfg(flags)
        muctpiTool = muctpiToolAcc.getPrimary()
        acc.merge(muctpiToolAcc)

    #Configure the MuonInputProvider
    if flags.Trigger.enableL1MuonPhase1:
        muProvider = CompFactory.LVL1.MuonInputProvider("MuonInputProvider", 
                                                        ROIBResultLocation = "", #disable input from RoIBResult
                                                        MuonROILocation = "",
                                                        MuonEncoding = 1)
    else:
        muProvider = CompFactory.LVL1.MuonInputProviderLegacy("MuonInputProviderLegacy", 
                                                              ROIBResultLocation = "", #disable input from RoIBResult
                                                              MuctpiSimTool = muctpiTool,
                                                              MuonEncoding = 1 if flags.Input.isMC else 0)

    #Configure the MuonRoiTools for the MIP
    from TrigT1MuonRecRoiTool.TrigT1MuonRecRoiToolConfig import getRun3RPCRecRoiTool, getRun3TGCRecRoiTool
    muProvider.RecRpcRoiTool = getRun3RPCRecRoiTool("RPCRecRoiTool", useRun3Config = flags.Trigger.enableL1MuonPhase1)
    muProvider.RecTgcRoiTool = getRun3TGCRecRoiTool("TGCRecRoiTool", useRun3Config = flags.Trigger.enableL1MuonPhase1)

    emtauProvider = CompFactory.LVL1.EMTauInputProvider("EMTauInputProvider")

    topoSimAlg = CompFactory.LVL1.L1TopoSimulation("L1LegacyTopoSimulation",
                                                    MuonInputProvider = muProvider,
                                                    EMTAUInputProvider = emtauProvider,
                                                    IsLegacyTopo = True,
                                                    MonHistBaseDir = "L1/L1LegacyTopoAlgorithms")
    acc.addEventAlgo(topoSimAlg)
    return acc

def L1TopoSimulationCfg(flags):
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    from AthenaConfiguration.ComponentFactory import CompFactory
    
    acc = ComponentAccumulator()

    #Configure the MuonInputProvider
    if flags.Trigger.enableL1MuonPhase1:
        muProvider = CompFactory.LVL1.MuonInputProvider("MuonInputProvider", 
                                                        ROIBResultLocation = "", #disable input from RoIBResult
                                                        MuonROILocation = "",
                                                        MuonEncoding = 1)
    else:
        #Grab the MUCTPI tool
        from TrigT1Muctpi.TrigT1MuctpiConfig import L1MuctpiToolRDOCfg
        muctpiToolAcc = L1MuctpiToolRDOCfg(flags)
        muctpiTool = muctpiToolAcc.getPrimary()
        acc.merge(muctpiToolAcc)

        muProvider = CompFactory.LVL1.MuonInputProviderLegacy("MuonInputProviderLegacy", 
                                                              ROIBResultLocation = "", #disable input from RoIBResult
                                                              MuctpiSimTool = muctpiTool,
                                                              MuonEncoding = 1 if flags.Input.isMC else 0)

    #Configure the MuonRoiTools for the MIP
    from TrigT1MuonRecRoiTool.TrigT1MuonRecRoiToolConfig import getRun3RPCRecRoiTool, getRun3TGCRecRoiTool
    muProvider.RecRpcRoiTool = getRun3RPCRecRoiTool("RPCRecRoiTool", useRun3Config = flags.Trigger.enableL1MuonPhase1)
    muProvider.RecTgcRoiTool = getRun3TGCRecRoiTool("TGCRecRoiTool", useRun3Config = flags.Trigger.enableL1MuonPhase1)

    emtauProvider = CompFactory.LVL1.EMTauInputProviderFEX("EMTauInputProviderFEX")

    topoSimAlg = CompFactory.LVL1.L1TopoSimulation("L1TopoSimulation",
                                                    MuonInputProvider = muProvider,
                                                    EMTAUInputProvider = emtauProvider,
                                                    IsLegacyTopo = False)
    acc.addEventAlgo(topoSimAlg)
    return acc

def L1TopoSimulationOldStyleCfg(flags, isLegacy):
    from AthenaCommon.AppMgr import ToolSvc
    from L1TopoSimulation.L1TopoSimulationConfig import L1TopoSimulation
    key = 'Legacy' if isLegacy else 'Phase1'
    topoSimSeq = L1TopoSimulation('L1'+key+'TopoSimulation')
    topoSimSeq.UseBitwise = False # Need to switch true (probably will change the counts)
    topoSimSeq.InputDumpFile = 'inputdump_' + key.lower() + '.txt'
    topoSimSeq.EnableInputDump = flags.Trigger.enableL1TopoDump
    topoSimSeq.IsLegacyTopo = isLegacy
    topoSimSeq.MonHistBaseDir = 'L1/L1'+key+'TopoAlgorithms'

    # Calo inputs
    if flags.Trigger.enableL1CaloPhase1 and not isLegacy:
        topoSimSeq.EMTAUInputProvider = 'LVL1::EMTauInputProviderFEX/EMTauInputProviderFEX'
        # Need further test from inputs.
        topoSimSeq.JetInputProvider = 'LVL1::JetInputProviderFEX/JetInputProviderFEX'

    # Muon inputs
    from L1TopoSimulation.L1TopoSimulationConfig import MuonInputProviderLegacy
    ToolSvc += MuonInputProviderLegacy('MuonInputProviderLegacy')

    if flags.Trigger.doLVL1:
        # TODO: the legacy simulation should not need to deal with muon inputs
        topoSimSeq.MuonInputProvider.ROIBResultLocation = "" #disable input from RoIBResult
        ToolSvc.MuonInputProviderLegacy.ROIBResultLocation = "" #disable input from RoIBResult

    if flags.Trigger.enableL1MuonPhase1: 
        from TrigT1MuonRecRoiTool.TrigT1MuonRecRoiToolConfig import getRun3RPCRecRoiTool, getRun3TGCRecRoiTool
        topoSimSeq.MuonInputProvider.RecRpcRoiTool = getRun3RPCRecRoiTool(useRun3Config=True)
        topoSimSeq.MuonInputProvider.RecTgcRoiTool = getRun3TGCRecRoiTool(useRun3Config=True)
        topoSimSeq.MuonInputProvider.MuonROILocation = ""
        topoSimSeq.MuonInputProvider.MuonEncoding = 1
    else:
        from AthenaCommon.AppMgr import ServiceMgr as svcMgr
        from TrigT1Muctpi.TrigT1MuctpiConfig import L1MuctpiTool
        ToolSvc += L1MuctpiTool("L1MuctpiTool")
        ToolSvc.L1MuctpiTool.LVL1ConfigSvc = svcMgr.LVL1ConfigSvc
        ToolSvc.MuonInputProviderLegacy.MuctpiSimTool = ToolSvc.L1MuctpiTool
        ToolSvc.MuonInputProviderLegacy.locationMuCTPItoL1Topo = ""
        if not flags.Trigger.doLVL1:
            ToolSvc.MuonInputProviderLegacy.MuonROILocation = ""
        # enable the reduced (coarse) granularity topo simulation
        # currently only for MC
        # apparently not needed for phase1
        if not flags.Input.isMC:
            ToolSvc.MuonInputProviderLegacy.MuonEncoding = 1
        else:
            ToolSvc.MuonInputProviderLegacy.MuonEncoding = 0

    if not flags.Trigger.enableL1MuonPhase1:
        topoSimSeq.MuonInputProvider = ToolSvc.MuonInputProviderLegacy

    return topoSimSeq
