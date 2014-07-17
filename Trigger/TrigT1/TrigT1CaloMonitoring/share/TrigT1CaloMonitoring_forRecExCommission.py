## @file TrigT1CaloMonitoring_forRecExCommission.py
#
# Standard monitoring jobOptions - runs on Tier0 (Reco_trf.py) or online.
#
# @authors Johanna Fleckner, Andrea Neusiedl, Peter Faulkner
#
if not 'DQMonFlags' in dir():
    print "TrigT1CaloMonitoring_forRecExCommission.py: DQMonFlags not yet imported - I import them now"
    from AthenaMonitoring.DQMonFlags import DQMonFlags
    
# On Tier0 select monitoring tools according to processing step
if DQMonFlags.monManEnvironment() == 'tier0Raw':
    # Tier0 RAWtoESD step
    l1caloRawMon = True
    l1caloESDMon = False
elif DQMonFlags.monManEnvironment() == 'tier0ESD':
    # Tier0 ESDtoAOD step
    l1caloRawMon = False
    l1caloESDMon = True
else:
    # Anything else
    l1caloRawMon = True
    l1caloESDMon = True
    
if l1caloRawMon:
    
    Offline = not athenaCommonFlags.isOnline
    isData  = (globalflags.DataSource() == "data")
    isCalo  = (rec.doCalo() and rec.doLArg() and rec.doTile())
    triggerConfigService = "TrigConf::TrigConfigSvc/TrigConfigSvc"

    #================================= Monitoring configuration ======================
    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()
    L1CaloMan = AthenaMonManager( "L1CaloMonManager" )
    
    ## get a handle on the ToolSvc
    from AthenaCommon.AppMgr import ToolSvc
    
    if globalflags.InputFormat() == "bytestream":
        include ("TrigT1CaloByteStream/ReadLVL1CaloBS_jobOptions.py")
    
    include("CaloConditions/CaloConditions_jobOptions.py")
    if Offline:
        include("LArDetDescr/LArDetDescr_joboptions.py")
    else:
        include("LArConditionsCommon/LArIdMap_comm_jobOptions.py")

    if isData:
        include("TrigT1CaloCalibConditions/L1CaloCalibConditionsTier0_jobOptions.py")
    
    from TrigT1CaloMonitoringTools.LVL1CaloMonFlags import LVL1CaloMonFlags
    
    doFineTime = False
    if isData and isCalo and ((LVL1CaloMonFlags.doPPrStabilityMon() and
       LVL1CaloMonFlags.doFineTimeMonitoring()) or (Offline and rec.triggerStream() == "express")):
        # load the sqlite file for the fine time monitoring
        dbpath = "/afs/cern.ch/user/l/l1ccalib/w0/DaemonData/reference/calibJuly.sqlite"
        import os.path
        if os.path.isfile(dbpath):
            from EventSelectorAthenaPool.EventSelectorAthenaPoolConf import MetaDataSvc
            svcMgr += MetaDataSvc( "MetaDataSvc" )
            #svcMgr.IOVDbSvc.Folders += ["<dbConnection>sqlite://;schema=../share/calibJuly.sqlite;dbname=L1CALO</dbConnection>/TRIGGER/L1Calo/V1/References/FineTimeReferences"]
            svcMgr.IOVDbSvc.Folders += ["<dbConnection>sqlite://;schema=" + dbpath + ";dbname=L1CALO</dbConnection>/TRIGGER/L1Calo/V1/References/FineTimeReferences"]
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
        ToolSvc += L1PPrStabilityMonTool
        L1CaloMan.AthenaMonTools += [ L1PPrStabilityMonTool ]
    
    else:
    
        if isData and Offline and rec.triggerStream() == "express":
    
            #=================================================================================
            #============== PPrStabilityMon without individual channel plots =================
            #=================================================================================
            from TrigT1CaloMonitoring.TrigT1CaloMonitoringConf import PPrStabilityMon
            L1PPrStabilityMonTool = PPrStabilityMon(
                name = "L1PPrStabilityMonTool",
                doFineTimeMonitoring = doFineTime,
                doEtCorrelationMonitoring = isCalo,
                BS_TriggerTowerContainer = "TriggerTowers",
                ppmADCMinValue = 60,
                lumiMax = 2000,
                fineTimeCut = 20,
                PathInRootFile = "L1Calo/PPM/Stability",
                #OutputLevel = DEBUG
                )
            ToolSvc += L1PPrStabilityMonTool
            L1CaloMan.AthenaMonTools += [ L1PPrStabilityMonTool ]
    
        #=================================================================================
        #================================= PPr ===========================================
        #=================================================================================
        from TrigT1CaloMonitoring.TrigT1CaloMonitoringConf import PPrMon
        L1PPrMonTool = PPrMon(
            name = "L1PPrMonTool",
            BS_TriggerTowerContainer = "TriggerTowers",
            LUTHitMap_ThreshVec = [0,1,2,3,4,5,6,7,10,15,20,33,45,50],
            LUTHitMap_LumiBlocks = 10,
            ADCHitMap_Thresh = 50,
            MaxEnergyRange = 256,
            EMFADCCut = 40,
            HADFADCCut = 40,
            ADCPedestal = 32,
            PathInRootFile = "L1Calo/PPM",
            ErrorPathInRootFile = "L1Calo/PPM/Errors",
            #OnlineTest = True,
            #OutputLevel = DEBUG
            )
        ToolSvc += L1PPrMonTool
        L1CaloMan.AthenaMonTools += [ L1PPrMonTool ]

        if isData:
                
            from TrigT1CaloMonitoring.TrigT1CaloMonitoringConf import PPMSimBSMon
            PPMSimBSMonTool = PPMSimBSMon("PPMSimBSMonTool")
            ToolSvc += PPMSimBSMonTool
            L1CaloMan.AthenaMonTools += [ PPMSimBSMonTool ]
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
            ToolSvc += L1PPrSpareMonTool
            L1CaloMan.AthenaMonTools += [ L1PPrSpareMonTool ]
    
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
        ToolSvc += L1JEMMonTool
        L1CaloMan.AthenaMonTools += [ L1JEMMonTool ]
    
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
        ToolSvc += L1CMMMonTool
        L1CaloMan.AthenaMonTools += [ L1CMMMonTool ]
    
        if isData:
    
            #--------------------- Transmission and Performance ------------------------------
            from TrigT1CaloMonitoring.TrigT1CaloMonitoringConf import JEPSimBSMon
            JEPSimBSMonTool = JEPSimBSMon("JEPSimBSMonTool",
                JEPHitsTool = "LVL1::L1JEPHitsTools/L1JEPHitsTools_Mon",
                JetTool = "LVL1::L1JetTools/L1JetTools_Mon",
                JEPEtSumsTool = "LVL1::L1JEPEtSumsTools/L1JEPEtSumsTools_Mon",
                )
            ToolSvc += JEPSimBSMonTool
            L1CaloMan.AthenaMonTools += [ JEPSimBSMonTool ]
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
        ToolSvc += L1BSCPMMonTool
        L1CaloMan.AthenaMonTools += [ L1BSCPMMonTool ]
    
        if isData:
    
            from TrigT1CaloMonitoring.TrigT1CaloMonitoringConf import CPMSimBSMon
            CPMSimBSMonTool = CPMSimBSMon("CPMSimBSMonTool",
                                      EmTauTool = "LVL1::L1EmTauTools/L1EmTauTools_Mon",
                                      )
            ToolSvc += CPMSimBSMonTool
            L1CaloMan.AthenaMonTools += [ CPMSimBSMonTool ]
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
            ToolSvc += L1BSRODMonTool
            L1CaloMan.AthenaMonTools += [ L1BSRODMonTool ]
    
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
            ToolSvc += L1GlobalMonTool
            L1CaloMan.AthenaMonTools += [ L1GlobalMonTool ]
    
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
            ToolSvc += L1EmEfficienciesMonTool
            L1CaloMan.AthenaMonTools += [ L1EmEfficienciesMonTool ]
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
            L1CaloMan.AthenaMonTools += [ L1JetEfficienciesMonTool ]
            if not hasattr( ToolSvc, "TrigDecisionTool" ):
                from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
                tdt = Trig__TrigDecisionTool('TrigDecisionTool')
                ToolSvc += tdt
    
    #=================================================================================
    # FileKey must match that given to THistSvc
    L1CaloMan.FileKey             = DQMonFlags.monManFileKey()
    L1CaloMan.Environment         = DQMonFlags.monManEnvironment()
    L1CaloMan.ManualDataTypeSetup = DQMonFlags.monManManualDataTypeSetup()
    L1CaloMan.DataType            = DQMonFlags.monManDataType()
    topSequence += L1CaloMan
