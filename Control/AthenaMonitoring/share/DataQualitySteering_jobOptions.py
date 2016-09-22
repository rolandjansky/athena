# ********************************************************************************
# Offline AthenaMonitoring configuration
#
# $Id: DataQualitySteering_jobOptions.py,v 1.15 2009-05-05 08:20:08 sschaetz Exp $
# ********************************************************************************

# disable TRTEleMon until further notice
# - PUEO 20140401
TRTELEMON=False

local_logger = logging.getLogger('DataQualitySteering_jobOptions')

if DQMonFlags.doMonitoring():
   if DQMonFlags.useTrigger():
      if not hasattr(ToolSvc, DQMonFlags.nameTrigDecTool()):
         local_logger.debug("trigger decision tool not found, including it now")
         include("AthenaMonitoring/TrigDecTool_jobOptions.py")


   # don't set up lumi access if in MC or enableLumiAccess == False
   if globalflags.DataSource.get_Value() != 'geant4' and DQMonFlags.enableLumiAccess():
      if not hasattr(ToolSvc,"LuminosityTool"):
         if athenaCommonFlags.isOnline:
            local_logger.debug("luminosity tool not found, importing online version")
            from LumiBlockComps.LuminosityToolDefault import LuminosityToolOnline
            ToolSvc+=LuminosityToolOnline()
         else:
            local_logger.debug("luminosity tool not found, importing offline version")
            from LumiBlockComps.LuminosityToolDefault import LuminosityToolDefault 
            ToolSvc+=LuminosityToolDefault()

      if not hasattr(ToolSvc,"TrigLivefractionTool"):
         local_logger.debug("live fraction tool not found, importing")
         from LumiBlockComps.TrigLivefractionToolDefault import TrigLivefractionToolDefault 
         ToolSvc+=TrigLivefractionToolDefault()

   include("AthenaMonitoring/AtlasReadyFilterTool_jobOptions.py")
   include("AthenaMonitoring/FilledBunchFilterTool_jobOptions.py")

   # Ugly hack to get list of tools in job at this point
   # We will compare the list at the end of the script to get the monitoring
   # tools
   monToolSet_before = set(ToolSvc.getChildren())

   #---------------------------#
   # Inner detector monitoring #
   #---------------------------#
   if DQMonFlags.doPixelMon() or DQMonFlags.doSCTMon() or DQMonFlags.doTRTMon() or \
         DQMonFlags.doInDetGlobalMon() or DQMonFlags.doInDetAlignMon():
      try:
         include("InDetRecExample/InDetMonitoring.py")
      except Exception:
         treatException("DataQualitySteering_jobOptions.py: exception when setting up inner detector monitoring")

   #----------------------------#
   # Global combined monitoring #
   #----------------------------#
   if DQMonFlags.doGlobalMon():
      try:
         include("DataQualityTools/DataQualityMon_jobOptions.py")
      except Exception:
         treatException("DataQualitySteering_jobOptions.py: exception when setting up global monitoring")
   
   #--------------------#
   # Trigger monitoring #
   #--------------------#
   if DQMonFlags.doLVL1CaloMon():
      try:
         include("TrigT1CaloMonitoring/TrigT1CaloMonitoring_forRecExCommission.py")
         include("TrigT1Monitoring/TrigT1Monitoring_forRecExCommission.py")
      except Exception:
         treatException("DataQualitySteering_jobOptions.py: exception when setting up L1 Calo monitoring")

   if DQMonFlags.doCTPMon():
      try:
         include("TrigT1CTMonitoring/TrigT1CTMonitoringJobOptions_forRecExCommission.py")
      except Exception:
         treatException("DataQualitySteering_jobOptions.py: exception when setting up central trigger monitoring")

   if DQMonFlags.doHLTMon():
      try:
         include("TrigHLTMonitoring/HLTMonitoring_topOptions.py")
         HLTMonMan = topSequence.HLTMonManager
         HLTMonMan.FileKey = DQMonFlags.monManFileKey()  
      except Exception:
         treatException("DataQualitySteering_jobOptions.py: exception when setting up HLT monitoring")

   #--------------#
   # TRT electron #
   #--------------#
   if DQMonFlags.doTRTElectronMon() and TRTELEMON:
      try:
         include("InDetPerformanceMonitoring/TRT_Monitoring_RecExCommonAddOn_jobOptions.py") 
         TRTElMonMan = topSequence.TRTElectronMonManager
         TRTElMonMan.FileKey     = DQMonFlags.monManFileKey()
         TRTElMonMan.Environment = DQMonFlags.monManEnvironment()
      except Exception:
         treatException("DataQualitySteering_jobOptions.py: exception when setting up TRT electron monitoring")

   #----------------#
   # ID performance #
   #----------------#
   if DQMonFlags.doInDetPerfMon():
      try:
         include("InDetPerformanceMonitoring/IDPerfMon_jobOptions.py")
         IDPerfMonManager = topSequence.IDPerfMonManager
         IDPerfMonManager.FileKey          = DQMonFlags.monManFileKey()
         IDPerfMonManager.Environment      = DQMonFlags.monManEnvironment()
         include("InDetDiMuonMonitoring/InDetDiMuMon_jobOptions.py")
         InDetDiMuMonManager = topSequence.InDetDiMuMonManager
         InDetDiMuMonManager.FileKey          = DQMonFlags.monManFileKey()
         InDetDiMuMonManager.Environment      = DQMonFlags.monManEnvironment()
      except Exception:
         treatException("DataQualitySteering_jobOptions.py: exception when setting up InDetPerformance/InDetDiMuon monitoring")

   #--------------------#
   # TileCal monitoring #
   #--------------------#
   if DQMonFlags.doTileMon():
      try:
      	 include("TileMonitoring/TileMon_jobOptions.py")
   
      except Exception:
         treatException("DataQualitySteering_jobOptions.py: exception when setting up Tile monitoring")

   #------------------#
   # LAr monitoring   #
   #------------------#
   if DQMonFlags.doLArMon():
      # FIXME: don't use global flags here
      if 'doLArCollisionTimeMon' not in dir():
         doLArCollisionTimeMon=True
      if doLArCollisionTimeMon:
         include ("LArCellRec/LArCollisionTime_jobOptions.py")
         include("LArClusterRec/LArClusterCollisionTime_jobOptions.py")
      try:
         LArMon = AthenaMonManager(name="LArMonManager",
                          FileKey             = DQMonFlags.monManFileKey(),
                          Environment         = DQMonFlags.monManEnvironment(),
                          ManualDataTypeSetup = DQMonFlags.monManManualDataTypeSetup(),
                          DataType            = DQMonFlags.monManDataType())
         topSequence += LArMon
         include("LArMonTools/LArAllMonitoring_jobOptions.py")
      except Exception:
         treatException("DataQualitySteering_jobOptions.py: exception when setting up LAr monitoring")

   #-------------------------------------------------------------------------#
   # Calo monitoring - cells and clusters independent of LAr or Tile origin  #
   #-------------------------------------------------------------------------#
   if DQMonFlags.doCaloMon():
      try:
         include("CaloMonitoring/CaloAllMonitoring_jobOptions.py")
      except Exception:
         treatException("DataQualitySteering_jobOptions.py: exception when setting up Calo monitoring")

   #-------------------#
   # Egamma monitoring #
   #-------------------#
   if DQMonFlags.doEgammaMon():
      try:
         include("egammaPerformance/egammaMonitoring_jobOptions.py")
      except Exception:
         treatException("DataQualitySteering_jobOptions.py: exception when setting up Egamma monitoring")

   #----------------#
   # MET monitoring #
   #----------------#
   if DQMonFlags.doMissingEtMon():
      try:
         include("MissingETMonitoring/MissingET_Monitoring.py")
      except Exception:
         treatException("DataQualitySteering_jobOptions.py: exception when setting up ETmiss monitoring")

   #----------------#
   # Jet monitoring #
   #----------------#
   if DQMonFlags.doJetMon():
      try:
         include("JetMonitoring/JetMonitoring_jobOptions.py")
      except Exception:
         treatException("DataQualitySteering_jobOptions.py: exception when setting up Jet monitoring")

   #----------------#
   # Tau monitoring #
   #----------------#
   if DQMonFlags.doTauMon():
      try:
         include("tauMonitoring/tauMonitoring_jobOptions.py")
      except Exception:
         treatException("DataQualitySteering_jobOptions.py: exception when setting up Tau monitoring")

   #--------------------#
   # Jet tag monitoring #
   #--------------------#
   if DQMonFlags.doJetTagMon():
      try:
         include("JetTagMonitoring/JetTagMonitoring_jobOptions.py")
      except Exception:
         treatException("DataQualitySteering_jobOptions.py: exception when setting up jet-tag monitoring")

   #-----------------------------#
   # MuonSpectrometer monitoring #
   #-----------------------------#
   if (DQMonFlags.doMuonRawMon() or DQMonFlags.doMuonSegmentMon()
       or DQMonFlags.doMuonTrackMon() or DQMonFlags.doMuonAlignMon()
       or DQMonFlags.doMuonTrkPhysMon() or DQMonFlags.doMuonPhysicsMon()):
     try:
        include("MuonDQAMonitoring/MuonDetMonitoring.py")
     except Exception:
        treatException("DataQualitySteering_jobOptions.py: exception when setting up Muon detector monitoring")
  
   #------------------#
   # LUCID monitoring #
   #------------------#
   if DQMonFlags.doLucidMon():
      try:
         include("LUCID_Monitoring/LUCIDMon_jobOptions.py")
      except Exception:
         treatException("DataQualitySteering_jobOptions.py: exception when setting up LUCID monitoring")

   #------------------#                                                                                                                       
   # AFP monitoring   #                                                                                                                       
   #------------------#                                                                                                                       
   if DQMonFlags.doAFPMon():
      try:
         include("AFP_Monitoring/AFPMonitoring_jobOptions.py")
      except Exception:
         treatException("DataQualitySteering_jobOptions.py: exception when setting up AFP monitoring")


   #---------------------#
   # HeavyIon monitoring #
   #---------------------#
   if DQMonFlags.doHIMon():
      try:
         include("HIMonitoring/HIMonitoringSteering_jo.py")
      except Exception:
         treatException("DataQualitySteering_jobOptions.py: exception when setting up HI monitoring")

   #------------------------#
   # Trigger chain steering #
   #------------------------#
   trigMap = DQMonFlags.triggerChainMap.get_Value()
   for toolName, trigChain in trigMap.iteritems():
      local_logger.debug("Applying trigger %s to %s", trigChain, toolName)
      if hasattr(ToolSvc,toolName):
         tool = getattr(ToolSvc,toolName)
         tool.TriggerChain = trigChain
      else:
         local_logger.debug("%s is not found in ToolSvc. Cannot set trigger chain!", toolName)

   #--------------------------#
   # Post-setup configuration #
   #--------------------------#
   if rec.triggerStream()=='express':
      include("AthenaMonitoring/AtlasReadyFilterTool_jobOptions.py")
   monToolSet_after = set(ToolSvc.getChildren())
   local_logger.debug('DQ Post-Setup Configuration')
   for tool in monToolSet_after-monToolSet_before:
      # stop lumi access if we're in MC or enableLumiAccess == False
      if 'EnableLumi' in dir(tool):
         if globalflags.DataSource.get_Value() == 'geant4' or not DQMonFlags.enableLumiAccess():
            tool.EnableLumi = False
         else:
            tool.EnableLumi = True
      # if we have the FilterTools attribute, assume this is in fact a
      # monitoring tool
      if hasattr(tool, 'FilterTools'):
         # if express: use ATLAS Ready filter
         local_logger.debug('Setting up filters for tool %s', tool)
         if rec.triggerStream()=='express':
            local_logger.info('Stream is express and we will add ready tool for %s', tool)
            tool.FilterTools += [monAtlasReadyFilterTool]
         # give all the tools the trigger translator
         if DQMonFlags.useTrigger():
            tool.TrigDecisionTool = monTrigDecTool
            tool.TriggerTranslatorTool = monTrigTransTool

         if DQMonFlags.monToolPostExec():
            postprocfunc = eval(DQMonFlags.monToolPostExec())
            local_logger.debug('Applying postexec transform to  ===> %s', tool)
            postprocfunc(tool)
            del postprocfunc

   del monToolSet_before, monToolSet_after

del local_logger
