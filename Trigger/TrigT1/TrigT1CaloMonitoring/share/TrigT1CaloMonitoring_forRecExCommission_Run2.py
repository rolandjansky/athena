# @file TrigT1CaloMonitoring_forRecExCommission.py
#
# Standard monitoring jobOptions - runs on Tier0 (Reco_tf.py) or online.
#
# @authors Johanna Fleckner, Andrea Neusiedl, Peter Faulkner

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
    isData = (globalflags.DataSource() == "data")
    isCalo = (rec.doCalo() and rec.doLArg() and rec.doTile())
    triggerConfigService = "TrigConf::TrigConfigSvc/TrigConfigSvc"

    #================================= Monitoring configuration ==============
    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()
    L1CaloMan = AthenaMonManager("L1CaloMonManager")

    # get a handle on the ToolSvc
    from AthenaCommon.AppMgr import ToolSvc

    if globalflags.InputFormat() == "bytestream":
        include("TrigT1CaloByteStream/ReadLVL1CaloBS_jobOptions.py")

    include("RecExCond/AllDet_detDescr.py")

    if isData:
        include(
            'TrigT1CaloCalibConditions/L1CaloCalibConditionsTier0_jobOptions.py'
        )

    # setup l1calo database
    if athenaCommonFlags.isOnline:
        include(
            'TrigT1CaloCalibConditions/L1CaloCalibConditions_jobOptions.py')

    include("CaloConditions/CaloConditions_jobOptions.py")
    if Offline:
        include("LArDetDescr/LArDetDescr_joboptions.py")
    else:
        include("LArConditionsCommon/LArIdMap_comm_jobOptions.py")

    from TrigT1CaloMonitoringTools.LVL1CaloMonFlags import LVL1CaloMonFlags

    doFineTime = False
    if isData and isCalo and (
            (LVL1CaloMonFlags.doPPrStabilityMon()
             and LVL1CaloMonFlags.doFineTimeMonitoring())
            or (Offline
                and (rec.triggerStream() == "express"
                     or rec.triggerStream() == 'CosmicCalo'))):
        # load the sqlite file for the fine time monitoring
        #dbpath = "/afs/cern.ch/user/l/l1ccalib/w0/DaemonData/reference/calibJuly.sqlite"
        #import os.path
        #if os.path.isfile(dbpath):
        #from AthenaServices.AthenaServicesConf import MetaDataSvc
        #svcMgr += MetaDataSvc("MetaDataSvc")
        #svcMgr.IOVDbSvc.Folders += ["<dbConnection>sqlite://;schema=;dbname=L1CALO</dbConnection>/TRIGGER/L1Calo/V1/References/FineTimeReferences"]

        from IOVDbSvc.CondDB import conddb
        conddb.addFolder("TRIGGER","/TRIGGER/L1Calo/V1/References/FineTimeReferences")
        doFineTime = True

    if LVL1CaloMonFlags.doPPrStabilityMon():

        #======================================================================
        #  Want Full PPrStabilityMon to run alone
        #======================================================================

        from TrigT1CaloMonitoring.TrigT1CaloMonitoringConf import LVL1__PPrStabilityMon
        L1PPrStabilityMonTool = LVL1__PPrStabilityMon(
            name="L1PPrStabilityMonTool",
            doFineTimeMonitoring=doFineTime,
            doPedestalMonitoring=LVL1CaloMonFlags.doPedestalMonitoring(),
            doPedestalCorrectionMonitoring=LVL1CaloMonFlags.doPedestalCorrectionMonitoring(
            ),
            doEtCorrelationMonitoring=(
                LVL1CaloMonFlags.doEtCorrelationMonitoring() and isCalo),
            ppmADCMinValue=60,
            lumiMax=2000,
            fineTimeCut=20,
            PathInRootFile="L1Calo/PPrStabilityMon",
            # OutputLevel = DEBUG
        )
        #ToolSvc += L1PPrStabilityMonTool
        L1CaloMan.AthenaMonTools += [L1PPrStabilityMonTool]

        # --- BunchCrossing Tool configuration ---
        if athenaCommonFlags.isOnline:
            from TrigBunchCrossingTool.BunchCrossingTool import BunchCrossingTool
            theBCTool = BunchCrossingTool()
            ToolSvc += theBCTool
            L1PPrStabilityMonTool.BunchCrossingTool = theBCTool

    else:

        if isData and Offline and rec.triggerStream() == "express":

            #==================================================================
            #============== PPrStabilityMon without individual channel plots ==
            #==================================================================
            from TrigT1CaloMonitoring.TrigT1CaloMonitoringConf import LVL1__PPrStabilityMon
            L1PPrStabilityMonTool = LVL1__PPrStabilityMon(
                name="L1PPrStabilityMonTool",
                doFineTimeMonitoring=doFineTime,
                doEtCorrelationMonitoring=isCalo,
                ppmADCMinValue=60,
                lumiMax=2000,
                fineTimeCut=20,
                PathInRootFile="L1Calo/PPM/Stability",
                #OutputLevel = DEBUG
            )
            #ToolSvc += L1PPrStabilityMonTool
            L1CaloMan.AthenaMonTools += [L1PPrStabilityMonTool]

        #======================================================================
        #================================= PPr ================================
        #======================================================================
        from TrigT1CaloMonitoring.TrigT1CaloMonitoringConf import LVL1__PPrMon
        from AthenaCommon.JobProperties import jobproperties
        L1PPrMonTool = LVL1__PPrMon(
            name="L1PPrMonTool",
            LUTHitMap_ThreshVec=[
                0, 1, 3, 5, 10, 20, 35, 50],
            LUTHitMap_LumiBlocks=10,
            ADCHitMap_Thresh=50,
            MaxEnergyRange=256,
            EMFADCCut=40,
            HADFADCCut=40,
            ADCPedestal=32,
            PathInRootFile="L1Calo/PPM",
            ErrorPathInRootFile="L1Calo/PPM/Errors",
            #OnlineTest = True,
            #OutputLevel = DEBUG
        )
        #ToolSvc += L1PPrMonTool
        L1CaloMan.AthenaMonTools += [L1PPrMonTool]


        if isData and rec.triggerStream() == "Mistimed":

            #======================================================================
            #=================== Monitoring of the Mistimed Stream ================
            #======================================================================
            from TrigT1CaloMonitoring.TrigT1CaloMonitoringConf import LVL1__MistimedStreamMon

            from AthenaCommon.JobProperties import jobproperties
            L1MistimedStreamTool = LVL1__MistimedStreamMon(
                name="L1MistimedStreamTool",
                #OnlineTest = True,
                #OutputLevel = DEBUG
            )
            # ToolSvc += L1MistimedStreamTool
            L1CaloMan.AthenaMonTools += [L1MistimedStreamTool]

            from TrigT1CaloCondSvc.TrigT1CaloCondSvcConf import L1CaloCondSvc
            ServiceMgr += L1CaloCondSvc()
            from IOVDbSvc.CondDB import conddb
            conddb.addFolderWithTag("TRIGGER","/TRIGGER/L1Calo/V1/Conditions/RunParameters","HEAD")
            conddb.addFolderWithTag("TRIGGER","/TRIGGER/L1Calo/V2/Configuration/ReadoutConfig","HEAD")

        if isData:

            from TrigT1CaloMonitoring.TrigT1CaloMonitoringConf import LVL1__PPMSimBSMon
            PPMSimBSMonTool = LVL1__PPMSimBSMon("PPMSimBSMonTool")
            #ToolSvc += PPMSimBSMonTool
            L1CaloMan.AthenaMonTools += [PPMSimBSMonTool]
            from TrigT1CaloTools.TrigT1CaloToolsConf import LVL1__L1TriggerTowerTool
            L1TriggerTowerTool = LVL1__L1TriggerTowerTool("L1TriggerTowerTool")
            ToolSvc += L1TriggerTowerTool
            from TrigT1CaloCondSvc.TrigT1CaloCondSvcConf import L1CaloCondSvc
            ServiceMgr += L1CaloCondSvc()
            from IOVDbSvc.CondDB import conddb
            conddb.addFolderWithTag("TRIGGER","/TRIGGER/L1Calo/V1/Conditions/RunParameters","HEAD")

            #--------------------------------- PPM Spare Channels--------------
            from TrigT1CaloMonitoring.TrigT1CaloMonitoringConf import LVL1__PPrSpareMon
            L1PPrSpareMonTool = LVL1__PPrSpareMon(
                name="L1PPrSpareMonTool",
                ADCHitMap_Thresh=40,
                PathInRootFile="L1Calo/PPM/SpareChannels",
                ErrorPathInRootFile="L1Calo/PPM/SpareChannels/Errors",
                #OutputLevel = DEBUG
            )
            #ToolSvc += L1PPrSpareMonTool
            L1CaloMan.AthenaMonTools += [L1PPrSpareMonTool]

        #======================================================================
        #=================================== JEP ==============================
        #======================================================================

        #------------------------------------ JEM -----------------------------
        from TrigT1CaloMonitoring.TrigT1CaloMonitoringConf import LVL1__JEPJEMMon
        L1JEPJEMMonTool = LVL1__JEPJEMMon("L1JEPJEMMonTool")
        #ToolSvc += L1JEPJEMMonTool
        L1CaloMan.AthenaMonTools += [L1JEPJEMMonTool]

        #----------------------------------- CMX ------------------------------
        from TrigT1CaloMonitoring.TrigT1CaloMonitoringConf import LVL1__JEPCMXMon
        L1JEPCMXMonTool = LVL1__JEPCMXMon("L1JEPCMXMonTool")
        #ToolSvc += L1JEPCMXMonTool
        L1CaloMan.AthenaMonTools += [L1JEPCMXMonTool]

        if isData:
            #--------------------- Transmission and Performance ---------------
            from TrigT1CaloMonitoring.TrigT1CaloMonitoringConf import LVL1__JEPSimMon
            JEPSimMonTool = LVL1__JEPSimMon("JEPSimMonTool")
            #ToolSvc += JEPSimMonTool
            L1CaloMan.AthenaMonTools += [JEPSimMonTool]
            # ToolSvc.JEPSimBSMonTool.OutputLevel = DEBUG

        # from TrigT1CaloTools.TrigT1CaloToolsConf import LVL1__L1JEPHitsTools
        # L1JEPHitsTools = LVL1__L1JEPHitsTools("L1JEPHitsTools_Mon")
        # L1JEPHitsTools.LVL1ConfigSvc = triggerConfigService
        # ToolSvc += L1JEPHitsTools
        # from TrigT1CaloTools.TrigT1CaloToolsConf import LVL1__L1JetTools
        # L1JetTools = LVL1__L1JetTools("L1JetTools_Mon")
        # L1JetTools.LVL1ConfigSvc = triggerConfigService
        # ToolSvc += L1JetTools
        # from TrigT1CaloTools.TrigT1CaloToolsConf import LVL1__L1EtTools
        # L1EtTools = LVL1__L1EtTools("L1EtTools_Mon")
        # L1EtTools.LVL1ConfigSvc = triggerConfigService
        # ToolSvc += L1EtTools
        # from TrigT1CaloTools.TrigT1CaloToolsConf import LVL1__L1JEPEtSumsTools
        # L1JEPEtSumsTools = LVL1__L1JEPEtSumsTools("L1JEPEtSumsTools_Mon",
        #                                     EtTool = "LVL1::L1EtTools/L1EtTools_Mon")
        # L1JEPEtSumsTools.LVL1ConfigSvc = triggerConfigService
        # ToolSvc += L1JEPEtSumsTools

        #======================================================================
        #===================================== CP =============================
        #=================================== CP-CMX ===========================
        #======================================================================
        if isData:
            # Sasha: Currently pool files does not contain xAOD objects for
            # CPMTowers, CMXCPTobs and CMXCPHits
            from TrigT1CaloMonitoring.TrigT1CaloMonitoringConf import LVL1__CPMon
            L1CPMMonTool = LVL1__CPMon("L1CPMMonTool")
            #ToolSvc += L1CPMMonTool
            L1CaloMan.AthenaMonTools += [L1CPMMonTool]

        if isData:

            from TrigT1CaloMonitoring.TrigT1CaloMonitoringConf import LVL1__CPSimMon
            CPSimMonTool = LVL1__CPSimMon("CPSimMonTool")
            #ToolSvc += CPSimMonTool
            L1CaloMan.AthenaMonTools += [CPSimMonTool]
            # ToolSvc.CPMSimBSMonTool.OutputLevel = DEBUG

            # from TrigT1CaloTools.TrigT1CaloToolsConf import LVL1__L1EmTauTools
            # L1EmTauTools = LVL1__L1EmTauTools("L1EmTauTools_Mon")
            # L1EmTauTools.LVL1ConfigSvc = triggerConfigService
            # ToolSvc += L1EmTauTools

            #==================================================================
            #===================================== ROD ========================
            #==================================================================
            from TrigT1CaloMonitoring.TrigT1CaloMonitoringConf import LVL1__RODMon
            L1BSRODMonTool = LVL1__RODMon(
                name="L1BSRODMonTool",
                #OnlineTest = True,
                #OutputLevel = DEBUG,
            )
            #ToolSvc += L1BSRODMonTool
            L1CaloMan.AthenaMonTools += [L1BSRODMonTool]

            #==================================================================
            #=============================== Global Overview ==================
            #==================================================================
            from TrigT1CaloMonitoring.TrigT1CaloMonitoringConf import LVL1__OverviewMon
            L1OverviewMonTool = LVL1__OverviewMon("L1OverviewMonTool")
            #ToolSvc += L1OverviewMonTool
            L1CaloMan.AthenaMonTools += [L1OverviewMonTool]

            from TrigT1CaloMonitoringTools.TrigT1CaloMonitoringToolsConf import LVL1__TrigT1CaloMonErrorTool
            L1MonErrorTool = LVL1__TrigT1CaloMonErrorTool(
                name="TrigT1CaloMonErrorTool",
                FlagCorruptEvents="None"
            )
            #ToolSvc += L1MonErrorTool
            # pass

        # if isData and isCalo and Offline and (rec.triggerStream() == "JetTauEtmiss"
        #                                    or rec.triggerStream() == "Muons"
        # or rec.triggerStream() == "express"):

            #==================================================================
            #===================== Tag & Probe Efficiencies  ==================
            #==================================================================

            from TrigT1CaloMonitoring.TrigT1CaloMonitoringConf import LVL1__TagProbeEfficiencyMon
            TagProbeEfficiencyMonTool = LVL1__TagProbeEfficiencyMon("TagProbeEfficiencyMonTool")
            #ToolSvc += TagProbeEfficiencyMonTool
            L1CaloMan.AthenaMonTools += [TagProbeEfficiencyMonTool]


            #==================================================================
            #=============================== EM Efficiencies ==================
            #==================================================================
            # trigstring = ['EF_.*']
        #     from TrigT1CaloMonitoring.TrigT1CaloMonitoringConf import EmEfficienciesMonTool
        #     L1EmEfficienciesMonTool = EmEfficienciesMonTool ( name = "EmEfficienciesMonTool",
        #                                                           TriggerStrings = trigstring
        #                                                     )
        #     ToolSvc += L1EmEfficienciesMonTool
        #     L1CaloMan.AthenaMonTools += [ L1EmEfficienciesMonTool ]
        #     if not hasattr( ToolSvc, "TrigDecisionTool" ):
        #         from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
        #         tdt = Trig__TrigDecisionTool('TrigDecisionTool')
        #         ToolSvc += tdt

        # if isData and isCalo and Offline and (rec.triggerStream() == "Egamma"
        #                                    or rec.triggerStream() == "Muons"
        # or rec.triggerStream() == "express"):

            #==================================================================
            #=============================== Jet Efficiencies =================
            #==================================================================
            # trigstring = ['EF_.*']
            # from TrigT1CaloMonitoring.TrigT1CaloMonitoringConf import JetEfficienciesMonTool
            # from JetSelectorTools.ConfiguredJetCleaningTools import *
            # L1JetEfficienciesMonTool = JetEfficienciesMonTool ( name = "JetEfficienciesMonTool",
            #                                                       TriggerStrings = trigstring
            #                                                   )
            # from AthenaCommon.AppMgr import ToolSvc
            # ToolSvc += JetCleaningTool('JetCleaningLooseTool')
            # ToolSvc += JetCleaningTool('JetCleaningMediumTool')
            # ToolSvc += JetCleaningTool('JetCleaningTightTool')
            # L1JetEfficienciesMonTool.JetCleaningLooseTool = ConfiguredJetCleaningTool_Loose("JetCleaningLooseTool")
            # L1JetEfficienciesMonTool.JetCleaningMediumTool = ConfiguredJetCleaningTool_Medium("JetCleaningMediumTool")
            # L1JetEfficienciesMonTool.JetCleaningTightTool = ConfiguredJetCleaningTool_Tight("JetCleaningTightTool")

            # ToolSvc += L1JetEfficienciesMonTool
            # L1CaloMan.AthenaMonTools += [ L1JetEfficienciesMonTool ]
            # if not hasattr( ToolSvc, "TrigDecisionTool" ):
            #     from TrigDecisionTool.TrigDecisionToolConf import Trig__TrigDecisionTool
            #     tdt = Trig__TrigDecisionTool('TrigDecisionTool')
            #     ToolSvc += tdt

    #=========================================================================
    # FileKey must match that given to THistSvc
    L1CaloMan.FileKey = DQMonFlags.monManFileKey()
    L1CaloMan.Environment = DQMonFlags.monManEnvironment()
    L1CaloMan.ManualDataTypeSetup = DQMonFlags.monManManualDataTypeSetup()
    L1CaloMan.DataType = DQMonFlags.monManDataType()
    topSequence += L1CaloMan

    #====
