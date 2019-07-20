# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration


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
    from AthenaCommon.CFElements import seqAND
    from AthenaCommon.AppMgr import ServiceMgr as svcMgr
    from AthenaCommon.AlgSequence import AthSequencer
    from TriggerJobOpts.TriggerFlags import TriggerFlags
    from AthenaCommon.Constants import DEBUG
    
    TriggerFlags.readLVL1configFromXML = True
    TriggerFlags.outputLVL1configFile = None
    from TrigConfigSvc.TrigConfigSvcConfig import LVL1ConfigSvc, findFileInXMLPATH
    svcMgr += LVL1ConfigSvc()
    svcMgr.LVL1ConfigSvc.XMLMenuFile = findFileInXMLPATH(TriggerFlags.inputLVL1configFile())

    # L1 menu provider Run 3
    from TrigConfIO.TrigConfCondSetup import setupMenuProvider
    setupMenuProvider()


    
    from TrigT1CaloSim.TrigT1CaloSimRun2Config import Run2TriggerTowerMaker
    caloTowerMaker              = Run2TriggerTowerMaker("Run2TriggerTowerMaker25ns")
    caloTowerMaker.ExtraInputs   = ["LArTTL1Container#LArTTL1EM", "LArTTL1Container#LArTTL1HAD", "TileTTL1Container#TileTTL1Cnt" ]
    caloTowerMaker.OutputLevel=DEBUG
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
    # muons
    
    from AthenaCommon.Include import include ## TODO, see if can be replaced by new JO
    include( "MuonByteStreamCnvTest/jobOptions_MuonRDOToDigit.py" )    
    from MuonByteStreamCnvTest.MuonByteStreamCnvTestConf import MuonRdoToMuonDigitTool
    MuonRdoToMuonDigitTool = MuonRdoToMuonDigitTool (DecodeMdtRDO = False,
                                                     DecodeRpcRDO = True,
                                                     DecodeTgcRDO = True,
                                                     DecodeCscRDO = False ) 
    
    MuonRdoToMuonDigitTool.cscCalibTool = "CscCalibTool"
    from AthenaCommon.AppMgr import ToolSvc
    ToolSvc += MuonRdoToMuonDigitTool

    from MuonByteStreamCnvTest.MuonByteStreamCnvTestConf import MuonRdoToMuonDigit
    from TrigT1RPCsteering.TrigT1RPCsteeringConf import TrigT1RPC    
    from TrigT1TGC.TrigT1TGCConf import LVL1TGCTrigger__LVL1TGCTrigger
    from TrigT1Muctpi.TrigT1MuctpiConfig import L1Muctpi
    from TrigT1Muctpi.TrigT1MuctpiConfig import L1MuctpiTool

    ToolSvc += L1MuctpiTool("L1MuctpiTool")
    ToolSvc.L1MuctpiTool.LVL1ConfigSvc = svcMgr.LVL1ConfigSvc
    
    ToolSvc += L1MuctpiTool("LVL1MUCTPI__L1MuctpiTool") # one for topo, no idea why we need two
    ToolSvc.LVL1MUCTPI__L1MuctpiTool.LVL1ConfigSvc = svcMgr.LVL1ConfigSvc        

    muctpi             = L1Muctpi()
    muctpi.LVL1ConfigSvc = svcMgr.LVL1ConfigSvc
    
    l1MuonSim = seqAND("l1MuonSim", [
        
        MuonRdoToMuonDigit( "MuonRdoToMuonDigit",
                            MuonRdoToMuonDigitTool = ToolSvc.MuonRdoToMuonDigitTool),

        TrigT1RPC("TrigT1RPC",
                  Hardware          = True, # not sure if needed, not there in old config, present in JO
                  DataDetail        = False,
                  RPCbytestream     = False,
                  RPCbytestreamFile = ""),
        
        # based on Trigger/TrigT1/TrigT1TGC/python/TrigT1TGCConfig.py
        # interesting is that this JO sets inexisting properties, commented out below
        LVL1TGCTrigger__LVL1TGCTrigger("LVL1TGCTrigger",
                                       InputData_perEvent  = "TGC_DIGITS", 
                                      # ASDOutDataLocation = "ASDOutDataLocation",
                                      # MuonTrigConfig     = "/Run/MuonTrigConfig",
                                       MuCTPIInput_TGC     = "L1MuctpiStoreTGC",
                                       MaskFileName        = "TrigT1TGCMaskedChannel.db",
                                       MaskFileName12      = "TrigT1TGCMaskedChannel._12.db"),
        muctpi
    ])
    
    # only needed for MC
    conddb.addFolder("TGC_OFL", "/TGC/TRIGGER/CW_EIFI", className="CondAttrListCollection")
    conddb.addFolder("TGC_OFL", "/TGC/TRIGGER/CW_BW", className="CondAttrListCollection")
    conddb.addFolder("TGC_OFL", "/TGC/TRIGGER/CW_TILE", className="CondAttrListCollection")
    from TrigT1CTP.TrigT1CTPConfig import CTPSimulationInReco
    from TrigT1RoIB.TrigT1RoIBConfig import RoIBuilder
    condSeq = AthSequencer("AthCondSeq")
    from MuonCondSvc.MuonCondSvcConf import TGCTriggerDbAlg
    condSeq += TGCTriggerDbAlg()

    ctp             = CTPSimulationInReco("CTPSimulation")
    ctp.DoLUCID     = False
    ctp.DoBCM       = False
    ctp.DoL1Topo    = False
    ctp.UseCondL1Menu = True
    ctp.TrigConfigSvc = svcMgr.LVL1ConfigSvc
    ctpSim      = seqAND("ctpSim", [ctp, RoIBuilder("RoIBuilder")])

    #l1Sim = seqAND("l1Sim", [caloTowerMaker] )
    l1Sim = seqAND("l1Sim", [l1CaloSim, l1MuonSim, ctpSim] )
    return l1Sim
    
