## @file TrigT1Monitoring_forRecExCommission.py
#
# Standard monitoring jobOptions - runs on Tier0 (Reco_trf.py) or online.
#
# @authors Johanna Fleckner, Andrea Neusiedl, Peter Faulkner
#
from AthenaCommon.GlobalFlags import globalflags
if not 'DQMonFlags' in dir():
    print "TrigT1CaloMonitoring_forRecExCommission.py: DQMonFlags not yet imported - I import them now"
    from AthenaMonitoring.DQMonFlags import DQMonFlags

# On Tier0 select monitoring tools according to processing step
if DQMonFlags.monManEnvironment() == 'tier0Raw':
    # Tier0 RAWtoESD step
    l1caloRawMon=True
    l1caloESDMon=False
elif DQMonFlags.monManEnvironment() == 'tier0ESD':
    # Tier0 ESDtoAOD step
    l1caloRawMon=False
    l1caloESDMon=True
else:
    # Anything else
    l1caloRawMon=True
    l1caloESDMon=True

# Don't run if full PPrStabilityMon running
from TrigT1CaloMonitoringTools.LVL1CaloMonFlags import LVL1CaloMonFlags
if LVL1CaloMonFlags.doPPrStabilityMon():
    l1caloRawMon=False
    l1caloESDMon=False

if l1caloRawMon:

    Online = athenaCommonFlags.isOnline
    isData = (globalflags.DataSource() == "data")
    isCalo = (rec.doCalo() and rec.doLArg() and rec.doTile())

    #================================= Monitoring configuration ======================
    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()
    topSequence += AthenaMonManager( "L1MonManager" )
    L1Man = topSequence.L1MonManager

    ## get a handle on the ToolSvc
    from AthenaCommon.AppMgr import ToolSvc

    if globalflags.InputFormat() == "bytestream":
        include ("TrigT1CaloByteStream/ReadLVL1CaloBS_jobOptions.py")
        if not hasattr( svcMgr, "ByteStreamAddressProviderSvc" ):
            from ByteStreamCnvSvcBase.ByteStreamCnvSvcBaseConf import ByteStreamAddressProviderSvc 
            svcMgr += ByteStreamAddressProviderSvc()
        svcMgr.ByteStreamAddressProviderSvc.TypeNames += [ "CTP_RDO/CTP_RDO" ]
        svcMgr.ByteStreamAddressProviderSvc.TypeNames += [ "ROIB::RoIBResult/RoIBResult" ]
    
    if isData and isCalo and (Online or rec.triggerStream() == "express"
                                     or rec.triggerStream() == "JetTauEtmiss"):

        ####################### Calorimeter -> L1Calo ################################
        include("CaloConditions/CaloConditions_jobOptions.py")
        include("LArDetDescr/LArDetDescr_joboptions.py")
        # CaloCells
        from TrigT1Monitoring.TrigT1MonitoringConf import CalorimeterL1CaloMon
        CalorimeterL1CaloMonTool = CalorimeterL1CaloMon(
            name = "CalorimeterL1CaloMonTool",
            PathInRootFile = "LVL1_Interfaces/Calorimeter",
            CaloThreshold = 0.1,
            Calo_HitMap_Thresh0 = 1,
            Calo_HitMap_Thresh1 = 3,
            Calo_HitMap_Thresh2 = 7,
            MaxEnergyRange = 256,
            #OutputLevel = VERBOSE,
            )
        ToolSvc += CalorimeterL1CaloMonTool
        L1Man.AthenaMonTools += [ CalorimeterL1CaloMonTool ]
        # HV corrections
        from LArRecUtils.LArHVCorrToolDefault import LArHVCorrToolDefault
        theLArHVCorrTool = LArHVCorrToolDefault()
        ToolSvc += theLArHVCorrTool
        include("TrigT1CaloCalibConditions/L1CaloCalibConditionsTier0_jobOptions.py")
        # Check reference database is available, if not don't try to read it
        dbpath = "/afs/cern.ch/user/l/l1ccalib/w0/DaemonData/reference/hvcorrections.sqlite"
        import os.path
        if os.path.isfile(dbpath):
            from IOVDbSvc.CondDB import conddb
            conddb.addFolder("", "<dbConnection>sqlite://;schema=" + dbpath + ";dbname=L1CALO</dbConnection>/TRIGGER/L1Calo/V1/Results/HVCorrections<tag>HEAD</tag>")
            conddb.addFolder("", "<dbConnection>sqlite://;schema=" + dbpath + ";dbname=L1CALO</dbConnection>/TRIGGER/L1Calo/V1/Results/RxLayers<tag>HEAD</tag>")
            doHV = True
        else:
            doHV = False
        from TrigT1Monitoring.TrigT1MonitoringConf import L1CaloHVScalesMon
        L1CaloHVScalesMonTool = L1CaloHVScalesMon(
            name = "L1CaloHVScalesMonTool",
            LArHVCorrTool = theLArHVCorrTool,
            DoHVDifference = doHV,
            PathInRootFile = "LVL1_Interfaces/Calorimeter",
            )
        ToolSvc += L1CaloHVScalesMonTool
        L1Man.AthenaMonTools += [ L1CaloHVScalesMonTool ]
        # PMT scores
        from TrigT1Monitoring.TrigT1MonitoringConf import L1CaloPMTScoresMon
        L1CaloPMTScoresMonTool = L1CaloPMTScoresMon(
            name = "L1CaloPMTScoresMonTool",
            PathInRootFile = "LVL1_Interfaces/Calorimeter",
            )
        ToolSvc += L1CaloPMTScoresMonTool
        L1Man.AthenaMonTools += [ L1CaloPMTScoresMonTool ]

    if isData and DQMonFlags.doCTPMon():

        ####################### L1Calo->CTP ################################
        from IOVDbSvc.CondDB import conddb
        conddb.addFolderWithTag("TRIGGER", "/TRIGGER/LVL1/CTPCoreInputMapping", "HEAD")

        from TrigT1Monitoring.TrigT1MonitoringConf import L1CaloCTPMon
        L1CaloCTPMonTool = L1CaloCTPMon(
            name = "L1CaloCTPMonTool",
            #OutputLevel = DEBUG,
            #OutputLevel = VERBOSE,
            )
        ToolSvc += L1CaloCTPMonTool
        L1Man.AthenaMonTools += [ L1CaloCTPMonTool ]

    ####################### L1Calo->Level-2 ################################
    from TrigT1Monitoring.TrigT1MonitoringConf import L1CaloLevel2Mon
    L1CaloLevel2MonTool = L1CaloLevel2Mon(
        name = "L1CaloLevel2MonTool",
        #OutputLevel = DEBUG,
        )
    ToolSvc += L1CaloLevel2MonTool
    L1Man.AthenaMonTools += [ L1CaloLevel2MonTool ]

    ##########################################################################
    # FileKey must match that given to THistSvc
    L1Man.FileKey             = DQMonFlags.monManFileKey()
    L1Man.Environment         = DQMonFlags.monManEnvironment()
    L1Man.ManualDataTypeSetup = DQMonFlags.monManManualDataTypeSetup()
    L1Man.DataType            = DQMonFlags.monManDataType()
