## @file TrigT1CaloMonitoring_forRecExCommission_cpu.py
#
# For testing purposes only - runs each monitoring tool in a separate manager
# so can see their individual cpu usage.
#
# @author Peter Faulkner
#
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
    
if l1caloRawMon:
    
    Offline = not athenaCommonFlags.isOnline
    isData  = (globalflags.DataSource() == "data")
    isCalo  = (rec.doCalo() and rec.doLArg() and rec.doTile())
    triggerConfigService = "TrigConf::TrigConfigSvc/TrigConfigSvc"

    #================================= Monitoring configuration ======================
    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()
    L1CaloMan0A1 = AthenaMonManager( "L1CaloMonManager0A1" )
    L1CaloMan0A2 = AthenaMonManager( "L1CaloMonManager0A2" )
    L1CaloMan0A3 = AthenaMonManager( "L1CaloMonManager0A3" )
    L1CaloMan0A4 = AthenaMonManager( "L1CaloMonManager0A4" )
    L1CaloMan0B = AthenaMonManager( "L1CaloMonManager0B" )
    L1CaloMan1A = AthenaMonManager( "L1CaloMonManager1A" )
    L1CaloMan1B = AthenaMonManager( "L1CaloMonManager1B" )
    L1CaloMan1C = AthenaMonManager( "L1CaloMonManager1C" )
    L1CaloMan2 = AthenaMonManager( "L1CaloMonManager2" )
    L1CaloMan3 = AthenaMonManager( "L1CaloMonManager3" )
    L1CaloMan4 = AthenaMonManager( "L1CaloMonManager4" )
    L1CaloMan5 = AthenaMonManager( "L1CaloMonManager5" )
    L1CaloMan6 = AthenaMonManager( "L1CaloMonManager6" )
    L1CaloMan7 = AthenaMonManager( "L1CaloMonManager7" )
    L1CaloMan8 = AthenaMonManager( "L1CaloMonManager8" )
    L1CaloMan9 = AthenaMonManager( "L1CaloMonManager9" )
    L1CaloManA = AthenaMonManager( "L1CaloMonManagerA" )
    L1CaloManB = AthenaMonManager( "L1CaloMonManagerB" )
    L1CaloManC = AthenaMonManager( "L1CaloMonManagerC" )
    L1CaloManD = AthenaMonManager( "L1CaloMonManagerD" )
    
    ## get a handle on the ToolSvc
    #from AthenaCommon.AppMgr import ToolSvc
    
    if globalflags.InputFormat() == "bytestream":
        include ("TrigT1CaloByteStream/ReadLVL1CaloBS_jobOptions.py")

    # Make sure our data is read in before our monitoring tools run so cpu accounted separately
    # (NB. other sub-detectors code may read some of it first and so get the cpu hit!)
    from TrigT1CaloMonitoring.TrigT1CaloMonitoringConf import TrigT1CaloBSMon
    # PPM
    L1TrigT1CaloBSMonToolA1 = TrigT1CaloBSMon(
        name = "L1TrigT1CaloBSMonToolA1",
        LoadL1CaloPPM = True
                )
    #ToolSvc += L1TrigT1CaloBSMonToolA1
    L1CaloMan0A1.AthenaMonTools += [ L1TrigT1CaloBSMonToolA1 ]
    # CPM
    L1TrigT1CaloBSMonToolA2 = TrigT1CaloBSMon(
        name = "L1TrigT1CaloBSMonToolA2",
        LoadL1CaloCPM = True
                )
    #ToolSvc += L1TrigT1CaloBSMonToolA2
    L1CaloMan0A2.AthenaMonTools += [ L1TrigT1CaloBSMonToolA2 ]
    # JEM
    L1TrigT1CaloBSMonToolA3 = TrigT1CaloBSMon(
        name = "L1TrigT1CaloBSMonToolA3",
        LoadL1CaloJEM = True
                )
    #ToolSvc += L1TrigT1CaloBSMonToolA3
    L1CaloMan0A3.AthenaMonTools += [ L1TrigT1CaloBSMonToolA3 ]
    # ROD headers
    L1TrigT1CaloBSMonToolA4 = TrigT1CaloBSMon(
        name = "L1TrigT1CaloBSMonToolA4",
        LoadL1CaloROD = True
                )
    #ToolSvc += L1TrigT1CaloBSMonToolA4
    L1CaloMan0A4.AthenaMonTools += [ L1TrigT1CaloBSMonToolA4 ]
    
    include("CaloConditions/CaloConditions_jobOptions.py")
    if Offline:
        include("LArDetDescr/LArDetDescr_joboptions.py")
    else:
        include("LArConditionsCommon/LArIdMap_comm_jobOptions.py")

    if isData:
        include("TrigT1CaloCalibConditions/L1CaloCalibConditionsTier0_jobOptions.py")

    ## Preload CaloCell data
    L1TrigT1CaloBSMonToolB = TrigT1CaloBSMon(
        name = "L1TrigT1CaloBSMonToolB",
        LoadCaloCells = True
                )
    #ToolSvc += L1TrigT1CaloBSMonToolB
    L1CaloMan0B.AthenaMonTools += [ L1TrigT1CaloBSMonToolB ]
    
    from TrigT1CaloMonitoringTools.LVL1CaloMonFlags import LVL1CaloMonFlags

    doFineTime = False
    if isData and isCalo and ((LVL1CaloMonFlags.doPPrStabilityMon() and
       LVL1CaloMonFlags.doFineTimeMonitoring()) or (Offline and rec.triggerStream() == "express")):
        # load the sqlite file for the fine time monitoring
        dbpath = "/afs/cern.ch/user/l/l1ccalib/w0/DaemonData/reference/calibJuly.sqlite"
        import os.path
        if os.path.isfile(dbpath):
            from AthenaServices.AthenaServicesConf import MetaDataSvc
            svcMgr += MetaDataSvc( "MetaDataSvc" )
            #svcMgr.IOVDbSvc.Folders += ["<dbConnection>sqlite://;schema=../share/calibJuly.sqlite;dbname=L1CALO</dbConnection>/TRIGGER/L1Calo/V1/References/FineTimeReferences"]
            svcMgr.IOVDbSvc.Folders += ["<dbConnection>sqlite://;schema=/afs/cern.ch/user/l/l1ccalib/w0/DaemonData/reference/calibJuly.sqlite;dbname=L1CALO</dbConnection>/TRIGGER/L1Calo/V1/References/FineTimeReferences"]
            doFineTime = True


    if LVL1CaloMonFlags.doPPrStabilityMon():        

        #=================================================================================
        #  Want Full PPrStabilityMon to run alone
        #=================================================================================

        from TrigT1CaloMonitoring.TrigT1CaloMonitoringConf import PPrStabilityMon
        L1PPrStabilityMonTool = PPrStabilityMon(
            name = "L1PPrStabilityMonTool",
            doFineTimeMonitoring = doFineTime,
            doPedestalMonitoring = LVL1CaloMonFlags.doPedestalMonitoring(),
            doEtCorrelationMonitoring = (LVL1CaloMonFlags.doEtCorrelationMonitoring() and isCalo),
            BS_TriggerTowerContainer = "TriggerTowers",
            ppmADCMinValue = 60,
            lumiMax = 2000,
            fineTimeCut = 20,
            PathInRootFile = "L1Calo/PPrStabilityMon",
            #OutputLevel = DEBUG
            )
        #ToolSvc += L1PPrStabilityMonTool
        L1CaloMan1A.AthenaMonTools += [ L1PPrStabilityMonTool ]
    
    else:
    
        if isData and Offline and rec.triggerStream() == "express":
    
            #=================================================================================
            #============== PPrStabilityMon without individual channel plots =================
            #=================================================================================
            from TrigT1CaloMonitoring.TrigT1CaloMonitoringConf import PPrStabilityMon
            L1PPrStabilityMonToolA = PPrStabilityMon(
                name = "L1PPrStabilityMonToolA",
                doFineTimeMonitoring = doFineTime,
                doPedestalMonitoring = False,
                doEtCorrelationMonitoring = False,
                BS_TriggerTowerContainer = "TriggerTowers",
                ppmADCMinValue = 60,
                lumiMax = 2000,
                fineTimeCut = 20,
                PathInRootFile = "L1Calo/PPM/Stability",
                #OutputLevel = DEBUG
                )
            #ToolSvc += L1PPrStabilityMonToolA
            L1CaloMan1A.AthenaMonTools += [ L1PPrStabilityMonToolA ]

            L1PPrStabilityMonToolB = PPrStabilityMon(
                name = "L1PPrStabilityMonToolB",
                doFineTimeMonitoring = False,
                doPedestalMonitoring = True,
                doEtCorrelationMonitoring = False,
                BS_TriggerTowerContainer = "TriggerTowers",
                ppmADCMinValue = 60,
                lumiMax = 2000,
                fineTimeCut = 20,
                PathInRootFile = "L1Calo/PPM/Stability",
                #OutputLevel = DEBUG
                )
            #ToolSvc += L1PPrStabilityMonToolB
            L1CaloMan1B.AthenaMonTools += [ L1PPrStabilityMonToolB ]

            L1PPrStabilityMonToolC = PPrStabilityMon(
                name = "L1PPrStabilityMonToolC",
                doFineTimeMonitoring = False,
                doPedestalMonitoring = False,
                doEtCorrelationMonitoring = isCalo,
                BS_TriggerTowerContainer = "TriggerTowers",
                ppmADCMinValue = 60,
                lumiMax = 2000,
                fineTimeCut = 20,
                PathInRootFile = "L1Calo/PPM/Stability",
                #OutputLevel = DEBUG
                )
            #ToolSvc += L1PPrStabilityMonToolC
            L1CaloMan1C.AthenaMonTools += [ L1PPrStabilityMonToolC ]
    
        #=================================================================================
        #================================= PPr ===========================================
        #=================================================================================
        from TrigT1CaloMonitoring.TrigT1CaloMonitoringConf import PPrMon
        L1PPrMonTool = PPrMon(
            name = "L1PPrMonTool",
            BS_TriggerTowerContainer = "TriggerTowers",
            LUTHitMap_ThreshVec=[0, 1, 3, 5, 10, 20, 35, 50],
            LUTHitMap_LumiBlocks = 10,
            ADCHitMap_Thresh = 50,
            MaxEnergyRange = 256,
            EMFADCCut = 40,
            HADFADCCut = 40,
            ADCPedestal = 32,
            PathInRootFile = "L1Calo/PPM",
            ErrorPathInRootFile = "L1Calo/PPM/Errors",
            #OutputLevel = DEBUG
            )
        #ToolSvc += L1PPrMonTool
        L1CaloMan2.AthenaMonTools += [ L1PPrMonTool ]

        if isData:
                
            from TrigT1CaloMonitoring.TrigT1CaloMonitoringConf import PPMSimBSMon
            PPMSimBSMonTool = PPMSimBSMon("PPMSimBSMonTool")
            #ToolSvc += PPMSimBSMonTool
            L1CaloMan3.AthenaMonTools += [ PPMSimBSMonTool ]
            #ToolSvc.PPMSimBSMonTool.OutputLevel = DEBUG
            from TrigT1CaloTools.TrigT1CaloToolsConf import LVL1__L1TriggerTowerTool
            L1TriggerTowerTool = LVL1__L1TriggerTowerTool("L1TriggerTowerTool")
            ToolSvc += L1TriggerTowerTool
            #ToolSvc.L1TriggerTowerTool.OutputLevel = DEBUG
            
            #--------------------------------- PPM Spare Channels----------------------------
            from TrigT1CaloMonitoring.TrigT1CaloMonitoringConf import PPrSpareMon
            L1PPrSpareMonTool = PPrSpareMon(
                name = "L1PPrSpareMonTool",
                BS_TriggerTowerContainer = "TriggerTowersSpare",
                ADCHitMap_Thresh = 40,
                PathInRootFile = "L1Calo/PPM/SpareChannels",
                ErrorPathInRootFile = "L1Calo/PPM/SpareChannels/Errors",
                #OutputLevel = DEBUG
                )
            #ToolSvc += L1PPrSpareMonTool
            L1CaloMan4.AthenaMonTools += [ L1PPrSpareMonTool ]
    
        #=================================================================================
        #=================================== JEP =========================================
        #=================================================================================
    
        #------------------------------------ JEM ----------------------------------------
        from TrigT1CaloMonitoring.TrigT1CaloMonitoringConf import JEMMon
        L1JEMMonTool = JEMMon(
            name = "L1JEMMonTool",
            JetElementLocation = "JetElements",
            JEMHitsLocation = "JEMHits",
            JEMEtSumsLocation = "JEMEtSums",
            JEMRoILocation = "JEMRoIs",
            MaxEnergyRange = 1024,
            PathInRootFile = "L1Calo/JEM",
            ErrorPathInRootFile = "L1Calo/JEM/Errors/Hardware",
            #OutputLevel = DEBUG
            )
        #ToolSvc += L1JEMMonTool
        L1CaloMan5.AthenaMonTools += [ L1JEMMonTool ]
    
        #----------------------------------- CMM ------------------------------------------
        from TrigT1CaloMonitoring.TrigT1CaloMonitoringConf import CMMMon
        L1CMMMonTool = CMMMon (
            name = "L1CMMMonTool",
            CMMJetHitsLocation = "CMMJetHits",
            CMMEtSumsLocation = "CMMEtSums",
            CMMRoILocation = "CMMRoIs",
            PathInRootFile = "L1Calo/JEM_CMM",
            ErrorPathInRootFile = "L1Calo/JEM_CMM/Errors/Hardware",
            #OutputLevel = DEBUG
            )
        #ToolSvc += L1CMMMonTool
        L1CaloMan6.AthenaMonTools += [ L1CMMMonTool ]
    
        if isData:
    
            #--------------------- Transmission and Performance ------------------------------
            from TrigT1CaloMonitoring.TrigT1CaloMonitoringConf import JEPSimBSMon
            JEPSimBSMonTool = JEPSimBSMon("JEPSimBSMonTool",
                JEPHitsTool = "LVL1::L1JEPHitsTools/L1JEPHitsTools_Mon",
                JetTool = "LVL1::L1JetTools/L1JetTools_Mon",
                JEPEtSumsTool = "LVL1::L1JEPEtSumsTools/L1JEPEtSumsTools_Mon",
                )
            #ToolSvc += JEPSimBSMonTool
            L1CaloMan7.AthenaMonTools += [ JEPSimBSMonTool ]
            #ToolSvc.JEPSimBSMonTool.OutputLevel = DEBUG
    
            from TrigT1CaloTools.TrigT1CaloToolsConf import LVL1__L1JEPHitsTools
            L1JEPHitsTools = LVL1__L1JEPHitsTools("L1JEPHitsTools_Mon")
            L1JEPHitsTools.LVL1ConfigSvc = triggerConfigService
            ToolSvc += L1JEPHitsTools
            from TrigT1CaloTools.TrigT1CaloToolsConf import LVL1__L1JetTools
            L1JetTools = LVL1__L1JetTools("L1JetTools_Mon")
            L1JetTools.LVL1ConfigSvc = triggerConfigService
            ToolSvc += L1JetTools
            from TrigT1CaloTools.TrigT1CaloToolsConf import LVL1__L1EtTools
            L1EtTools = LVL1__L1EtTools("L1EtTools_Mon")
            L1EtTools.LVL1ConfigSvc = triggerConfigService
            ToolSvc += L1EtTools
            from TrigT1CaloTools.TrigT1CaloToolsConf import LVL1__L1JEPEtSumsTools
            L1JEPEtSumsTools = LVL1__L1JEPEtSumsTools("L1JEPEtSumsTools_Mon",
                                                EtTool = "LVL1::L1EtTools/L1EtTools_Mon")
            L1JEPEtSumsTools.LVL1ConfigSvc = triggerConfigService
            ToolSvc += L1JEPEtSumsTools
    
        #=================================================================================
        #===================================== CP ========================================
        #=================================================================================
        from TrigT1CaloMonitoring.TrigT1CaloMonitoringConf import TrigT1CaloCpmMonTool
        L1BSCPMMonTool = TrigT1CaloCpmMonTool (
            name = "L1BSCPMMonTool",
            TriggerTowerLocation = "TriggerTowers",
            CPMTowerLocation = "CPMTowers",
            CPMHitsLocation = "CPMHits",
            CMMCPHitsLocation = "CMMCPHits",
            CPMRoILocation = "CPMRoIs",
            RootDirectory = "L1Calo",
            MaxEnergyRange = 256,
            #OutputLevel = DEBUG,
            )
        #ToolSvc += L1BSCPMMonTool
        L1CaloMan8.AthenaMonTools += [ L1BSCPMMonTool ]

        if isData:
    
            from TrigT1CaloMonitoring.TrigT1CaloMonitoringConf import CPMSimBSMon
            CPMSimBSMonTool = CPMSimBSMon("CPMSimBSMonTool",
                                      EmTauTool = "LVL1::L1EmTauTools/L1EmTauTools_Mon",
                                      )
            #ToolSvc += CPMSimBSMonTool
            L1CaloMan9.AthenaMonTools += [ CPMSimBSMonTool ]
            #ToolSvc.CPMSimBSMonTool.OutputLevel = DEBUG
    
            from TrigT1CaloTools.TrigT1CaloToolsConf import LVL1__L1EmTauTools
            L1EmTauTools = LVL1__L1EmTauTools("L1EmTauTools_Mon")
            L1EmTauTools.LVL1ConfigSvc = triggerConfigService
            ToolSvc += L1EmTauTools
    
            #=================================================================================
            #===================================== ROD =======================================
            #=================================================================================
            from TrigT1CaloMonitoring.TrigT1CaloMonitoringConf import TrigT1CaloRodMonTool
            L1BSRODMonTool = TrigT1CaloRodMonTool (
                name = "L1BSRODMonTool",
                #OnlineTest = True,
                #OutputLevel = DEBUG,
                )
            #ToolSvc += L1BSRODMonTool
            L1CaloManA.AthenaMonTools += [ L1BSRODMonTool ]
    
            #=================================================================================
            #=============================== Global Overview =================================
            #=================================================================================
            #from TrigT1CaloMonitoringTools.TrigT1CaloMonitoringToolsConf import TrigT1CaloMonErrorTool
            #L1MonErrorTool = TrigT1CaloMonErrorTool( name = "TrigT1CaloMonErrorTool",
            #                                         FlagCorruptEvents = "None"
            #                                         )
            #ToolSvc += L1MonErrorTool
            from TrigT1CaloMonitoring.TrigT1CaloMonitoringConf import TrigT1CaloGlobalMonTool
            L1GlobalMonTool = TrigT1CaloGlobalMonTool ( name = "L1GlobalMonTool",
    						        #OnlineTest = True,
    						        #OutputLevel = DEBUG
                                                          )
            #ToolSvc += L1GlobalMonTool
            L1CaloManB.AthenaMonTools += [ L1GlobalMonTool ]
    
        if isData and isCalo and Offline and (rec.triggerStream() == "JetTauEtmiss"
                                           or rec.triggerStream() == "Muons"
                                           or rec.triggerStream() == "express"):
    
            #=================================================================================
            #=============================== EM Efficiencies =================================
            #=================================================================================
            trigstring = ['EF_.*']
            from TrigT1CaloMonitoring.TrigT1CaloMonitoringConf import EmEfficienciesMonTool
            L1EmEfficienciesMonTool = EmEfficienciesMonTool ( name = "EmEfficienciesMonTool",
                                                                  TriggerStrings = trigstring
                                                            )
            #ToolSvc += L1EmEfficienciesMonTool
            L1CaloManC.AthenaMonTools += [ L1EmEfficienciesMonTool ]
            if not hasattr( ToolSvc, "TrigDecisionTool" ):
                from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
                tdt = Trig__TrigDecisionTool('TrigDecisionTool')
                ToolSvc += tdt
    
        if isData and isCalo and Offline and (rec.triggerStream() == "Egamma"
                                           or rec.triggerStream() == "Muons"
                                           or rec.triggerStream() == "express"):
    
            #=================================================================================
            #=============================== Jet Efficiencies ================================
            #=================================================================================
            trigstring = ['EF_.*']
            from TrigT1CaloMonitoring.TrigT1CaloMonitoringConf import JetEfficienciesMonTool
            L1JetEfficienciesMonTool = JetEfficienciesMonTool ( name = "JetEfficienciesMonTool",
                                                                  TriggerStrings = trigstring
                                                              )
            ToolSvc += L1JetEfficienciesMonTool
            L1CaloManD.AthenaMonTools += [ L1JetEfficienciesMonTool ]
            if not hasattr( #ToolSvc, "TrigDecisionTool" ):
                from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
                tdt = Trig__TrigDecisionTool('TrigDecisionTool')
                ToolSvc += tdt
    
    #=================================================================================
    # FileKey must match that given to THistSvc
    L1CaloMan0A1.FileKey             = DQMonFlags.monManFileKey()
    L1CaloMan0A1.Environment         = DQMonFlags.monManEnvironment()
    L1CaloMan0A1.ManualDataTypeSetup = DQMonFlags.monManManualDataTypeSetup()
    L1CaloMan0A1.DataType            = DQMonFlags.monManDataType()
    L1CaloMan0A2.FileKey             = DQMonFlags.monManFileKey()
    L1CaloMan0A2.Environment         = DQMonFlags.monManEnvironment()
    L1CaloMan0A2.ManualDataTypeSetup = DQMonFlags.monManManualDataTypeSetup()
    L1CaloMan0A2.DataType            = DQMonFlags.monManDataType()
    L1CaloMan0A3.FileKey             = DQMonFlags.monManFileKey()
    L1CaloMan0A3.Environment         = DQMonFlags.monManEnvironment()
    L1CaloMan0A3.ManualDataTypeSetup = DQMonFlags.monManManualDataTypeSetup()
    L1CaloMan0A3.DataType            = DQMonFlags.monManDataType()
    L1CaloMan0A4.FileKey             = DQMonFlags.monManFileKey()
    L1CaloMan0A4.Environment         = DQMonFlags.monManEnvironment()
    L1CaloMan0A4.ManualDataTypeSetup = DQMonFlags.monManManualDataTypeSetup()
    L1CaloMan0A4.DataType            = DQMonFlags.monManDataType()
    # Make sure we run these before RoIBResultToAOD as it also accesses
    # TriggerTowers and JetElements
    index = 0
    for alg in topSequence: 
        if alg.getName() == 'RoIBResultToAOD': break
        index += 1
    topSequence.insert(index,   L1CaloMan0A1)
    topSequence.insert(index+1, L1CaloMan0A2)
    topSequence.insert(index+2, L1CaloMan0A3)
    topSequence.insert(index+3, L1CaloMan0A4)
    L1CaloMan0B.FileKey             = DQMonFlags.monManFileKey()
    L1CaloMan0B.Environment         = DQMonFlags.monManEnvironment()
    L1CaloMan0B.ManualDataTypeSetup = DQMonFlags.monManManualDataTypeSetup()
    L1CaloMan0B.DataType            = DQMonFlags.monManDataType()
    topSequence += L1CaloMan0B
    L1CaloMan1A.FileKey             = DQMonFlags.monManFileKey()
    L1CaloMan1A.Environment         = DQMonFlags.monManEnvironment()
    L1CaloMan1A.ManualDataTypeSetup = DQMonFlags.monManManualDataTypeSetup()
    L1CaloMan1A.DataType            = DQMonFlags.monManDataType()
    topSequence += L1CaloMan1A
    L1CaloMan1B.FileKey             = DQMonFlags.monManFileKey()
    L1CaloMan1B.Environment         = DQMonFlags.monManEnvironment()
    L1CaloMan1B.ManualDataTypeSetup = DQMonFlags.monManManualDataTypeSetup()
    L1CaloMan1B.DataType            = DQMonFlags.monManDataType()
    topSequence += L1CaloMan1B
    L1CaloMan1C.FileKey             = DQMonFlags.monManFileKey()
    L1CaloMan1C.Environment         = DQMonFlags.monManEnvironment()
    L1CaloMan1C.ManualDataTypeSetup = DQMonFlags.monManManualDataTypeSetup()
    L1CaloMan1C.DataType            = DQMonFlags.monManDataType()
    topSequence += L1CaloMan1C
    L1CaloMan2.FileKey             = DQMonFlags.monManFileKey()
    L1CaloMan2.Environment         = DQMonFlags.monManEnvironment()
    L1CaloMan2.ManualDataTypeSetup = DQMonFlags.monManManualDataTypeSetup()
    L1CaloMan2.DataType            = DQMonFlags.monManDataType()
    topSequence += L1CaloMan2
    L1CaloMan3.FileKey             = DQMonFlags.monManFileKey()
    L1CaloMan3.Environment         = DQMonFlags.monManEnvironment()
    L1CaloMan3.ManualDataTypeSetup = DQMonFlags.monManManualDataTypeSetup()
    L1CaloMan3.DataType            = DQMonFlags.monManDataType()
    topSequence += L1CaloMan3
    L1CaloMan4.FileKey             = DQMonFlags.monManFileKey()
    L1CaloMan4.Environment         = DQMonFlags.monManEnvironment()
    L1CaloMan4.ManualDataTypeSetup = DQMonFlags.monManManualDataTypeSetup()
    L1CaloMan4.DataType            = DQMonFlags.monManDataType()
    topSequence += L1CaloMan4
    L1CaloMan5.FileKey             = DQMonFlags.monManFileKey()
    L1CaloMan5.Environment         = DQMonFlags.monManEnvironment()
    L1CaloMan5.ManualDataTypeSetup = DQMonFlags.monManManualDataTypeSetup()
    L1CaloMan5.DataType            = DQMonFlags.monManDataType()
    topSequence += L1CaloMan5
    L1CaloMan6.FileKey             = DQMonFlags.monManFileKey()
    L1CaloMan6.Environment         = DQMonFlags.monManEnvironment()
    L1CaloMan6.ManualDataTypeSetup = DQMonFlags.monManManualDataTypeSetup()
    L1CaloMan6.DataType            = DQMonFlags.monManDataType()
    topSequence += L1CaloMan6
    L1CaloMan7.FileKey             = DQMonFlags.monManFileKey()
    L1CaloMan7.Environment         = DQMonFlags.monManEnvironment()
    L1CaloMan7.ManualDataTypeSetup = DQMonFlags.monManManualDataTypeSetup()
    L1CaloMan7.DataType            = DQMonFlags.monManDataType()
    topSequence += L1CaloMan7
    L1CaloMan8.FileKey             = DQMonFlags.monManFileKey()
    L1CaloMan8.Environment         = DQMonFlags.monManEnvironment()
    L1CaloMan8.ManualDataTypeSetup = DQMonFlags.monManManualDataTypeSetup()
    L1CaloMan8.DataType            = DQMonFlags.monManDataType()
    topSequence += L1CaloMan8
    L1CaloMan9.FileKey             = DQMonFlags.monManFileKey()
    L1CaloMan9.Environment         = DQMonFlags.monManEnvironment()
    L1CaloMan9.ManualDataTypeSetup = DQMonFlags.monManManualDataTypeSetup()
    L1CaloMan9.DataType            = DQMonFlags.monManDataType()
    topSequence += L1CaloMan9
    L1CaloManA.FileKey             = DQMonFlags.monManFileKey()
    L1CaloManA.Environment         = DQMonFlags.monManEnvironment()
    L1CaloManA.ManualDataTypeSetup = DQMonFlags.monManManualDataTypeSetup()
    L1CaloManA.DataType            = DQMonFlags.monManDataType()
    topSequence += L1CaloManA
    L1CaloManB.FileKey             = DQMonFlags.monManFileKey()
    L1CaloManB.Environment         = DQMonFlags.monManEnvironment()
    L1CaloManB.ManualDataTypeSetup = DQMonFlags.monManManualDataTypeSetup()
    L1CaloManB.DataType            = DQMonFlags.monManDataType()
    topSequence += L1CaloManB
    L1CaloManC.FileKey             = DQMonFlags.monManFileKey()
    L1CaloManC.Environment         = DQMonFlags.monManEnvironment()
    L1CaloManC.ManualDataTypeSetup = DQMonFlags.monManManualDataTypeSetup()
    L1CaloManC.DataType            = DQMonFlags.monManDataType()
    topSequence += L1CaloManC
    L1CaloManD.FileKey             = DQMonFlags.monManFileKey()
    L1CaloManD.Environment         = DQMonFlags.monManEnvironment()
    L1CaloManD.ManualDataTypeSetup = DQMonFlags.monManManualDataTypeSetup()
    L1CaloManD.DataType            = DQMonFlags.monManDataType()
    topSequence += L1CaloManD
