# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function

class PixelConditionsServicesSetup:
  """
  instantiates Pixel conditions services 
  allows multiple copies with a different prefix
  triggerMode for services available and used online
  useDCS is a special switch to be able to investigate 
  effects of data available in the DCS (and not online)
  The class can be queried for the instance name to ease 
  configuration with non-default service names
  """
  def __init__(self):

    self._print = False
    self._lock = False
    self.config(useDCS=True, onlineMode=False, prefix='')    #default offline settings

    from AthenaCommon.AppMgr import ServiceMgr
    self.svcMgr = ServiceMgr
    from AthenaCommon.AppMgr import ToolSvc
    self.toolSvc = ToolSvc
    self.summaryTool = None
    
    self.isData = False
    from AthenaCommon.GlobalFlags import globalflags
    if globalflags.DataSource() == 'data':
      self.isData = True

  def config(self, useDCS=True, onlineMode=False, prefix=''):
    if not self._lock:
      self.useDCS = useDCS
      self.onlineMode = onlineMode
      self.prefix = prefix
      self.useBS = True
      self.useTDAQ = False
      self.usePixMap = True
      self.eventInfoKey = "ByteStreamEventInfo"
      from AthenaCommon.GlobalFlags import globalflags
      if globalflags.DataSource() == 'geant4':      #does not work for transbs
        self.useBS = False
        self.eventInfoKey = "McEventInfo" 
        #self.useTDAQ=True
    else:
      print ('Not modifying an instance of PixelConditionsSetup as it is locked')
     

  def lock(self):
    " prevent modifications to this instance "
    self._lock = True


  def createTool(self):
    from AthenaCommon.AppMgr import ToolSvc

    #############################
    # Setup Pixel Configuration #
    #############################
    from AthenaCommon.AlgSequence import AthSequencer
    condSeq = AthSequencer("AthCondSeq")

    from IOVDbSvc.CondDB import conddb
    from AthenaCommon.GlobalFlags import globalflags
    from AtlasGeoModel.CommonGMJobProperties import CommonGeometryFlags as commonGeoFlags
    from AtlasGeoModel.InDetGMJobProperties import InDetGeometryFlags as geoFlags

    useNewDeadmapFormat = False
    useNewChargeFormat  = False

    if not useNewDeadmapFormat:
      if not (conddb.folderRequested("/PIXEL/PixMapOverlay") or conddb.folderRequested("/PIXEL/Onl/PixMapOverlay")):
        conddb.addFolderSplitOnline("PIXEL","/PIXEL/Onl/PixMapOverlay","/PIXEL/PixMapOverlay", className='CondAttrListCollection')

    if not hasattr(condSeq, 'PixelConfigCondAlg'):
      from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelConfigCondAlg

      useCablingConditions = False
      IdMappingDat="PixelCabling/Pixels_Atlas_IdMapping_2016.dat"
      rodIDForSingleLink40=0
      if (globalflags.DataSource()=='geant4'):
        # ITk:
        if geoFlags.isSLHC():
          IdMappingDat = "ITk_Atlas_IdMapping.dat"
          if "BrlIncl4.0_ref" == commonGeoFlags.GeoType():
            IdMappingDat = "ITk_Atlas_IdMapping_InclBrl4.dat"
          elif "IBrlExt4.0ref" == commonGeoFlags.GeoType():
            IdMappingDat = "ITk_Atlas_IdMapping_IExtBrl4.dat"
          elif "BrlExt4.0_ref" == commonGeoFlags.GeoType():
            IdMappingDat = "ITk_Atlas_IdMapping_ExtBrl4.dat"
          elif "BrlExt3.2_ref" == commonGeoFlags.GeoType():
            IdMappingDat = "ITk_Atlas_IdMapping_ExtBrl32.dat"
        elif (geoFlags.isIBL() == False):
          IdMappingDat="PixelCabling/Pixels_Atlas_IdMapping.dat"
        else:
          # Planar IBL
          if (geoFlags.IBLLayout() == "planar"):
            if (geoFlags.isDBM() == True):
              IdMappingDat="PixelCabling/Pixels_Atlas_IdMapping_inclIBL_DBM.dat"
            else:
              IdMappingDat="PixelCabling/Pixels_Atlas_IdMapping_inclIBL.dat"
          # Hybrid IBL plus DBM
          elif (geoFlags.IBLLayout() == "3D"):
            IdMappingDat="PixelCabling/Pixels_Atlas_IdMapping_Run2.dat"
        
      elif (globalflags.DataSource=='data'):
        from RecExConfig.AutoConfiguration import GetRunNumber
        runNum = GetRunNumber()
        if (runNum<222222):
          useCablingConditions = False
          IdMappingDat="PixelCabling/Pixels_Atlas_IdMapping_May08.dat"
          rodIDForSingleLink40=1300000
        else:
          useCablingConditions = True
          rodIDForSingleLink40=1300000
          # Even though we are reading from COOL, set the correct fallback map.
          if (runNum >= 344494):
            IdMappingDat="PixelCabling/Pixels_Atlas_IdMapping_344494.dat"
          elif (runNum >= 314940 and runNum < 344494):
            IdMappingDat="PixelCabling/Pixels_Atlas_IdMapping_314940.dat"
          elif (runNum >= 289350 and runNum < 314940): # 2016
            IdMappingDat="PixelCabling/Pixels_Atlas_IdMapping_2016.dat"
          elif (runNum >= 222222 and runNum < 289350): # 2015
            IdMappingDat="PixelCabling/Pixels_Atlas_IdMapping_Run2.dat"
          else:
            IdMappingDat="PixelCabling/Pixels_Atlas_IdMapping_344494.dat"

      alg = PixelConfigCondAlg(name="PixelConfigCondAlg", 
                               UseCablingConditions=useCablingConditions,
                               CablingMapFileName=IdMappingDat)
      if not self.usePixMap:
        alg.ReadDeadMapKey = ""
      condSeq += alg

    #########################
    # Deadmap Setup (RUN-3) #
    #########################
    if useNewDeadmapFormat:
      if not conddb.folderRequested("/PIXEL/PixelModuleFeMask"):
        conddb.addFolder("PIXEL_OFL", "/PIXEL/PixelModuleFeMask", className="CondAttrListCollection")
      if not hasattr(condSeq, "PixelDeadMapCondAlg"):
        from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelDeadMapCondAlg
        alg = PixelDeadMapCondAlg(name="PixelDeadMapCondAlg")
        if not self.usePixMap:
          alg.ReadKey = ""
        condSeq += alg

    ########################
    # DCS Conditions Setup #
    ########################
    PixelHVFolder = "/PIXEL/DCS/HV"
    PixelTempFolder = "/PIXEL/DCS/TEMPERATURE"
    PixelDBInstance = "DCS_OFL"

    from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
    if athenaCommonFlags.isOnline():
       PixelHVFolder = "/PIXEL/HLT/DCS/HV"
       PixelTempFolder = "/PIXEL/HLT/DCS/TEMPERATURE"
       PixelDBInstance = "PIXEL_ONL"

    if not conddb.folderRequested(PixelHVFolder):
      conddb.addFolder(PixelDBInstance, PixelHVFolder, className="CondAttrListCollection")
    if not conddb.folderRequested(PixelTempFolder):
      conddb.addFolder(PixelDBInstance, PixelTempFolder, className="CondAttrListCollection")

    if not self.onlineMode and self.useDCS:   #this is only for testing in offline like setup 
      if not conddb.folderRequested("/PIXEL/DCS/FSMSTATE"):
        conddb.addFolder("DCS_OFL", "/PIXEL/DCS/FSMSTATE", className="CondAttrListCollection")
      if not conddb.folderRequested("/PIXEL/DCS/FSMSTATUS"):
        conddb.addFolder("DCS_OFL", "/PIXEL/DCS/FSMSTATUS", className="CondAttrListCollection")

    if not hasattr(condSeq, 'PixelDCSCondStateAlg'):
      from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelDCSCondStateAlg
      condSeq += PixelDCSCondStateAlg(name="PixelDCSCondStateAlg")

    if not hasattr(condSeq, 'PixelDCSCondStatusAlg'):
      from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelDCSCondStatusAlg
      condSeq += PixelDCSCondStatusAlg(name="PixelDCSCondStatusAlg")

    if not hasattr(condSeq, 'PixelDCSCondHVAlg'):
      from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelDCSCondHVAlg
      condSeq += PixelDCSCondHVAlg(name="PixelDCSCondHVAlg", ReadKey=PixelHVFolder)

    if not hasattr(condSeq, 'PixelDCSCondTempAlg'):
      from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelDCSCondTempAlg
      condSeq += PixelDCSCondTempAlg(name="PixelDCSCondTempAlg", ReadKey=PixelTempFolder)

    #########################
    # TDAQ Conditions Setup #
    #########################
    if self.useTDAQ:
      PixelTDAQFolder   = "/TDAQ/Resources/ATLAS/PIXEL/Modules"
      PixelTDAQInstance = "TDAQ_ONL"
      if not conddb.folderRequested(PixelTDAQFolder):
        conddb.addFolder(PixelTDAQInstance, PixelTDAQFolder, className="CondAttrListCollection")

      if not hasattr(condSeq, "PixelTDAQCondAlg"):
        from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelTDAQCondAlg
        condSeq += PixelTDAQCondAlg(name="PixelTDAQCondAlg", ReadKey=PixelTDAQFolder)

    ############################
    # Conditions Summary Setup #
    ############################
    # This is future replacement of the PixelConditionsSummaryTool...

    from PixelConditionsTools.PixelConditionsToolsConf import PixelConditionsSummaryTool
    TrigPixelConditionsSummaryTool = PixelConditionsSummaryTool(name=self.instanceName('PixelConditionsSummaryTool'), 
                                                                UseByteStream=self.useBS)

    if self.useDCS and not self.onlineMode:
      TrigPixelConditionsSummaryTool.IsActiveStates = [ 'READY', 'ON' ]
      TrigPixelConditionsSummaryTool.IsActiveStatus = [ 'OK', 'WARNING' ]

    self.summaryTool = TrigPixelConditionsSummaryTool

    if self._print: print (TrigPixelConditionsSummaryTool)
  
    #####################
    # Calibration Setup #
    #####################
    if not useNewChargeFormat:
      if not conddb.folderRequested("/PIXEL/PixCalib"):
        conddb.addFolderSplitOnline("PIXEL", "/PIXEL/Onl/PixCalib", "/PIXEL/PixCalib", className="CondAttrListCollection")
      if not hasattr(condSeq, 'PixelChargeCalibCondAlg'):
        from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelChargeCalibCondAlg
        condSeq += PixelChargeCalibCondAlg(name="PixelChargeCalibCondAlg", ReadKey="/PIXEL/PixCalib")
    else:
      if not conddb.folderRequested("/PIXEL/ChargeCalibration"):
        conddb.addFolder("PIXEL_OFL", "/PIXEL/ChargeCalibration", className="CondAttrListCollection")
      if not hasattr(condSeq, 'PixelChargeLUTCalibCondAlg'):
        from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelChargeLUTCalibCondAlg
        condSeq += PixelChargeLUTCalibCondAlg(name="PixelChargeLUTCalibCondAlg", ReadKey="/PIXEL/ChargeCalibration")

    #####################
    # Cabling map Setup #
    #####################
    if (conddb.dbdata=="CONDBR2" or (conddb.dbmc=="OFLP200" and geoFlags.isIBL()==True)) and not conddb.folderRequested("/PIXEL/HitDiscCnfg"):
      conddb.addFolderSplitMC("PIXEL","/PIXEL/HitDiscCnfg","/PIXEL/HitDiscCnfg", className="AthenaAttributeList")

    if not hasattr(condSeq, 'PixelHitDiscCnfgAlg'):
      from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelHitDiscCnfgAlg
      condSeq += PixelHitDiscCnfgAlg(name="PixelHitDiscCnfgAlg")

    if not conddb.folderRequested("/PIXEL/ReadoutSpeed"):
      if not (globalflags.DataSource() == 'geant4'):
        conddb.addFolder("PIXEL", "/PIXEL/ReadoutSpeed", className="AthenaAttributeList")
      else:
        conddb.addFolderSplitMC("PIXEL","/PIXEL/ReadoutSpeed","/PIXEL/ReadoutSpeed", className="AthenaAttributeList")

    if not hasattr(condSeq, 'PixelReadoutSpeedAlg'):
      from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelReadoutSpeedAlg
      condSeq += PixelReadoutSpeedAlg(name="PixelReadoutSpeedAlg")

    if (globalflags.DataSource=='data'):
      if not conddb.folderRequested("/PIXEL/CablingMap"):
        conddb.addFolderSplitOnline("PIXEL", "/PIXEL/Onl/CablingMap","/PIXEL/CablingMap", className="AthenaAttributeList")

    if not hasattr(condSeq, 'PixelCablingCondAlg'):
      from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelCablingCondAlg
      condSeq += PixelCablingCondAlg(name="PixelCablingCondAlg",
                                     MappingFile=IdMappingDat,
                                     RodIDForSingleLink40=rodIDForSingleLink40)

    #############################
    # Offline calibration Setup #
    #############################
    if not conddb.folderRequested("/PIXEL/PixReco"):
      conddb.addFolderSplitOnline("PIXEL","/PIXEL/Onl/PixReco","/PIXEL/PixReco",className="DetCondCFloat") 

    if not hasattr(condSeq, 'PixelOfflineCalibCondAlg'):
      from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelOfflineCalibCondAlg
      condSeq += PixelOfflineCalibCondAlg(name="PixelOfflineCalibCondAlg", ReadKey="/PIXEL/PixReco")
      PixelOfflineCalibCondAlg.InputSource = 2

    if not conddb.folderRequested("/Indet/PixelDist"):
      conddb.addFolderSplitOnline("INDET","/Indet/Onl/PixelDist","/Indet/PixelDist",className="DetCondCFloat") 

    if not hasattr(condSeq, 'PixelDistortionAlg'):
      from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelDistortionAlg
      condSeq += PixelDistortionAlg(name="PixelDistortionAlg", ReadKey="/Indet/PixelDist")

    ### configure the special pixel map service
    if not (conddb.folderRequested("/PIXEL/PixMapShort") or conddb.folderRequested("/PIXEL/Onl/PixMapShort")):
      conddb.addFolderSplitOnline("PIXEL","/PIXEL/Onl/PixMapShort","/PIXEL/PixMapShort", className='CondAttrListCollection') 
    if not (conddb.folderRequested("/PIXEL/PixMapLong") or conddb.folderRequested("/PIXEL/Onl/PixMapLong")):
      conddb.addFolderSplitOnline("PIXEL","/PIXEL/Onl/PixMapLong","/PIXEL/PixMapLong", className='CondAttrListCollection')
    if not (conddb.folderRequested("/PIXEL/NoiseMapShort") or conddb.folderRequested("/PIXEL/Onl/NoiseMapShort")):      
      conddb.addFolderSplitOnline("PIXEL","/PIXEL/Onl/NoiseMapShort","/PIXEL/NoiseMapShort", className='CondAttrListCollection')
    if not (conddb.folderRequested("/PIXEL/NoiseMapLong") or conddb.folderRequested("/PIXEL/Onl/NoiseMapLong")):      
      conddb.addFolderSplitOnline("PIXEL","/PIXEL/Onl/NoiseMapLong","/PIXEL/NoiseMapLong", className='CondAttrListCollection')
    if not (conddb.folderRequested("/PIXEL/PixMapOverlay") or conddb.folderRequested("/PIXEL/Onl/PixMapOverlay")):
      conddb.addFolderSplitOnline("PIXEL","/PIXEL/Onl/PixMapOverlay","/PIXEL/PixMapOverlay", className='CondAttrListCollection')

    #######################
    # Lorentz Angle Setup #
    #######################
    if not hasattr(condSeq, 'PixelSiPropertiesCondAlg'):
      from SiPropertiesTool.SiPropertiesToolConf import PixelSiPropertiesCondAlg
      condSeq += PixelSiPropertiesCondAlg(name="PixelSiPropertiesCondAlg")

    from SiPropertiesTool.SiPropertiesToolConf import SiPropertiesTool
    TrigSiPropertiesTool = SiPropertiesTool(name="PixelSiPropertiesTool", DetectorName="Pixel", ReadKey="PixelSiliconPropertiesVector")

    ToolSvc += TrigSiPropertiesTool

    if not hasattr(condSeq, 'PixelSiLorentzAngleCondAlg'):
      from SiLorentzAngleTool.SiLorentzAngleToolConf import PixelSiLorentzAngleCondAlg
      condSeq += PixelSiLorentzAngleCondAlg(name="PixelSiLorentzAngleCondAlg", 
                                            SiPropertiesTool=TrigSiPropertiesTool,
                                            UseMagFieldCache = True,
                                            UseMagFieldDcs = (not athenaCommonFlags.isOnline()))

    from SiLorentzAngleTool.SiLorentzAngleToolConf import SiLorentzAngleTool
    TrigPixelLorentzAngleTool = SiLorentzAngleTool(name=self.instanceName('PixelLorentzAngleTool'), DetectorName="Pixel", SiLorentzAngleCondData="PixelSiLorentzAngleCondData")

    ToolSvc += TrigPixelLorentzAngleTool

  def instanceName(self, toolname):
    return self.prefix+toolname

  pass


#to be moved to
class SCT_ConditionsToolsSetup:
  "Class to simplify setup of SCT_ConditionsTools"
  def __init__(self):
    self._lock = False
    self.config(useDCS=True, onlineMode=False, prefix='')     #default offline settings
    self._print = False

    
    from AthenaCommon.AppMgr import ServiceMgr
    self.svcMgr = ServiceMgr
    from AthenaCommon.AppMgr import ToolSvc
    self.toolSvc = ToolSvc
    
    from IOVDbSvc.CondDB import conddb
    self.condDB = conddb
    
    self.isMC = False
    from AthenaCommon.GlobalFlags import globalflags
    self.eventInfoKey = "ByteStreamEventInfo"
    if globalflags.DataSource() == 'geant4':
      self.isMC = True
      self.eventInfoKey = "McEventInfo" 

  def config(self, useDCS=True, onlineMode=False, prefix=''):
    if not self._lock:
      self.onlineMode=onlineMode
      self.useDCS=useDCS
      self.prefix = prefix
    else:
      print ('Not modifying an instance of SCT_ConditionsToolsSetup as it is locked')

  def lock(self):
    " prevent modifications to this instance "
    self._lock = True


  def createTool(self):
    self.summaryTool = self.initSummaryTool(self.instanceName('InDetSCT_ConditionsSummaryTool'))
    self.flaggedTool = self.initFlaggedTool(self.instanceName('InDetSCT_FlaggedConditionTool'))
    self.configTool  = self.initConfigTool(self.instanceName('InDetSCT_ConfigurationConditionsTool'))
    self.bsErrTool   = self.initBSErrTool(self.instanceName('InDetSCT_ByteStreamErrorsTool'))
    self.calibTool    = self.initCalibTool(self.instanceName('InDetSCT_ReadCalibDataTool'))
    if not self.onlineMode:
      self.monitorTool = self.initMonitorTool(self.instanceName('InDetSCT_MonitorConditionsTool'))

    self.dcsTool     = self.initDcsTool('InDetSCT_DCSConditionsTool')
    self.lorentzTool = self.initLorentzAngleTool('SCTLorentzAngleTool')

    self.summaryToolWoFlagged = self.initSummaryToolWithoutFlagged(self.instanceName('InDetSCT_ConditionsSummaryToolWithoutFlagged'))

    pass

  def initSummaryTool(self, instanceName):
    "Init summary conditions tool"
    from SCT_ConditionsTools.SCT_ConditionsSummaryToolSetup import SCT_ConditionsSummaryToolSetup
    sct_ConditionsSummaryToolSetup = SCT_ConditionsSummaryToolSetup(instanceName)
    sct_ConditionsSummaryToolSetup.setup()
    summaryTool = sct_ConditionsSummaryToolSetup.getTool()
    if self._print:  print (summaryTool)
    return summaryTool

  def initSummaryToolWithoutFlagged(self, instanceName):
    "Init summary conditions tool without flaggedConditionTool"
    from SCT_ConditionsTools.SCT_ConditionsSummaryToolSetup import SCT_ConditionsSummaryToolSetup
    sct_ConditionsSummaryToolSetupWithoutFlagged = SCT_ConditionsSummaryToolSetup(instanceName)
    sct_ConditionsSummaryToolSetupWithoutFlagged.setup()
    summaryToolWoFlagged = sct_ConditionsSummaryToolSetupWithoutFlagged.getTool()
    condTools = []
    for condToolHandle in self.summaryTool.ConditionsTools:
      if condToolHandle not in condTools:
        if not "SCT_FlaggedConditionTool" in condToolHandle.getFullName():
          condTools.append(condToolHandle)
    summaryToolWoFlagged.ConditionsTools = condTools
    if self._print:  print (summaryToolWoFlagged)
    return summaryToolWoFlagged

  def initFlaggedTool(self, instanceName):
    "Init flagged conditions tool"
    from SCT_ConditionsTools.SCT_FlaggedConditionToolSetup import SCT_FlaggedConditionToolSetup
    sct_FlaggedConditionToolSetup = SCT_FlaggedConditionToolSetup()
    sct_FlaggedConditionToolSetup.setToolName(instanceName)
    sct_FlaggedConditionToolSetup.setup()
    flaggedTool = sct_FlaggedConditionToolSetup.getTool()
    if self.prefix == "InDetTrig":
      # SCT_FlaggedCondData_TRIG created by SCT_TrgClusterization is used.
      flaggedTool.SCT_FlaggedCondData = "SCT_FlaggedCondData_TRIG"
      # Otherwise, SCT_FlaggedCondData created by SCT_Clusterization
    if self._print:  print (flaggedTool)
    if not (flaggedTool.getFullName() in self.summaryTool.ConditionsTools):
      self.summaryTool.ConditionsTools+=[flaggedTool]
      return flaggedTool

  def initConfigTool(self, instanceName):
    "Init configuration conditions tool"

    # Set up SCT cabling
    from AthenaCommon.Include import include
    include('InDetRecExample/InDetRecCabling.py')
    
    from InDetTrigRecExample.InDetTrigFlags import InDetTrigFlags
    from IOVDbSvc.CondDB import conddb
    if conddb.dbdata == "COMP200" or InDetTrigFlags.ForceCoraCool():
      sctdaqpath='/SCT/DAQ/Configuration'
    else:
      sctdaqpath='/SCT/DAQ/Config'

    if InDetTrigFlags.ForceCoolVectorPayload():
      sctdaqpath='/SCT/DAQ/Config'

    from SCT_ConditionsTools.SCT_ConfigurationConditionsToolSetup import SCT_ConfigurationConditionsToolSetup
    sct_ConfigurationConditionsToolSetup = SCT_ConfigurationConditionsToolSetup()
    from AthenaCommon.GlobalFlags import globalflags
    if (globalflags.DataSource() == 'data'):
      sct_ConfigurationConditionsToolSetup.setChannelFolder(sctdaqpath+"/Chip")
    else:
      sct_ConfigurationConditionsToolSetup.setChannelFolder(sctdaqpath+"/ChipSlim") # For MC (OFLP200)
    sct_ConfigurationConditionsToolSetup.setModuleFolder(sctdaqpath+"/Module")
    sct_ConfigurationConditionsToolSetup.setMurFolder(sctdaqpath+"/MUR")
    sct_ConfigurationConditionsToolSetup.setToolName(instanceName)
    sct_ConfigurationConditionsToolSetup.setup()
    configTool = sct_ConfigurationConditionsToolSetup.getTool()
    if self._print:  print (configTool)
    if not (configTool.getFullName() in self.summaryTool.ConditionsTools):
      self.summaryTool.ConditionsTools+=[configTool.getFullName()]

    if self._print:  print (self.condDB)
    return configTool

  def initMonitorTool(self, instanceName):
    "Init monitoring conditions tool"
    from SCT_ConditionsTools.SCT_MonitorConditionsToolSetup import SCT_MonitorConditionsToolSetup
    sct_MonitorConditionsToolSetup = SCT_MonitorConditionsToolSetup()
    sct_MonitorConditionsToolSetup.setToolName(instanceName)
    sct_MonitorConditionsToolSetup.setup()
    monitorTool = sct_MonitorConditionsToolSetup.getTool()
    if not (monitorTool.getFullName() in self.summaryTool.ConditionsTools):
      self.summaryTool.ConditionsTools+=[monitorTool.getFullName()]
    return monitorTool

  def initDcsTool(self, instanceName):
    "Init DCS conditions tool"

    from SCT_ConditionsTools.SCT_DCSConditionsToolSetup import SCT_DCSConditionsToolSetup
    sct_DCSConditionsToolSetup = SCT_DCSConditionsToolSetup()
    sct_DCSConditionsToolSetup.setToolName(instanceName)

    dcs_folder="/SCT/DCS"
    db_loc = "DCS_OFL"
    if (not self.isMC):
      dcs_folder="/SCT/HLT/DCS"
      db_loc = "SCT"
    sct_DCSConditionsToolSetup.setDbInstance(db_loc)
    sct_DCSConditionsToolSetup.setStateFolder(dcs_folder+"/CHANSTAT")
    sct_DCSConditionsToolSetup.setHVFolder(dcs_folder+"/HV")
    sct_DCSConditionsToolSetup.setTempFolder(dcs_folder+"/MODTEMP")

    readAllDBFolders = True
    if (not self.isMC):
      readAllDBFolders = False
    if self.onlineMode:
      readAllDBFolders = False
    sct_DCSConditionsToolSetup.setReadAllDBFolders(readAllDBFolders)

    sct_DCSConditionsToolSetup.setup()
    dcsTool = sct_DCSConditionsToolSetup.getTool()

    returnHVTemp = sct_DCSConditionsToolSetup.getReturnHVTemp()
    if ((readAllDBFolders and returnHVTemp) or (not readAllDBFolders and not returnHVTemp)):
      if not (dcsTool.getFullName() in self.summaryTool.ConditionsTools):
        self.summaryTool.ConditionsTools+=[dcsTool.getFullName()]

    if self.isMC:
      if not self.condDB.folderRequested("/SCT/DCS/MPS/LV"):
        self.condDB.addFolder(db_loc,"/SCT/DCS/MPS/LV")

    return dcsTool

  def initBSErrTool(self, instanceName):
    "Init ByteStream errors tool"
    from SCT_ConditionsTools.SCT_ByteStreamErrorsToolSetup import SCT_ByteStreamErrorsToolSetup
    sct_ByteStreamErrorsToolSetup = SCT_ByteStreamErrorsToolSetup()
    sct_ByteStreamErrorsToolSetup.setToolName(instanceName)
    sct_ByteStreamErrorsToolSetup.setConfigTool(self.configTool)
    sct_ByteStreamErrorsToolSetup.setup()
    bsErrTool =sct_ByteStreamErrorsToolSetup.getTool()
    if self._print:  print (bsErrTool)
    if not (bsErrTool.getFullName() in self.summaryTool.ConditionsTools):
      self.summaryTool.ConditionsTools+=[bsErrTool.getFullName()]
    return  bsErrTool

  def initCalibTool(self, instanceName):
    "Init Calibration Data tool"
    from AthenaCommon.GlobalFlags import globalflags
    if (globalflags.DataSource() == 'data'):
      from SCT_ConditionsTools.SCT_ReadCalibDataToolSetup import SCT_ReadCalibDataToolSetup
      sct_ReadCalibDataToolSetup = SCT_ReadCalibDataToolSetup()
      sct_ReadCalibDataToolSetup.setToolName(instanceName)
      sct_ReadCalibDataToolSetup.setup()
      calibTool = sct_ReadCalibDataToolSetup.getTool()
      if not (calibTool.getFullName() in self.summaryTool.ConditionsTools):
        self.summaryTool.ConditionsTools+=[calibTool.getFullName()]
        return  calibTool
    else:
      return None

  def initLorentzAngleTool(self, instanceName):
    # Set up Silicon Conditions Tool
    from SCT_ConditionsTools.SCT_SiliconConditionsToolSetup import SCT_SiliconConditionsToolSetup
    sct_SiliconConditionsToolSetup = SCT_SiliconConditionsToolSetup()
    sct_SiliconConditionsToolSetup.setDcsTool(self.dcsTool)
    sct_SiliconConditionsToolSetup.setToolName("InDetSCT_SiliconConditionsTool")
    sct_SiliconConditionsToolSetup.setup()
    sctSiliconConditionsTool = sct_SiliconConditionsToolSetup.getTool()
    sctSiliconConditionsTool.CheckGeoModel = False
    sctSiliconConditionsTool.ForceUseGeoModel = False
    if self._print: print (sctSiliconConditionsTool)

    # Set up SCTSiLorentzAngleCondAlg
    from AthenaCommon.AlgSequence import AthSequencer
    condSeq = AthSequencer("AthCondSeq")
    if not hasattr(condSeq, "SCTSiLorentzAngleCondAlg"):
      from SiLorentzAngleTool.SiLorentzAngleToolConf import SCTSiLorentzAngleCondAlg
      from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
      condSeq += SCTSiLorentzAngleCondAlg(name = "SCTSiLorentzAngleCondAlg",
                                          SiConditionsTool = sctSiliconConditionsTool,
                                          UseMagFieldCache = True,
                                          UseMagFieldDcs = (not athenaCommonFlags.isOnline()))
      sctSiLorentzAngleCondAlg = condSeq.SCTSiLorentzAngleCondAlg

    "Inititalize Lorentz angle Tool"
    from SiLorentzAngleTool.SiLorentzAngleToolConf import SiLorentzAngleTool
    SCTLorentzAngleTool = SiLorentzAngleTool(name=instanceName, DetectorName="SCT", SiLorentzAngleCondData="SCTSiLorentzAngleCondData")
    SCTLorentzAngleTool.UseMagFieldCache = True
    
  def instanceName(self, toolname):
    return self.prefix+toolname
    
  pass


# Create instance of setup tool
#sctConditionsTool = SCT_ConditionsToolsSetup()

# InDetSCT_ConditionsSummaryTool        = sctConditionsTool.summaryTool
# InDetSCT_ConfigurationConditionsTool  = sctConditionsTool.configTool
# InDetSCT_FlaggedConditionTool         = sctConditionsTool.flaggedTool
# InDetSCT_MonitorConditionsTool        = sctConditionsTool.monitorTool
# InDetSCT_ByteStreamErrorsTool         = sctConditionsTool.bsErrTool
# InDetSCT_ReadCalibDataTool            = sctConditionsTool.calibTool
#if not self.isMC: InDetSCT_DCSConditionsTool            = sctConditionsTool.dcsTool


class TRTConditionsServicesSetup:
  ""
  def __init__(self,printConfigurables=False):
    self._print = printConfigurables    #printConfigurables
    self._lock = False
    self.config(useDCS=True, onlineMode=False, prefix='') 
    self._isMC = False
    from AthenaCommon.GlobalFlags import globalflags
    if globalflags.DataSource() == 'geant4': self._isMC = True
    

  def config(self, useDCS=True, onlineMode=False, prefix=''):
    if not self._lock:
      self.useDCS = useDCS
      self.onlineMode = onlineMode
      self.prefix = prefix
    else:
      print ('Not modifying an instance of TRTConditionsServicesSetup as it is locked')
      

  def lock(self):
    " prevent modifications to this instance "
    self._lock = True

  def createSvc(self):
    
    #
    # Load necessary conditions folders
    #
    
    # Calibration constants
    from IOVDbSvc.CondDB import conddb
    
    if not self._isMC:
      #only needed for data
      if not (conddb.folderRequested('/TRT/Onl/ROD/Compress')):
        conddb.addFolder("TRT_ONL","/TRT/Onl/ROD/Compress",className='CondAttrListCollection')

    if not (conddb.folderRequested('/TRT/Calib/RT') or conddb.folderRequested('/TRT/Onl/Calib/RT')):
      conddb.addFolderSplitOnline('TRT','/TRT/Onl/Calib/RT','/TRT/Calib/RT',className='TRTCond::RtRelationMultChanContainer')
    if not (conddb.folderRequested('/TRT/Calib/T0') or conddb.folderRequested('/TRT/Onl/Calib/T0')):
      conddb.addFolderSplitOnline('TRT','/TRT/Onl/Calib/T0','/TRT/Calib/T0',className='TRTCond::StrawT0MultChanContainer')

    if not (conddb.folderRequested('/TRT/Calib/errors') or conddb.folderRequested('/TRT/Onl/Calib/errors')):
      conddb.addFolderSplitOnline ("TRT","/TRT/Onl/Calib/errors","/TRT/Calib/errors",className='TRTCond::RtRelationMultChanContainer')

    if not (conddb.folderRequested('/TRT/Calib/errors2d') or conddb.folderRequested('/TRT/Onl/Calib/errors2d')):
      conddb.addFolderSplitOnline ("TRT","/TRT/Onl/Calib/errors2d","/TRT/Calib/errors2d",className='TRTCond::RtRelationMultChanContainer')

    if not (conddb.folderRequested('/TRT/Calib/slopes') or conddb.folderRequested('/TRT/Onl/Calib/slopes')):
      conddb.addFolderSplitOnline ("TRT","/TRT/Onl/Calib/slopes","/TRT/Calib/slopes",className='TRTCond::RtRelationMultChanContainer')


    # Calibration DB Service
    from AthenaCommon.AppMgr import ServiceMgr

    # Dead/Noisy Straw Lists
    if not conddb.folderRequested('/TRT/Cond/Status'):
        conddb.addFolderSplitOnline("TRT","/TRT/Onl/Cond/Status","/TRT/Cond/Status",className='TRTCond::StrawStatusMultChanContainer')
    if not conddb.folderRequested('/TRT/Cond/StatusPermanent'):
       conddb.addFolderSplitOnline("TRT","/TRT/Onl/Cond/StatusPermanent","/TRT/Cond/StatusPermanent",className='TRTCond::StrawStatusMultChanContainer')

    # Argon straw list
    if not conddb.folderRequested('/TRT/Cond/StatusHT'):
      conddb.addFolderSplitOnline("TRT","/TRT/Onl/Cond/StatusHT","/TRT/Cond/StatusHT",className='TRTCond::StrawStatusMultChanContainer')

    #these conditions were instantiated together with specific tools using them in InDetTrigRecLoadTools
    #now required for the condAlg
    if not (conddb.folderRequested("/TRT/Calib/PID_vector") or \
            conddb.folderRequested("/TRT/Onl/Calib/PID_vector")):
      conddb.addFolderSplitOnline("TRT","/TRT/Onl/Calib/PID_vector","/TRT/Calib/PID_vector",className='CondAttrListVec')
    if not (conddb.folderRequested("/TRT/Calib/ToT/ToTVectors") or \
            conddb.folderRequested("/TRT/Onl/Calib/ToT/ToTVectors")):
      conddb.addFolderSplitOnline("TRT","/TRT/Onl/Calib/ToT/ToTVectors","/TRT/Calib/ToT/ToTVectors",className='CondAttrListVec')
    if not (conddb.folderRequested("/TRT/Calib/ToT/ToTValue") or \
            conddb.folderRequested("/TRT/Onl/Calib/ToT/ToTValue")):
      conddb.addFolderSplitOnline("TRT","/TRT/Onl/Calib/ToT/ToTValue","/TRT/Calib/ToT/ToTValue",className='CondAttrListCollection')

    # Straw status tool (now private, cannot be passed by name)
    from InDetTrigRecExample.InDetTrigCommonTools import InDetTrigTRTStrawStatusSummaryTool
    
    # Alive straws algorithm
    from TRT_ConditionsAlgs.TRT_ConditionsAlgsConf import TRTStrawCondAlg
    TRTStrawCondAlg = TRTStrawCondAlg(name = "TRTStrawCondAlg",
                                      TRTStrawStatusSummaryTool = InDetTrigTRTStrawStatusSummaryTool,
                                      isGEANT4 = self._isMC)
    # Active Fraction algorithm
    from TRT_ConditionsAlgs.TRT_ConditionsAlgsConf import TRTActiveCondAlg
    TRTActiveCondAlg = TRTActiveCondAlg(name = "TRTActiveCondAlg",
                                      TRTStrawStatusSummaryTool = InDetTrigTRTStrawStatusSummaryTool)


    # HT probability algorithm
    from TRT_ConditionsAlgs.TRT_ConditionsAlgsConf import TRTHTCondAlg
    TRTHTCondAlg = TRTHTCondAlg(name = "TRTHTCondAlg")

    from TRT_ConditionsAlgs.TRT_ConditionsAlgsConf import TRTToTCondAlg
    TRTToTCondAlg = TRTToTCondAlg(name = "TRTToTCondAlg")

    from AthenaCommon.AlgSequence import AthSequencer
    condSeq = AthSequencer("AthCondSeq")

    # Condition algorithms for straw conditions
    if not hasattr(condSeq, "TRTStrawCondAlg"):
        condSeq += TRTStrawCondAlg
    if not hasattr(condSeq, "TRTActiveCondAlg"):
        condSeq += TRTActiveCondAlg
    # Condition algorithms for Pid
    if not hasattr(condSeq, "TRTHTCondAlg"):
        condSeq += TRTHTCondAlg

    if not hasattr(condSeq, "TRTToTCondAlg"):
        condSeq += TRTToTCondAlg
        

    from AthenaCommon.GlobalFlags import globalflags
    
    
    #
    # Load and Configure TRT Conditions Services
    #
    InDetTRTConditionsServices=[]

    
    # Services which only run on raw data
    if (globalflags.InputFormat() == 'bytestream' and globalflags.DataSource() == 'data'):
        
      # Hardware Mapping Service
      from TRT_ConditionsServices.TRT_ConditionsServicesConf import TRT_HWMappingSvc
      InDetTRT_HWMappingSvc = TRT_HWMappingSvc(name=self.instanceName("InDetTRT_HWMappingSvc"))
      ServiceMgr += InDetTRT_HWMappingSvc
      if self._print:
        print (InDetTRT_HWMappingSvc)

      # DCS Conditions Service
      if self.useDCS and not self.onlineMode:
        from TRT_ConditionsServices.TRT_ConditionsServicesConf import TRT_DCS_ConditionsSvc
        InDetTRT_DCS_ConditionsSvc = TRT_DCS_ConditionsSvc(name=self.instanceName("InDetTRT_DCS_ConditionsSvc"),
                                                           HWMapSvc = InDetTRT_HWMappingSvc,
                                                           #OutputLevel = VERBOSE,
                                                           EventInfoKey = "ByteStreamEventInfo",
                                                           DoIOVChecking = True,
                                                           IOVmaxLength = 7*24*60*60,
                                                           #FallBackOnCOOLChanName = False,
                                                           )
        ServiceMgr += InDetTRT_DCS_ConditionsSvc
        if self._print:
          print (InDetTRT_DCS_ConditionsSvc)
        InDetTRTConditionsServices.append(InDetTRT_DCS_ConditionsSvc)
    
    # TRT Conditions Summary Service
    from TRT_ConditionsServices.TRT_ConditionsServicesConf import TRT_ConditionsSummarySvc
    InDetTRTConditionsSummaryService = TRT_ConditionsSummarySvc(name=self.instanceName("InDetTRTConditionsSummaryService"),
                                                                ServiceList=InDetTRTConditionsServices,
                                                                )
    ServiceMgr += InDetTRTConditionsSummaryService
    if self._print:
      print (InDetTRTConditionsSummaryService )


  def instanceName(self, toolname):
    return self.prefix+toolname


############################################################
class dummyConditionsSetup:
  def __init__(self,prefix=''):
    self.prefix=prefix
    
  def instanceName(self, toolname):
    return self.prefix+toolname


############################################################
# instances of the conditions classes

PixelConditionsSetup = PixelConditionsServicesSetup()
SCT_ConditionsSetup = SCT_ConditionsToolsSetup()
TRT_ConditionsSetup = TRTConditionsServicesSetup()
