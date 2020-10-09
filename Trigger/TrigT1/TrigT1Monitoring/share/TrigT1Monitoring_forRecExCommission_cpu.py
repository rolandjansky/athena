## @file TrigT1Monitoring_forRecExCommission_cpu.py
#
# For testing purposes only - gives cpu separately for each monitoring tool
#
# @author Peter Faulkner
#
from AthenaCommon.GlobalFlags import globalflags
if not 'DQMonFlags' in dir():
    print("TrigT1CaloMonitoring_forRecExCommission.py: DQMonFlags not yet imported - I import them now")
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
    topSequence += AthenaMonManager( "L1MonManager0A" )
    L1Man0A = topSequence.L1MonManager0A
    topSequence += AthenaMonManager( "L1MonManager0B" )
    L1Man0B = topSequence.L1MonManager0B
    topSequence += AthenaMonManager( "L1MonManager0C" )
    L1Man0C = topSequence.L1MonManager0C
    topSequence += AthenaMonManager( "L1MonManager1" )
    L1Man1 = topSequence.L1MonManager1
    topSequence += AthenaMonManager( "L1MonManager2" )
    L1Man2 = topSequence.L1MonManager2

    ## get a handle on the ToolSvc
    #from AthenaCommon.AppMgr import ToolSvc

    if globalflags.InputFormat() == "bytestream":
        include ("TrigT1CaloByteStream/ReadLVL1CaloBS_jobOptions.py")
        if not hasattr( svcMgr, "ByteStreamAddressProviderSvc" ):
            from ByteStreamCnvSvcBase.ByteStreamCnvSvcBaseConf import ByteStreamAddressProviderSvc 
            svcMgr += ByteStreamAddressProviderSvc()
        svcMgr.ByteStreamAddressProviderSvc.TypeNames += [ "CTP_RDO/CTP_RDO" ]
    
    if isData and isCalo and (Online or rec.triggerStream() == "express"
                                     or rec.triggerStream() == "JetTauEtmiss"):

        ####################### Calorimeter -> L1Calo ################################
        include("CaloConditions/CaloConditions_jobOptions.py")
        include("LArDetDescr/LArDetDescr_joboptions.py")
        # CaloCells
        from TrigT1Monitoring.TrigT1MonitoringConf import LVL1__CalorimeterL1CaloMon
        CalorimeterL1CaloMonTool = LVL1__CalorimeterL1CaloMon(
            name = "CalorimeterL1CaloMonTool",
            PathInRootFile = "LVL1_Interfaces/Calorimeter",
            CaloThreshold = 0.1,
            Calo_HitMap_Thresh0 = 1,
            Calo_HitMap_Thresh1 = 3,
            Calo_HitMap_Thresh2 = 7,
            MaxEnergyRange = 256,
            #OutputLevel = VERBOSE,
            )
        #ToolSvc += CalorimeterL1CaloMonTool
        L1Man0A.AthenaMonTools += [ CalorimeterL1CaloMonTool ]
        # HV corrections
        from LArConditionsCommon import LArHVDB
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
        from TrigT1Monitoring.TrigT1MonitoringConf import LVL1__L1CaloHVScalesMon
        L1CaloHVScalesMonTool = LVL1__L1CaloHVScalesMon(
            name = "L1CaloHVScalesMonTool",
            DoHVDifference = doHV,
            PathInRootFile = "LVL1_Interfaces/Calorimeter",
           )
        #ToolSvc += L1CaloHVScalesMonTool
        L1Man0B.AthenaMonTools += [ L1CaloHVScalesMonTool ]
        # PMT scores
        from TrigT1Monitoring.TrigT1MonitoringConf import LVL1__L1CaloPMTScoresMon
        L1CaloPMTScoresMonTool = LVL1__L1CaloPMTScoresMon(
            name = "L1CaloPMTScoresMonTool",
            PathInRootFile = "LVL1_Interfaces/Calorimeter",
            )
        #ToolSvc += L1CaloPMTScoresMonTool
        L1Man0C.AthenaMonTools += [ L1CaloPMTScoresMonTool ]

    if isData and DQMonFlags.doCTPMon():

        ####################### L1Calo->CTP ################################
        from IOVDbSvc.CondDB import conddb
        conddb.addFolderWithTag("TRIGGER", "/TRIGGER/LVL1/CTPCoreInputMapping", "HEAD")

        from TrigT1Monitoring.TrigT1MonitoringConf import LVL1__L1CaloCTPMon
        L1CaloCTPMonTool = LVL1__L1CaloCTPMon(
            name = "L1CaloCTPMonTool",
            #OutputLevel = DEBUG,
            #OutputLevel = VERBOSE,
            )
        #ToolSvc += L1CaloCTPMonTool
        L1Man1.AthenaMonTools += [ L1CaloCTPMonTool ]

    ####################### L1Calo->Level-2 ################################
    from TrigT1Monitoring.TrigT1MonitoringConf import LVL1__L1CaloLevel2Mon
    L1CaloLevel2MonTool = LVL1__L1CaloLevel2Mon(
        name = "L1CaloLevel2MonTool",
        #OutputLevel = DEBUG,
        )
    #ToolSvc += L1CaloLevel2MonTool
    L1Man2.AthenaMonTools += [ L1CaloLevel2MonTool ]

    ##########################################################################
    # FileKey must match that given to THistSvc
    L1Man0A.FileKey             = DQMonFlags.monManFileKey()
    L1Man0A.Environment         = DQMonFlags.monManEnvironment()
    L1Man0A.ManualDataTypeSetup = DQMonFlags.monManManualDataTypeSetup()
    L1Man0A.DataType            = DQMonFlags.monManDataType()
    L1Man0B.FileKey             = DQMonFlags.monManFileKey()
    L1Man0B.Environment         = DQMonFlags.monManEnvironment()
    L1Man0B.ManualDataTypeSetup = DQMonFlags.monManManualDataTypeSetup()
    L1Man0B.DataType            = DQMonFlags.monManDataType()
    L1Man0C.FileKey             = DQMonFlags.monManFileKey()
    L1Man0C.Environment         = DQMonFlags.monManEnvironment()
    L1Man0C.ManualDataTypeSetup = DQMonFlags.monManManualDataTypeSetup()
    L1Man0C.DataType            = DQMonFlags.monManDataType()
    L1Man1.FileKey              = DQMonFlags.monManFileKey()
    L1Man1.Environment          = DQMonFlags.monManEnvironment()
    L1Man1.ManualDataTypeSetup  = DQMonFlags.monManManualDataTypeSetup()
    L1Man1.DataType             = DQMonFlags.monManDataType()
    L1Man2.FileKey              = DQMonFlags.monManFileKey()
    L1Man2.Environment          = DQMonFlags.monManEnvironment()
    L1Man2.ManualDataTypeSetup  = DQMonFlags.monManManualDataTypeSetup()
    L1Man2.DataType             = DQMonFlags.monManDataType()
