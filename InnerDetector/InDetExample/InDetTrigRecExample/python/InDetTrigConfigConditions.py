# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

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


  def config(self, useDCS=True, onlineMode=False, prefix=''):
    if not self._lock:
      self.useDCS = useDCS
      self.onlineMode = onlineMode
      self.prefix = prefix
      self.useBS = True
      self.useTDAQ = False
      self.eventInfoKey = "ByteStreamEventInfo"
      from AthenaCommon.GlobalFlags import globalflags
      if globalflags.DataSource() == 'geant4':      #does not work for transbs
        self.useBS = False
        self.eventInfoKey = "McEventInfo" 
        #self.useTDAQ=True
    else:
      print 'Not modifying an instance of PixelConditionsSetup as it is locked'
     

  def lock(self):
    " prevent modifications to this instance "
    self._lock = True


  def createSvc(self):
    from AthenaCommon.AppMgr import ToolSvc,ServiceMgr,theApp
    from IOVDbSvc.CondDB import conddb
    from AthenaCommon.AlgSequence import AthSequencer
    condSeq = AthSequencer("AthCondSeq")

    #Retire service later
    from PixelConditionsServices.PixelConditionsServicesConf import PixelConditionsSummarySvc as pixSummarySvc
    PixelConditionsSummarySvc = \
        pixSummarySvc(name=self.instanceName('PixelConditionsSummarySvc'),
                      UseDCS = self.useDCS,
                      UseByteStream=self.useBS,
                      #UseSpecialPixelMap= not self.onlineMode,
                      UseSpecialPixelMap=True,
                      UseTDAQ=self.useTDAQ
                      )
    #active states used by dcs (if on)
    if self.useDCS and not self.onlineMode:
      PixelConditionsSummarySvc.IsActiveStates = [ 'READY', 'ON' ]
      PixelConditionsSummarySvc.IsActiveStatus = [ 'OK', 'WARNING' ]

    ServiceMgr += PixelConditionsSummarySvc

    if self._print: print PixelConditionsSummarySvc

    from PixelConditionsServices.PixelConditionsServicesConf import PixelConditionsSummaryTool as pixSummaryTool
    PixelConditionsSummaryTool = \
        pixSummaryTool(name=self.instanceName('PixelConditionsSummaryTool'),
                      UseDCS = self.useDCS,
                      UseByteStream=self.useBS,
                      #UseSpecialPixelMap= not self.onlineMode,
                      UseSpecialPixelMap=True,
                      UseTDAQ=self.useTDAQ
                      )
    #active states used by dcs (if on)
    if self.useDCS and not self.onlineMode:
      PixelConditionsSummaryTool.IsActiveStates = [ 'READY', 'ON' ]
      PixelConditionsSummaryTool.IsActiveStatus = [ 'OK', 'WARNING' ]

    ToolSvc += PixelConditionsSummaryTool

    if self._print: print PixelConditionsSummaryTool
    


    # #create another instance of the PixelConditionsSummarySvc w/o BS
    # #   service to be used with RegionSelector
    # InDetTrigRSPixelConditionsSummarySvc = pixSummarySvc(name='InDetTrigRSPixelConditionsSummarySvc',
    #                                                      UseDCS = self.useDCS,
    #                                                      UseByteStream=self.useBS
    #                                                      UseSpecialPixelMap= True,
    #                                                      )
    # ServiceMgr += InDetTrigRSPixelConditionsSummarySvc
    # if self._print: print InDetTrigRSPixelConditionsSummarySvc

    if not self.onlineMode:
      from PixelConditionsServices.PixelConditionsServicesConf import PixelCalibSvc
      PixelCalibSvc = PixelCalibSvc(name=self.instanceName('PixelCalibSvc'))

      if not conddb.folderRequested("/PIXEL/PixCalib"):
        conddb.addFolder("PIXEL_OFL","/PIXEL/PixCalib")

      if self._print: print PixelCalibSvc

      #only when inputsource=1
      #if not conddb.folderRequested("/PIXEL/PixReco"):
      #  conddb.addFolder("PIXEL_OFL","/PIXEL/PixReco")

    if not conddb.folderRequested("/PIXEL/PixReco"):
      conddb.addFolderSplitOnline("PIXEL","/PIXEL/Onl/PixReco","/PIXEL/PixReco") 

    #Configure PixelRecoDbTool
    from PixelConditionsTools.PixelConditionsToolsConf import PixelRecoDbTool
    PixelRecoDbTool = PixelRecoDbTool(name=self.instanceName('PixelRecoDbTool'))
    ToolSvc += PixelRecoDbTool
    PixelRecoDbTool.InputSource = 2
    # if self.onlineMode:
    #   PixelRecoDbTool.InputSource = 1      #after change of run1 conditions
    # else:
    #   PixelRecoDbTool.InputSource = 2

    if self._print:  print PixelRecoDbTool

    #use corresponding PixelRecoDBTool
    from PixelConditionsServices.PixelConditionsServicesConf import PixelOfflineCalibSvc
    PixelOfflineCalibSvc = PixelOfflineCalibSvc(self.instanceName('PixelOfflineCalibSvc'))
    PixelOfflineCalibSvc.PixelRecoDbTool = PixelRecoDbTool
    ServiceMgr += PixelOfflineCalibSvc

    if self._print:  print PixelOfflineCalibSvc
                                                

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

#    from PixelConditionsServices.PixelConditionsServicesConf import SpecialPixelMapSvc

#    SpecialPixelMapSvc = SpecialPixelMapSvc(name='SpecialPixelMapSvc')
#    ServiceMgr += SpecialPixelMapSvc
#    SpecialPixelMapSvc.RegisterCallback = True;
#    SpecialPixelMapSvc.DBFolders = [ "/PIXEL/PixMapShort", "/PIXEL/PixMapLong" ]
#    SpecialPixelMapSvc.SpecialPixelMapKeys = [ "SpecialPixelMap", "SpecialPixelMapLong" ]
  
#    SpecialPixelMapSvc.DBFolders += [ "/PIXEL/NoiseMapShort", "/PIXEL/NoiseMapLong" ]
#    SpecialPixelMapSvc.SpecialPixelMapKeys += [ "NoiseMapShort", "NoiseMapLong" ]
#  
#    SpecialPixelMapSvc.OverlayKey = "PixMapOverlay"
#    SpecialPixelMapSvc.OverlayFolder = "/PIXEL/PixMapOverlay"
#    ServiceMgr += SpecialPixelMapSvc

    #Alg is suppose to replace service, sync withh service for now
#    if "InDetSpecialPixelMapCondAlg" not in condSeq:
#       from PixelConditionsServices.PixelConditionsServicesConf import SpecialPixelMapCondAlg
#       SpecialPixelMapCondAlg = SpecialPixelMapCondAlg(name="SpecialPixelMapCondAlg",
#               DBFolders  = [ "/PIXEL/PixMapShort", "/PIXEL/PixMapLong", "/PIXEL/NoiseMapShort", "/PIXEL/NoiseMapLong"  ],
#               SpecialPixelMapKeys = [ "SpecialPixelMap", "SpecialPixelMapLong", "NoiseMapShort", "NoiseMapLong" ],
#               OverlayFolder       = "/PIXEL/PixMapOverlay",
#               OverlayKey          = "PixMapOverlay")
#       condSeq += SpecialPixelMapCondAlg

    #theApp.CreateSvc += [ 'SpecialPixelMapSvc/%s' % self.instanceName('SpecialPixelMapSvc') ]

    
#    if self._print:  print SpecialPixelMapSvc

    from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
    from AthenaCommon.GlobalFlags import globalflags
    if self.useDCS or self.onlineMode:
      #sim
      if globalflags.DataSource() == 'geant4' or (not athenaCommonFlags.isOnline()):      
        if not conddb.folderRequested('/PIXEL/DCS/TEMPERATURE'):
          conddb.addFolder("DCS_OFL","/PIXEL/DCS/TEMPERATURE")
        if not conddb.folderRequested('/PIXEL/DCS/HV'):
          conddb.addFolder("DCS_OFL","/PIXEL/DCS/HV")
        if not conddb.folderRequested('/PIXEL/DCS/FSMSTATUS'):
          conddb.addFolder("DCS_OFL","/PIXEL/DCS/FSMSTATUS")
        if not conddb.folderRequested('/PIXEL/DCS/FSMSTATE'):
          conddb.addFolder("DCS_OFL","/PIXEL/DCS/FSMSTATE")
      else:
        if not conddb.folderRequested('/PIXEL/HLT/DCS/TEMPERATURE'):
          conddb.addFolder("PIXEL_ONL","/PIXEL/HLT/DCS/TEMPERATURE")
          #conddb.addFolder("PIXEL","/PIXEL/HLT/DCS/TEMPERATURE <tag>PixDCSTemp-UPD1-00</tag>")
        if not conddb.folderRequested('/PIXEL/HLT/DCS/HV'):
          conddb.addFolder("PIXEL_ONL","/PIXEL/HLT/DCS/HV")
          #conddb.addFolder("PIXEL","/PIXEL/HLT/DCS/HV <tag>PixDCSHV-UPD1-00</tag>")
        

      from PixelConditionsServices.PixelConditionsServicesConf import PixelDCSSvc
      InDetPixelDCSSvc =  PixelDCSSvc(name = 'TrigPixelDCSSvc',
                                      RegisterCallback     = True,
                                      TemperatureFolder    = "/PIXEL/DCS/TEMPERATURE",
                                      HVFolder             = "/PIXEL/DCS/HV",
                                      FSMStatusFolder      = "/PIXEL/DCS/FSMSTATUS",
                                      FSMStateFolder       = "/PIXEL/DCS/FSMSTATE",
                                      TemperatureFieldName = "temperature",
                                      HVFieldName          = "HV",
                                      FSMStatusFieldName   = "FSM_status",
                                      FSMStateFieldName    = "FSM_state",
                                      UseFSMStatus         = False,
                                      UseFSMState          = False
                                      )

      if globalflags.DataSource() == 'data':
        if (not athenaCommonFlags.isOnline()):
          InDetPixelDCSSvc.TemperatureFolder = "/PIXEL/DCS/TEMPERATURE"
          InDetPixelDCSSvc.HVFolder = "/PIXEL/DCS/HV"
        else:
          InDetPixelDCSSvc.TemperatureFolder = "/PIXEL/HLT/DCS/TEMPERATURE"
          InDetPixelDCSSvc.HVFolder = "/PIXEL/HLT/DCS/HV"
        
      ServiceMgr += InDetPixelDCSSvc


    if self.useDCS or self.onlineMode:
      if self._print: print InDetPixelDCSSvc


    #this needs also updates how LorentzAngleSvc is accessed ()
    from SiLorentzAngleSvc.SiLorentzAngleSvcConf import SiLorentzAngleSvc
    PixelLorentzAngleSvc = SiLorentzAngleSvc(name='PixelLorentzAngleSvc')
    from PixelConditionsServices.PixelConditionsServicesConf import PixelSiliconConditionsSvc
    pixelSiliconConditionsSvc=PixelSiliconConditionsSvc(name=self.instanceName('PixelSiliconConditionsSvc'),
                                                        PixelDCSSvc = 'TrigPixelDCSSvc')
    ServiceMgr += pixelSiliconConditionsSvc

    PixelLorentzAngleSvc.SiConditionsServices = pixelSiliconConditionsSvc
    PixelLorentzAngleSvc.UseMagFieldSvc = True         #may need also MagFieldSvc instance

    #if self.useDCS or self.onlineMode:
      #if (globalflags.DataSource() == 'data'):
    #else:
      #pixelSiliconConditionsSvc.ForceUseGeoModel = True
      #PixelLorentzAngleSvc.pixelForceUseGeoModel()
    if self._print: 
      print pixelSiliconConditionsSvc
      print PixelLorentzAngleSvc



  def instanceName(self,tool):
    return self.prefix+tool




#to be moved to
class SCT_ConditionsToolsSetup:
  "Class to simplify setup of SCT_ConditionsTools"
  def __init__(self):
    self._lock = False
    self.config(useDCS=True, onlineMode=False, prefix='')     #default offline settings
    self._print = False

    
    from AthenaCommon.AppMgr import ServiceMgr
    self.svcMgr = ServiceMgr
    
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
      print 'Not modifying an instance of SCT_ConditionsServicesSetup as it is locked'

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
    self.lorentzSvc  = self.initLorentzAngleSvc('SCTLorentzAngleSvc')

    self.summaryToolWoFlagged = self.initSummaryToolWithoutFlagged(self.instanceName('InDetSCT_ConditionsSummaryToolWithoutFlagged'))

    pass

  def initSummaryTool(self, instanceName):
    "Init summary conditions tool"
    from SCT_ConditionsTools.SCT_ConditionsSummaryToolSetup import SCT_ConditionsSummaryToolSetup
    sct_ConditionsSummaryToolSetup = SCT_ConditionsSummaryToolSetup()
    sct_ConditionsSummaryToolSetup.setToolName(instanceName)
    sct_ConditionsSummaryToolSetup.setup()
    summaryTool = sct_ConditionsSummaryToolSetup.getTool()
    if self._print:  print summaryTool
    return summaryTool

  def initSummaryToolWithoutFlagged(self, instanceName):
    "Init summary conditions tool without flaggedConditionTool"
    from SCT_ConditionsTools.SCT_ConditionsSummaryToolSetup import SCT_ConditionsSummaryToolSetup
    sct_ConditionsSummaryToolSetupWithoutFlagged = SCT_ConditionsSummaryToolSetup()
    sct_ConditionsSummaryToolSetupWithoutFlagged.setToolName(instanceName)
    sct_ConditionsSummaryToolSetupWithoutFlagged.setup()
    summaryToolWoFlagged = sct_ConditionsSummaryToolSetupWithoutFlagged.getTool()
    condTools = []
    for condToolHandle in self.summaryTool.ConditionsTools:
      condTool = condToolHandle.typeAndName
      if condTool not in condTools:
        if condTool != self.flaggedTool.getFullName():
          condTools.append(condTool)
    summaryToolWoFlagged.ConditionsTools = condTools
    if self._print:  print summaryToolWoFlagged
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
    if self._print:  print flaggedTool
    if not (flaggedTool.getFullName() in self.summaryTool.ConditionsTools):
      self.summaryTool.ConditionsTools+=[flaggedTool.getFullName()]
      return flaggedTool

  def initConfigTool(self, instanceName):
    "Init configuration conditions tool"
    
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
    sct_ConfigurationConditionsToolSetup.setChannelFolder(sctdaqpath+"/Chip")
    sct_ConfigurationConditionsToolSetup.setModuleFolder(sctdaqpath+"/Module")
    sct_ConfigurationConditionsToolSetup.setMurFolder(sctdaqpath+"/MUR")
    sct_ConfigurationConditionsToolSetup.setToolName(instanceName)
    sct_ConfigurationConditionsToolSetup.setup()
    configTool = sct_ConfigurationConditionsToolSetup.getTool()
    if self._print:  print configTool
    if not (configTool.getFullName() in self.summaryTool.ConditionsTools):
      self.summaryTool.ConditionsTools+=[configTool.getFullName()]

    if self._print:  print self.condDB
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
    if self._print:  print bsErrTool
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

  def initLorentzAngleSvc(self, instanceName):
    # Set up Silicon Conditions Service
    from SCT_ConditionsTools.SCT_SiliconConditionsToolSetup import SCT_SiliconConditionsToolSetup
    sct_SiliconConditionsToolSetup = SCT_SiliconConditionsToolSetup()
    sct_SiliconConditionsToolSetup.setDcsTool(self.dcsTool)
    sct_SiliconConditionsToolSetup.setToolName("InDetSCT_SiliconConditionsTool")
    sct_SiliconConditionsToolSetup.setup()
    sctSiliconConditionsTool = sct_SiliconConditionsToolSetup.getTool()
    sctSiliconConditionsTool.CheckGeoModel = False
    sctSiliconConditionsTool.ForceUseGeoModel = False
    if self._print: print sctSiliconConditionsTool

    # Set up SCTSiLorentzAngleCondAlg
    from AthenaCommon.AlgSequence import AthSequencer
    condSeq = AthSequencer("AthCondSeq")
    if not hasattr(condSeq, "SCTSiLorentzAngleCondAlg"):
      from SiLorentzAngleSvc.SiLorentzAngleSvcConf import SCTSiLorentzAngleCondAlg
      from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
      condSeq += SCTSiLorentzAngleCondAlg(name = "SCTSiLorentzAngleCondAlg",
                                          SiConditionsTool = sctSiliconConditionsTool,
                                          UseMagFieldSvc = True,
                                          UseMagFieldDcs = (not athenaCommonFlags.isOnline()))
      sctSiLorentzAngleCondAlg = condSeq.SCTSiLorentzAngleCondAlg

    "Inititalize Lorentz angle Service"
    if not hasattr(self.svcMgr, instanceName):
      from SiLorentzAngleSvc.SiLorentzAngleSvcConf import SiLorentzAngleCHSvc
      self.svcMgr += SiLorentzAngleCHSvc(name = instanceName,
                                         DetectorName = "SCT")
    SCTLorentzAngleSvc = getattr(self.svcMgr, instanceName)
    SCTLorentzAngleSvc.UseMagFieldSvc = True #may need also MagFieldSvc instance
    
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
      print 'Not modifying an instance of TRTConditionsServicesSetup as it is locked'
      

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
        conddb.addFolder("TRT_ONL","/TRT/Onl/ROD/Compress")

    if not (conddb.folderRequested('/TRT/Calib/RT') or conddb.folderRequested('/TRT/Onl/Calib/RT')):
      conddb.addFolderSplitOnline('TRT','/TRT/Onl/Calib/RT','/TRT/Calib/RT',className='TRTCond::RtRelationMultChanContainer')
    if not (conddb.folderRequested('/TRT/Calib/T0') or conddb.folderRequested('/TRT/Onl/Calib/T0')):
      conddb.addFolderSplitOnline('TRT','/TRT/Onl/Calib/T0','/TRT/Calib/T0',className='TRTCond::StrawT0MultChanContainer')

    if not (conddb.folderRequested('/TRT/Calib/errors') or conddb.folderRequested('/TRT/Onl/Calib/errors')):
      conddb.addFolderSplitOnline ("TRT","/TRT/Onl/Calib/errors","/TRT/Calib/errors",className='TRTCond::RtRelationMultChanContainer')
      # not needed anymore conddb.addOverride('/TRT/Onl/Calib/errors','TrtCalibErrorsOnl-ErrorVal-00-00')

    if not (conddb.folderRequested('/TRT/Calib/ToTCalib') or conddb.folderRequested('/TRT/Onl/Calib/ToTCalib')):
      conddb.addFolderSplitOnline("TRT","/TRT/Onl/Calib/ToTCalib","/TRT/Calib/ToTCalib")

    if not (conddb.folderRequested('/TRT/Calib/HTCalib') or conddb.folderRequested('/TRT/Onl/Calib/HTCalib')):
      conddb.addFolderSplitOnline("TRT","/TRT/Onl/Calib/HTCalib","/TRT/Calib/HTCalib")

    # Calibration DB Service
    from AthenaCommon.AppMgr import ServiceMgr
    from TRT_ConditionsServices.TRT_ConditionsServicesConf import TRT_CalDbSvc
    #InDetTRTCalDbSvc = TRT_CalDbSvc(self.instanceName('TRT_CalDbSvc'))    #
    InDetTRTCalDbSvc = TRT_CalDbSvc('TRT_CalDbSvc')
    ServiceMgr += InDetTRTCalDbSvc
    if self._print:
        print InDetTRTCalDbSvc


    # Dead/Noisy Straw Lists
    if not (conddb.folderRequested('/TRT/Cond/Status') or conddb.folderRequested('/TRT/Onl/Cond/Status')):
      conddb.addFolderSplitOnline("TRT","/TRT/Onl/Cond/Status", "/TRT/Cond/Status")
    #there used to be an additional tag here!
    #conddb.addFolder("TRT","/TRT/Cond/StatusPermanent<tag>TrtStrawStatusPermanent-01</tag>")
    if not (conddb.folderRequested('/TRT/Cond/StatusPermanent') or conddb.folderRequested('/TRT/Onl/Cond/StatusPermanent')):
      conddb.addFolderSplitOnline("TRT","/TRT/Onl/Cond/StatusPermanent", "/TRT/Cond/StatusPermanent")

    #high threshold capabilities
    if not (conddb.folderRequested('/TRT/Cond/StatusHT') or conddb.folderRequested('/TRT/Onl/Cond/StatusHT')):
      conddb.addFolderSplitOnline("TRT","/TRT/Onl/Cond/StatusHT", "/TRT/Cond/StatusHT")


    # DCS Data Folders
    from AthenaCommon.GlobalFlags import globalflags
    if (globalflags.InputFormat() == 'bytestream' and globalflags.DataSource() == 'data'):
      if self.useDCS and not self.onlineMode:
        if not conddb.folderRequested('/TRT/DCS/HV/BARREL <cache>600</cache>'):
          conddb.addFolder('DCS_OFL',"/TRT/DCS/HV/BARREL <cache>600</cache>")
        if not conddb.folderRequested('/TRT/DCS/HV/ENDCAPA <cache>600</cache>'):
          conddb.addFolder('DCS_OFL',"/TRT/DCS/HV/ENDCAPA <cache>600</cache>")
        if not conddb.folderRequested('/TRT/DCS/HV/ENDCAPC <cache>600</cache>'):
          conddb.addFolder('DCS_OFL',"/TRT/DCS/HV/ENDCAPC <cache>600</cache>")
    
    # TRT PID tools
    if not (conddb.folderRequested('/TRT/Calib/PID') or conddb.folderRequested('/TRT/Onl/Calib/PID')):
      conddb.addFolderSplitOnline("TRT","/TRT/Onl/Calib/PID","/TRT/Calib/PID")
    if not (conddb.folderRequested('/TRT/Calib/PIDver_New') or conddb.folderRequested('/TRT/Onl/Calib/PIDver_New')):
      conddb.addFolderSplitOnline("TRT","/TRT/Onl/Calib/PIDver_New","/TRT/Calib/PIDver_New")
    if not (conddb.folderRequested('/TRT/Calib/PID_RToTver_New') or conddb.folderRequested('/TRT/Onl/Calib/PID_RToTver_New')):
      conddb.addFolderSplitOnline("TRT","/TRT/Onl/Calib/PID_RToTver_New","/TRT/Calib/PID_RToTver_New")

    
    #
    # Load and Configure TRT Conditions Services
    #
    InDetTRTConditionsServices=[]

    # Dead/Noisy Straw Service
    from TRT_ConditionsServices.TRT_ConditionsServicesConf import TRT_StrawStatusSummarySvc
    InDetTRTStrawStatusSummarySvc = \
        TRT_StrawStatusSummarySvc(name=self.instanceName("InDetTRTStrawStatusSummarySvc"))
    ServiceMgr += InDetTRTStrawStatusSummarySvc
    InDetTRTConditionsServices.append(InDetTRTStrawStatusSummarySvc)

    if self._print:
      print InDetTRTStrawStatusSummarySvc
    
    # Services which only run on raw data
    if (globalflags.InputFormat() == 'bytestream' and globalflags.DataSource() == 'data'):
        
      # Hardware Mapping Service
      from TRT_ConditionsServices.TRT_ConditionsServicesConf import TRT_HWMappingSvc
      InDetTRT_HWMappingSvc = TRT_HWMappingSvc(name=self.instanceName("InDetTRT_HWMappingSvc"))
      ServiceMgr += InDetTRT_HWMappingSvc
      if self._print:
        print InDetTRT_HWMappingSvc

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
          print InDetTRT_DCS_ConditionsSvc
        InDetTRTConditionsServices.append(InDetTRT_DCS_ConditionsSvc)
    
    # TRT Conditions Summary Service
    from TRT_ConditionsServices.TRT_ConditionsServicesConf import TRT_ConditionsSummarySvc
    InDetTRTConditionsSummaryService = TRT_ConditionsSummarySvc(name=self.instanceName("InDetTRTConditionsSummaryService"),
                                                                ServiceList=InDetTRTConditionsServices,
                                                                )
    ServiceMgr += InDetTRTConditionsSummaryService
    if self._print:
      print InDetTRTConditionsSummaryService 

    from TRT_RecoConditionsServices.TRT_RecoConditionsServicesConf import TRT_ActiveFractionSvc
    InDetTRT_ActiveFractionSvc = TRT_ActiveFractionSvc(name=self.instanceName("InDetTRTActiveFractionSvc"),
                                                       #missing link to TRTSummarySvc
                                                       )
    ServiceMgr += InDetTRT_ActiveFractionSvc


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
