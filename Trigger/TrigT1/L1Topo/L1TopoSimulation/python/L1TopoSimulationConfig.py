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

def L1LegacyTopoSimulationCfg(flags):
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    from AthenaConfiguration.ComponentFactory import CompFactory
    
    acc = ComponentAccumulator()
    
    emtauProvider = CompFactory.LVL1.EMTauInputProvider("EMTauInputProvider")

    topoSimAlg = CompFactory.LVL1.L1TopoSimulation("L1LegacyTopoSimulation",
                                                    EMTAUInputProvider = emtauProvider,
                                                    IsLegacyTopo = True,
                                                    MonHistBaseDir = "L1/L1LegacyTopoAlgorithms")

    # No muon inputs to legacy Topo
    topoSimAlg.MuonInputProvider.ROIBResultLocation = ""
    topoSimAlg.MuonInputProvider.MuonROILocation = ""
    topoSimAlg.MuonInputProvider.locationMuCTPItoL1Topo = ""
    topoSimAlg.MuonInputProvider.ROIBResultLocation = ""

    acc.addEventAlgo(topoSimAlg)
    return acc

def L1TopoSimulationCfg(flags):
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    from AthenaConfiguration.ComponentFactory import CompFactory
    
    acc = ComponentAccumulator()

    #Configure the MuonInputProvider
    muProvider = CompFactory.LVL1.MuonInputProvider("MuonInputProvider",
                                                    ROIBResultLocation = "", #disable input from RoIBResult
                                                    MuonROILocation = "",
                                                    MuonEncoding = 1)

    #Configure the MuonRoiTools for the MIP
    from TrigT1MuonRecRoiTool.TrigT1MuonRecRoiToolConfig import getRun3RPCRecRoiTool, getRun3TGCRecRoiTool
    muProvider.RecRpcRoiTool = getRun3RPCRecRoiTool("RPCRecRoiTool", useRun3Config = True)
    muProvider.RecTgcRoiTool = getRun3TGCRecRoiTool("TGCRecRoiTool", useRun3Config = True)

    emtauProvider = CompFactory.LVL1.EMTauInputProviderFEX("EMTauInputProviderFEX")
    jetProvider = CompFactory.LVL1.JetInputProviderFEX("JetInputProviderFEX")
    #energyProvider = CompFactory.LVL1.EnergyInputProviderFEX("EnergyInputProviderFEX")

    topoSimAlg = CompFactory.LVL1.L1TopoSimulation("L1TopoSimulation",
                                                    MuonInputProvider = muProvider,
                                                    EMTAUInputProvider = emtauProvider,
                                                    JetInputProvider = jetProvider,
                                                    #EnergyInputProvider = energyProvider,
                                                    IsLegacyTopo = False)
    acc.addEventAlgo(topoSimAlg)
    return acc

def L1TopoSimulationOldStyleCfg(flags, isLegacy):
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
        # Need further test from inputs. Reverting back to Run 2 MET 
        #topoSimSeq.EnergyInputProvider = 'LVL1::EnergyInputProviderFEX/EnergyInputProviderFEX'

    # Muon inputs only for phase-1 Topo
    if isLegacy:
        topoSimSeq.MuonInputProvider.ROIBResultLocation = ""
        topoSimSeq.MuonInputProvider.MuonROILocation = ""
        topoSimSeq.MuonInputProvider.locationMuCTPItoL1Topo = ""
        topoSimSeq.MuonInputProvider.ROIBResultLocation = ""
    else:
        if flags.Trigger.doLVL1:
            topoSimSeq.MuonInputProvider.ROIBResultLocation = "" #disable input from RoIBResult

        from TrigT1MuonRecRoiTool.TrigT1MuonRecRoiToolConfig import getRun3RPCRecRoiTool, getRun3TGCRecRoiTool
        topoSimSeq.MuonInputProvider.RecRpcRoiTool = getRun3RPCRecRoiTool(useRun3Config=True)
        topoSimSeq.MuonInputProvider.RecTgcRoiTool = getRun3TGCRecRoiTool(useRun3Config=True)
        topoSimSeq.MuonInputProvider.MuonROILocation = ""
        topoSimSeq.MuonInputProvider.MuonEncoding = 1

    return topoSimSeq
