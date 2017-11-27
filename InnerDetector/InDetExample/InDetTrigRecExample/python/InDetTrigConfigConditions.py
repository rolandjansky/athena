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
      conddb.addFolderSplitOnline("PIXEL","/PIXEL/Onl/PixMapShort","/PIXEL/PixMapShort") 
    if not (conddb.folderRequested("/PIXEL/PixMapLong") or conddb.folderRequested("/PIXEL/Onl/PixMapLong")):
      conddb.addFolderSplitOnline("PIXEL","/PIXEL/Onl/PixMapLong","/PIXEL/PixMapLong")
    if not (conddb.folderRequested("/PIXEL/NoiseMapShort") or conddb.folderRequested("/PIXEL/Onl/NoiseMapShort")):      
      conddb.addFolderSplitOnline("PIXEL","/PIXEL/Onl/NoiseMapShort","/PIXEL/NoiseMapShort")
    if not (conddb.folderRequested("/PIXEL/NoiseMapLong") or conddb.folderRequested("/PIXEL/Onl/NoiseMapLong")):      
      conddb.addFolderSplitOnline("PIXEL","/PIXEL/Onl/NoiseMapLong","/PIXEL/NoiseMapLong")
    if not (conddb.folderRequested("/PIXEL/PixMapOverlay") or conddb.folderRequested("/PIXEL/Onl/PixMapOverlay")):
      conddb.addFolderSplitOnline("PIXEL","/PIXEL/Onl/PixMapOverlay","/PIXEL/PixMapOverlay")

    from PixelConditionsServices.PixelConditionsServicesConf import SpecialPixelMapSvc

    SpecialPixelMapSvc = SpecialPixelMapSvc(name='SpecialPixelMapSvc')
    ServiceMgr += SpecialPixelMapSvc
  
    SpecialPixelMapSvc.DBFolders = [ "/PIXEL/PixMapShort", "/PIXEL/PixMapLong" ]
    SpecialPixelMapSvc.SpecialPixelMapKeys = [ "SpecialPixelMap", "SpecialPixelMapLong" ]
  
    SpecialPixelMapSvc.DBFolders += [ "/PIXEL/NoiseMapShort", "/PIXEL/NoiseMapLong" ]
    SpecialPixelMapSvc.SpecialPixelMapKeys += [ "NoiseMapShort", "NoiseMapLong" ]
  
    SpecialPixelMapSvc.OverlayKey = "PixMapOverlay"
    SpecialPixelMapSvc.OverlayFolder = "/PIXEL/PixMapOverlay"

  
    ServiceMgr += SpecialPixelMapSvc
    #theApp.CreateSvc += [ 'SpecialPixelMapSvc/%s' % self.instanceName('SpecialPixelMapSvc') ]

    
    if self._print:  print SpecialPixelMapSvc

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
    pixelSiliconConditionsSvc.ForceUseGeoModel = False
    pixelSiliconConditionsSvc.UseDBForHV=True
    pixelSiliconConditionsSvc.UseDBForTemperature=True
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
class SCT_ConditionsServicesSetup:
  "Class to simplify setup of SCT_ConditionsSerivces"
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


  def createSvc(self):
    self.summarySvc  = self.initSummarySvc(self.instanceName('InDetSCT_ConditionsSummarySvc'))     
    self.flaggedSvc  = self.initFlaggedSvc(self.instanceName('InDetSCT_FlaggedConditionSvc'))      
    self.configSvc   = self.initConfigSvc(self.instanceName('InDetSCT_ConfigurationConditionsSvc'))
    self.bsErrSvc    = self.initBSErrSvc(self.instanceName('InDetSCT_ByteStreamErrorsSvc'))
    self.calibSvc    = self.initCalibSvc(self.instanceName('InDetSCT_ReadCalibDataSvc'))
    if not self.onlineMode:
      self.monitorSvc  = self.initMonitorSvc(self.instanceName('InDetSCT_MonitorConditionsSvc'))

    self.dcsSvc      = self.initDcsSvc('InDetSCT_DCSConditionsSvc')     
    self.lorentzSvc  = self.initLorentzAngleSvc('SCTLorentzAngleSvc')
    pass

  def initSummarySvc(self, instanceName):
    "Init summary conditions servic"
        
    if hasattr(self.svcMgr,instanceName):
      summarySvc = getattr(self.svcMgr, instanceName); 
    else:
      from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_ConditionsSummarySvc
      summarySvc = SCT_ConditionsSummarySvc(name = instanceName)
      self.svcMgr += summarySvc
      if self._print:  print summarySvc
    return summarySvc

  def initFlaggedSvc(self, instanceName):
    "Init flagged conditions service"
      
    if hasattr(self.svcMgr,instanceName):
      flaggedSvc = getattr(self.svcMgr, instanceName); 
    else :
      from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_FlaggedConditionSvc
      flaggedSvc = SCT_FlaggedConditionSvc(name = instanceName)
      self.svcMgr += flaggedSvc
      if self._print:  print flaggedSvc

    self.summarySvc.ConditionsServices+=[instanceName]
    return flaggedSvc

  def initConfigSvc(self, instanceName):
    "Init configuration conditions service"
    
    from InDetTrigRecExample.InDetTrigFlags import InDetTrigFlags
    from IOVDbSvc.CondDB import conddb
    if conddb.dbdata == "COMP200" or InDetTrigFlags.ForceCoraCool():
      sctdaqpath='/SCT/DAQ/Configuration'
    else:
      sctdaqpath='/SCT/DAQ/Config'

    if InDetTrigFlags.ForceCoolVectorPayload():
      sctdaqpath='/SCT/DAQ/Config'


    if not self.condDB.folderRequested(sctdaqpath+'/Chip'):
      self.condDB.addFolderSplitMC("SCT",
                                   sctdaqpath+'/Chip',
                                   sctdaqpath+'/Chip',
                                   className="CondAttrListVec")
    if not self.condDB.folderRequested(sctdaqpath+'/Module'):
      self.condDB.addFolderSplitMC("SCT",
                                   sctdaqpath+'/Module',
                                   sctdaqpath+'/Module',
                                   className="CondAttrListVec")
    if not self.condDB.folderRequested(sctdaqpath+'/MUR'):
      self.condDB.addFolderSplitMC("SCT",
                                   sctdaqpath+'/MUR',
                                   sctdaqpath+'/MUR',
                                   className="CondAttrListVec")

    if not self.condDB.folderRequested(sctdaqpath+'/ROD'):
      self.condDB.addFolderSplitMC("SCT",
                                   sctdaqpath+'/ROD',
                                   sctdaqpath+'/ROD')
    if not self.condDB.folderRequested(sctdaqpath+'/RODMUR'):
      self.condDB.addFolderSplitMC("SCT",
                                   sctdaqpath+'/RODMUR',
                                   sctdaqpath+'/RODMUR')
    if not self.condDB.folderRequested(sctdaqpath+'/Geog'):
      self.condDB.addFolderSplitMC("SCT",
                                   sctdaqpath+'/Geog',
                                   sctdaqpath+'/Geog')

    if hasattr(self.svcMgr,instanceName):
      configSvc = getattr(self.svcMgr, instanceName); 
    else:
      from AthenaCommon.AlgSequence import AthSequencer
      condSeq = AthSequencer("AthCondSeq")
      if not hasattr(condSeq, "SCT_ConfigurationCondAlg"):
        from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_ConfigurationCondAlg
        condSeq += SCT_ConfigurationCondAlg(name = "SCT_ConfigurationCondAlg",
                                            ReadKeyChannel= sctdaqpath+'/Chip',
                                            ReadKeyModule = sctdaqpath+'/Module',
                                            ReadKeyMur = sctdaqpath+'/MUR')

      from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_ConfigurationConditionsSvc
      configSvc = SCT_ConfigurationConditionsSvc(name = instanceName)
      self.svcMgr += configSvc
      if self._print:  print configSvc

    self.summarySvc.ConditionsServices+=[instanceName]

    if self._print:  print self.condDB
    return configSvc

  def initMonitorSvc(self, instanceName):
    "Init monitoring conditions service"
    
    if hasattr(self.svcMgr,instanceName):
      monitorSvc = getattr(self.svcMgr, instanceName); 
    else:        
      from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_MonitorConditionsSvc
      monitorSvc = SCT_MonitorConditionsSvc(name = instanceName)
                                            #OutputLevel = INFO)
      self.svcMgr += monitorSvc

    self.summarySvc.ConditionsServices+=[instanceName]
    
    #if not self.condDB.folderRequested('/SCT/Derived/Monitoring'):
    #  self.condDB.addFolder("SCT","/SCT/Derived/Monitoring")
        
    return monitorSvc

  def initDcsSvc(self, instanceName):
    "Init DCS conditions service"
    dcs_folder="/SCT/DCS"
    db_loc = "DCS_OFL"
    if (not self.isMC): 
      dcs_folder="/SCT/HLT/DCS"
      db_loc = "SCT"

    sctDCSStateFolder = dcs_folder+'/CHANSTAT'
    sctDCSTempFolder = dcs_folder+'/MODTEMP'
    sctDCSHVFolder = dcs_folder+'/HV'

    if hasattr(self.svcMgr,instanceName):
      dcsSvc = getattr(self.svcMgr, instanceName); 
    else:
      from AthenaCommon.AlgSequence import AthSequencer
      condSeq = AthSequencer("AthCondSeq")
      if not hasattr(condSeq, "SCT_DCSConditionsHVCondAlg"):
        from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_DCSConditionsHVCondAlg
        condSeq += SCT_DCSConditionsHVCondAlg(name = "SCT_DCSConditionsHVCondAlg",
                                              ReadKey = sctDCSHVFolder)
      if not hasattr(condSeq, "SCT_DCSConditionsStatCondAlg"):
        from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_DCSConditionsStatCondAlg
        condSeq += SCT_DCSConditionsStatCondAlg(name = "SCT_DCSConditionsStatCondAlg",
                                                ReadKeyHV = sctDCSHVFolder,
                                                ReadKeyState = sctDCSStateFolder)
      if not hasattr(condSeq, "SCT_DCSConditionsTempCondAlg"):
        from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_DCSConditionsTempCondAlg
        condSeq += SCT_DCSConditionsTempCondAlg(name = "SCT_DCSConditionsTempCondAlg",
                                                ReadKey = sctDCSTempFolder)

      from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_DCSConditionsSvc
      dcsSvc = SCT_DCSConditionsSvc(name = instanceName)
      if (not self.isMC):
        dcsSvc.FolderLocation="/SCT/HLT/DCS"
        dcsSvc.ReadAllDBFolders=False
        dcsSvc.ReturnHVTemp=True

    if self.onlineMode:
      dcsSvc.ReadAllDBFolders=False
      dcsSvc.ReturnHVTemp=True

    self.svcMgr += dcsSvc
      
    self.summarySvc.ConditionsServices+=[instanceName]

    if not self.condDB.folderRequested(sctDCSHVFolder):
      self.condDB.addFolder(db_loc, sctDCSHVFolder, className="CondAttrListCollection")
    if not self.condDB.folderRequested(sctDCSTempFolder):
      self.condDB.addFolder(db_loc, sctDCSTempFolder, className="CondAttrListCollection")
      
    if self.isMC:
      if not self.condDB.folderRequested('/SCT/DCS/MPS/LV'):
        self.condDB.addFolder(db_loc,"/SCT/DCS/MPS/LV")
      if not self.condDB.folderRequested(sctDCSStateFolder):
        self.condDB.addFolder(db_loc, sctDCSStateFolder, className="CondAttrListCollection")

      
      
    return dcsSvc           

  def initBSErrSvc(self, instanceName):
    "Init ByteStream errors service"
    
    if hasattr(self.svcMgr,instanceName):
      bsErrSvc = getattr(self.svcMgr, instanceName); 
    else:
      from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_ByteStreamErrorsSvc
      bsErrSvc = SCT_ByteStreamErrorsSvc(name = instanceName)
      self.svcMgr += bsErrSvc
      if self._print:  print bsErrSvc
      
    self.summarySvc.ConditionsServices+=[instanceName]
    return  bsErrSvc

  def initCalibSvc(self, instanceName):
    "Init Calibration Data service"
    from AthenaCommon.GlobalFlags import globalflags
    if (globalflags.DataSource() == 'data'):
      if hasattr(self.svcMgr,instanceName):
        calibSvc = getattr(self.svcMgr, instanceName); 
      else:
        from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_ReadCalibDataSvc
        calibSvc = SCT_ReadCalibDataSvc(name = instanceName)
        self.svcMgr += calibSvc

      self.summarySvc.ConditionsServices+=[instanceName]

      if not self.condDB.folderRequested('/SCT/DAQ/Calibration/NPtGainDefects'):
        self.condDB.addFolderSplitMC("SCT",
                                     "/SCT/DAQ/Calibration/NPtGainDefects",
                                     "/SCT/DAQ/Calibration/NPtGainDefects")
      if not self.condDB.folderRequested('/SCT/DAQ/Calibration/NoiseOccupancyDefects'):
        self.condDB.addFolderSplitMC("SCT",
                                     "/SCT/DAQ/Calibration/NoiseOccupancyDefects",
                                     "/SCT/DAQ/Calibration/NoiseOccupancyDefects")
      return  calibSvc
    else:
      return None

  def initLorentzAngleSvc(self, instanceName):
    "Inititalize Lorentz angle Service"
    from SiLorentzAngleSvc.SiLorentzAngleSvcConf import SiLorentzAngleSvc
    SCTLorentzAngleSvc = SiLorentzAngleSvc(name=instanceName,
                                           DetectorName="SCT")
    from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_SiliconConditionsSvc
    sctSiliconConditionsSvc=\
                              SCT_SiliconConditionsSvc(name=self.instanceName('InDetSCT_SiliconConditionsSvc'),
                                                       DCSConditionsSvc=self.dcsSvc)
    sctSiliconConditionsSvc.CheckGeoModel = False
    sctSiliconConditionsSvc.ForceUseGeoModel = False
    #sctSiliconConditionsSvc.OutputLevel=1
    self.svcMgr += sctSiliconConditionsSvc
    if self._print: print sctSiliconConditionsSvc

    SCTLorentzAngleSvc.SiConditionsServices = sctSiliconConditionsSvc
    SCTLorentzAngleSvc.UseMagFieldSvc = True       #may need also MagFieldSvc instance

    self.svcMgr += SCTLorentzAngleSvc
    
  def instanceName(self, toolname):
    return self.prefix+toolname
    
  pass


# Create instance of setup service 
#sctConditionsSvc = SCT_ConditionsServicesSetup()

# InDetSCT_ConditionsSummarySvc        = sctConditionsSvc.summarySvc
# InDetSCT_ConfigurationConditionsSvc  = sctConditionsSvc.configSvc
# InDetSCT_FlaggedConditionSvc         = sctConditionsSvc.flaggedSvc
# InDetSCT_MonitorConditionsSvc        = sctConditionsSvc.monitorSvc
# InDetSCT_ByteStreamErrorsSvc         = sctConditionsSvc.bsErrSvc
# InDetSCT_ReadCalibDataSvc            = sctConditionsSvc.calibSvc
#if not self.isMC: InDetSCT_DCSConditionsSvc            = sctConditionsSvc.dcsSvc


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
SCT_ConditionsSetup = SCT_ConditionsServicesSetup()
TRT_ConditionsSetup = TRTConditionsServicesSetup()
