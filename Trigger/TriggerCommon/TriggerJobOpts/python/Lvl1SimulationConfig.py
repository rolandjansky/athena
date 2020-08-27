# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration


def Lvl1SimulationSequence( flags = None ):
    """ 
    Configure L1 simulation for Athena MT jobs

    The code is structure in a way that should be easier to change once New JO system with ComponentAccumulators will be in use
    """
    # for the New JO migration
    #    from AthenaCommon.ComponentAccumulator import ComponentAccumulator
    #    acc = ComponentAccumulator()
    # all tools then should be added to the acc, cond folders as well.
    # L1ConfigSvc CA has to be imported and merged
    # at the end the sequence added to the CA
    #
    from AthenaCommon.Logging import logging
    log = logging.getLogger('TriggerJobOpts.Lvl1Simulation')
    from AthenaConfiguration.ComponentAccumulator import conf2toConfigurable
    from AthenaCommon.CFElements import seqAND
    from AthenaCommon.AppMgr import ServiceMgr as svcMgr
    from AthenaCommon.AlgSequence import AthSequencer
    from TriggerJobOpts.TriggerFlags import TriggerFlags

    # this configuration of the LVL1ConfigSvc is only temporary
    TriggerFlags.readLVL1configFromXML = True
    TriggerFlags.outputLVL1configFile = None
    log.info("setting up LVL1ConfigSvc, including the menu generation")
    from TrigConfigSvc.TrigConfigSvcCfg import getL1ConfigSvc
    svcMgr += conf2toConfigurable(getL1ConfigSvc(flags))

    ##################################################
    # Calo
    ##################################################

    if flags.Trigger.enableL1CaloLegacy:

        from TrigT1CaloSim.TrigT1CaloSimRun2Config import Run2TriggerTowerMaker
        caloTowerMaker              = Run2TriggerTowerMaker("Run2TriggerTowerMaker25ns")
        caloTowerMaker.ExtraInputs   = ["LArTTL1Container#LArTTL1EM", "LArTTL1Container#LArTTL1HAD", "TileTTL1Container#TileTTL1Cnt" ]
        caloTowerMaker.ZeroSuppress = True
        caloTowerMaker.CellType     = 3

        from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__Run2CPMTowerMaker
        from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__Run2JetElementMaker
        from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__CPMSim
        from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__JEMJetSim
        from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__JEMEnergySim
        from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__CPCMX
        from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__JetCMX
        from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__EnergyCMX
        from TrigT1CaloSim.TrigT1CaloSimConf import LVL1__RoIROD

        from TrigT1MBTS.TrigT1MBTSConf import LVL1__TrigT1MBTS
        from TrigT1ZDC.TrigT1ZDCConf import LVL1__TrigT1ZDC

        l1CaloSim = seqAND('l1CaloSim',[
            caloTowerMaker,
            #LVL1__Run2CPMTowerMaker( 'CPMTowerMaker', ExtraInputs=["XYZ#1"], ExtraOutputs=["XYZ#2"]) ,
            LVL1__Run2CPMTowerMaker( 'CPMTowerMaker') ,
            LVL1__Run2JetElementMaker( 'JetElementMaker'),
            LVL1__CPMSim( 'CPMSim' ) ,
            LVL1__JEMJetSim( 'JEMJetSim' ) ,
            LVL1__JEMEnergySim( 'JEMEnergySim' ) ,
            LVL1__CPCMX( 'CPCMX' ) ,
            LVL1__JetCMX( 'JetCMX' ) ,
            LVL1__EnergyCMX( 'EnergyCMX' ) ,
            LVL1__RoIROD( 'RoIROD' ),
            LVL1__TrigT1MBTS(),
            LVL1__TrigT1ZDC()
        ])

        from IOVDbSvc.CondDB import conddb
        L1CaloFolderList = []
        #L1CaloFolderList += ["/TRIGGER/L1Calo/V1/Calibration/Physics/PprChanCalib"]
        L1CaloFolderList += ["/TRIGGER/L1Calo/V2/Calibration/Physics/PprChanCalib"]
        #L1CaloFolderList += ["/TRIGGER/L1Calo/V1/Conditions/RunParameters"]
        #L1CaloFolderList += ["/TRIGGER/L1Calo/V1/Conditions/DerivedRunPars"]
        #L1CaloFolderList += ["/TRIGGER/Receivers/Conditions/VgaDac"]
        #L1CaloFolderList += ["/TRIGGER/Receivers/Conditions/Strategy"]
        L1CaloFolderList += ["/TRIGGER/L1Calo/V2/Conditions/DisabledTowers"]
        L1CaloFolderList += ["/TRIGGER/L1Calo/V2/Calibration/PpmDeadChannels"]
        L1CaloFolderList += ["/TRIGGER/L1Calo/V2/Configuration/PprChanDefaults"]

        for l1calofolder in L1CaloFolderList:
            #conddb.addFolderWithTag("TRIGGER_OFL", l1calofolder, "HEAD")
            conddb.addFolder( "TRIGGER_OFL", l1calofolder )

    ##################################################
    # Muons
    ##################################################
    
    from MuonByteStreamCnvTest.MuonByteStreamCnvTestConf import MuonRdoToMuonDigitTool
    MuonRdoToMuonDigitTool = MuonRdoToMuonDigitTool (DecodeMdtRDO = False,
                                                     DecodeRpcRDO = True,
                                                     DecodeTgcRDO = True,
                                                     DecodeCscRDO = False,
                                                     DecodeSTGC_RDO = False,
                                                     DecodeMM_RDO = False,
                                                     # for those subdetectors where the decoding is turned off, no need to create a RDO_Decoder ToolHandle
                                                     mdtRdoDecoderTool="",
                                                     cscRdoDecoderTool="",
                                                     stgcRdoDecoderTool="",
                                                     mmRdoDecoderTool="",
                                                     RpcDigitContainer = "RPC_DIGITS_L1",
                                                     TgcDigitContainer = "TGC_DIGITS_L1")
    
    MuonRdoToMuonDigitTool.cscCalibTool = "CscCalibTool"
    from AthenaCommon.AppMgr import ToolSvc
    ToolSvc += MuonRdoToMuonDigitTool

    from MuonByteStreamCnvTest.MuonByteStreamCnvTestConf import MuonRdoToMuonDigit
    from TrigT1RPCsteering.TrigT1RPCsteeringConf import TrigT1RPC    
    from TrigT1TGC.TrigT1TGCConf import LVL1TGCTrigger__LVL1TGCTrigger

    TrigT1RPC.useRun3Config = flags.Trigger.enableL1Phase1
    LVL1TGCTrigger__LVL1TGCTrigger.useRun3Config = flags.Trigger.enableL1Phase1

    if flags.Trigger.enableL1Phase1:
        # Placeholder for phase-I MUCTPI simulation
        log.info("Configuring Phase-I MUCTPI simulation")
        from TrigT1MuctpiPhase1.TrigT1MuctpiPhase1Config import L1MuctpiPhase1
        from TrigT1MuctpiPhase1.TrigT1MuctpiPhase1Config import L1MuctpiPhase1Tool

        ToolSvc += L1MuctpiPhase1Tool("MUCTPI_AthTool")
        ToolSvc.MUCTPI_AthTool.LVL1ConfigSvc = svcMgr.LVL1ConfigSvc

        muctpi = L1MuctpiPhase1()
        muctpi.LVL1ConfigSvc = svcMgr.LVL1ConfigSvc

    else:
        log.info("Configuring legacy (Run 2) MUCTPI simulation")
        from TrigT1Muctpi.TrigT1MuctpiConfig import L1Muctpi
        from TrigT1Muctpi.TrigT1MuctpiConfig import L1MuctpiTool

        ToolSvc += L1MuctpiTool("L1MuctpiTool")
        ToolSvc.L1MuctpiTool.LVL1ConfigSvc = svcMgr.LVL1ConfigSvc

        muctpi = L1Muctpi()
        muctpi.LVL1ConfigSvc = svcMgr.LVL1ConfigSvc

    # Sets up and configures the muon alignment and detector manager
    from MuonRecExample import MuonAlignConfig # noqa: F401

    l1MuonSim = seqAND("l1MuonSim", [
        
        MuonRdoToMuonDigit( "MuonRdoToMuonDigit",
                            MuonRdoToMuonDigitTool = ToolSvc.MuonRdoToMuonDigitTool),

        TrigT1RPC("TrigT1RPC",
                  Hardware          = True, # not sure if needed, not there in old config, present in JO
                  DataDetail        = False,
                  RPCbytestream     = False,
                  RPCbytestreamFile = "",
                  RPCDigitContainer = "RPC_DIGITS_L1"),
        
        # based on Trigger/TrigT1/TrigT1TGC/python/TrigT1TGCConfig.py
        # interesting is that this JO sets inexisting properties, commented out below
        LVL1TGCTrigger__LVL1TGCTrigger("LVL1TGCTrigger",
                                       InputData_perEvent  = "TGC_DIGITS_L1", 
                                       MuCTPIInput_TGC     = "L1MuctpiStoreTGC",
                                       MaskFileName12      = "TrigT1TGCMaskedChannel._12.db"),
        muctpi
    ])
    
    # only needed for MC
    conddb.addFolder("TGC_OFL", "/TGC/TRIGGER/CW_EIFI", className="CondAttrListCollection")
    conddb.addFolder("TGC_OFL", "/TGC/TRIGGER/CW_BW", className="CondAttrListCollection")
    conddb.addFolder("TGC_OFL", "/TGC/TRIGGER/CW_TILE", className="CondAttrListCollection")
    condSeq = AthSequencer("AthCondSeq")
    from MuonCondSvc.MuonCondSvcConf import TGCTriggerDbAlg
    condSeq += TGCTriggerDbAlg()

    ##################################################
    # Topo
    ##################################################

    l1TopoSim = None
    if flags.Trigger.enableL1Phase1:
        log.info("No phase1 configuration for L1Topo simulation is available")
    else:
        from L1TopoSimulation.L1TopoSimulationConfig import L1TopoSimulation
        l1TopoSim = L1TopoSimulation()

        l1TopoSim.MuonInputProvider.ROIBResultLocation = "" #disable input from RoIBResult
        l1TopoSim.MuonInputProvider.MuctpiSimTool = ToolSvc.L1MuctpiTool

        # enable the reduced (coarse) granularity topo simulation
        # currently only for MC
        from AthenaCommon.GlobalFlags  import globalflags
        if globalflags.DataSource()!='data':
            l1TopoSim.MuonInputProvider.MuonEncoding = 1
        else:
            l1TopoSim.MuonInputProvider.MuonEncoding = 0

    ##################################################
    # CTP
    ##################################################

    from TrigT1CTP.TrigT1CTPConfig import CTPSimulationInReco
    ctp             = CTPSimulationInReco("CTPSimulation")
    ctp.DoLUCID     = False
    ctp.DoBCM       = False
    ctp.DoL1Topo    = not flags.Trigger.enableL1Phase1
    ctp.UseCondL1Menu = False
    ctp.TrigConfigSvc = svcMgr.LVL1ConfigSvc
    ctpSim      = seqAND("ctpSim", [ctp])

    if flags.Trigger.enableL1CaloLegacy or not flags.Trigger.enableL1Phase1:
        from TrigT1RoIB.TrigT1RoIBConfig import RoIBuilder
        roib = RoIBuilder("RoIBuilder")
        roib.DoCalo = flags.Trigger.enableL1CaloLegacy
        roib.DoMuon = not flags.Trigger.enableL1Phase1
        ctpSim += [roib]

    #l1Sim = seqAND("l1Sim", [caloTowerMaker] )
    if l1TopoSim:
      l1Sim = seqAND("l1Sim", [l1CaloSim, l1MuonSim, l1TopoSim, ctpSim] )
    else:
      l1Sim = seqAND("l1Sim", [l1CaloSim, l1MuonSim, ctpSim] )

    return l1Sim
