# block include of file, this is used by many packages
include.block ("InDetRecExample/InDetRecConditionsAccess.py")

isData = (globalflags.DataSource == 'data')

if not ('conddb' in dir()):
  IOVDbSvc = Service("IOVDbSvc")
  from IOVDbSvc.CondDB import conddb

#
# --- Load PixelConditionsServices
#
if DetFlags.haveRIO.pixel_on():
    # Load pixel conditions summary service
    from PixelConditionsServices.PixelConditionsServicesConf import PixelConditionsSummarySvc
    InDetPixelConditionsSummarySvc = PixelConditionsSummarySvc()
  
    # Load pixel calibration service
    if not athenaCommonFlags.isOnline():
        if not conddb.folderRequested('/PIXEL/PixCalib'):
            conddb.addFolder("PIXEL_OFL","/PIXEL/PixCalib")
        from PixelConditionsServices.PixelConditionsServicesConf import PixelCalibSvc
        InDetPixelCalibSvc = PixelCalibSvc()
        ServiceMgr += InDetPixelCalibSvc
        if InDetFlags.doPrintConfigurables():
            print InDetPixelCalibSvc

    # Load pixel special pixel map services
    if athenaCommonFlags.isOnline() :
       InDetPixelConditionsSummarySvc.UseSpecialPixelMap = False
    else:
        InDetPixelConditionsSummarySvc.UseSpecialPixelMap = True
        if not conddb.folderRequested('/PIXEL/PixMapShort'):
            conddb.addFolder("PIXEL_OFL","/PIXEL/PixMapShort")
        if not conddb.folderRequested('/PIXEL/PixMapLong'):
            conddb.addFolder("PIXEL_OFL","/PIXEL/PixMapLong")
        if not conddb.folderRequested('/PIXEL/NoiseMapShort'):
            conddb.addFolder("PIXEL_OFL","/PIXEL/NoiseMapShort")
        if not conddb.folderRequested('/PIXEL/NoiseMapLong'):
            conddb.addFolder("PIXEL_OFL","/PIXEL/NoiseMapLong")
        if not conddb.folderRequested('/PIXEL/PixMapOverlay'):
            conddb.addFolder("PIXEL_OFL","/PIXEL/PixMapOverlay")
        from PixelConditionsServices.PixelConditionsServicesConf import SpecialPixelMapSvc
        InDetSpecialPixelMapSvc = SpecialPixelMapSvc(DBFolders           = [ "/PIXEL/PixMapShort", "/PIXEL/PixMapLong" , "/PIXEL/NoiseMapShort", "/PIXEL/NoiseMapLong" ] ,
                                                     SpecialPixelMapKeys = [ "SpecialPixelMap", "SpecialPixelMapLong", "NoiseMapShort", "NoiseMapLong" ] ,
                                                     OverlayFolder       = "/PIXEL/PixMapOverlay",
                                                     OverlayKey          = "PixMapOverlay")
        ServiceMgr += InDetSpecialPixelMapSvc
        if InDetFlags.doPrintConfigurables():
            print InDetSpecialPixelMapSvc
    
    # Load pixel DCS information
    from SiLorentzAngleSvc.PixelLorentzAngleSvcSetup import pixelLorentzAngleSvcSetup
    if InDetFlags.usePixelDCS():
        from PixelConditionsServices.PixelConditionsServicesConf import PixelDCSSvc
        if athenaCommonFlags.isOnline():
            if not conddb.folderRequested('/PIXEL/HLT/DCS/TEMPERATURE'):
                conddb.addFolder("PIXEL_ONL","/PIXEL/HLT/DCS/TEMPERATURE")
            if not conddb.folderRequested('/PIXEL/HLT/DCS/HV'):
                conddb.addFolder("PIXEL_ONL","/PIXEL/HLT/DCS/HV")
                
            InDetPixelDCSSvc =  PixelDCSSvc(RegisterCallback     = TRUE,
                                            TemperatureFolder    = "/PIXEL/HLT/DCS/TEMPERATURE",
                                            HVFolder             = "/PIXEL/HLT/DCS/HV",
                                            TemperatureFieldName = "temperature",
                                            HVFieldName          = "HV")
        else:
            if not conddb.folderRequested('/PIXEL/DCS/TEMPERATURE'):
                conddb.addFolder("DCS_OFL","/PIXEL/DCS/TEMPERATURE")
            if not conddb.folderRequested('/PIXEL/DCS/HV'):
                conddb.addFolder("DCS_OFL","/PIXEL/DCS/HV")
            if not conddb.folderRequested('/PIXEL/DCS/FSMSTATUS'):
                conddb.addFolder("DCS_OFL","/PIXEL/DCS/FSMSTATUS")
            if not conddb.folderRequested('/PIXEL/DCS/FSMSTATE'):
                conddb.addFolder("DCS_OFL","/PIXEL/DCS/FSMSTATE")
            from AtlasGeoModel.InDetGMJobProperties import GeometryFlags as geoFlags
            if (rec.doMonitoring() and globalflags.DataSource() == 'data' and geoFlags.Run() == "RUN2" and conddb.dbdata == "CONDBR2"): 
                # geoFlags.isIBL() == True may work too instead of geoFlags.Run() == "RUN2"
                if not conddb.folderRequested('/PIXEL/DCS/PIPES'):
                    conddb.addFolder("DCS_OFL","/PIXEL/DCS/PIPES")
                if not conddb.folderRequested('/PIXEL/DCS/LV'):
                    conddb.addFolder("DCS_OFL","/PIXEL/DCS/LV")
                if not conddb.folderRequested('/PIXEL/DCS/HVCURRENT'):
                    conddb.addFolder("DCS_OFL","/PIXEL/DCS/HVCURRENT")
                # not used anymore
                # if not conddb.folderRequested('/PIXEL/DCS/PLANTS'):
                #    conddb.addFolder("DCS_OFL","/PIXEL/DCS/PLANTS")
            
            InDetPixelDCSSvc =  PixelDCSSvc(RegisterCallback     = TRUE,
                                            TemperatureFolder    = "/PIXEL/DCS/TEMPERATURE",
                                            HVFolder             = "/PIXEL/DCS/HV",
                                            FSMStatusFolder      = "/PIXEL/DCS/FSMSTATUS",
                                            FSMStateFolder       = "/PIXEL/DCS/FSMSTATE",
                                            TemperatureFieldName = "temperature",
                                            HVFieldName          = "HV",
                                            FSMStatusFieldName   = "FSM_status",
                                            FSMStateFieldName    = "FSM_state" )
        ServiceMgr += InDetPixelDCSSvc
        if InDetFlags.doPrintConfigurables():
            print InDetPixelDCSSvc

        # temporarily workaround incomplete conditions data for MC
        #  by only enabling the usage of dcs in the pixel conditions summary service for data
        InDetPixelConditionsSummarySvc.UseDCS         = isData
        InDetPixelConditionsSummarySvc.IsActiveStates = [ 'READY', 'ON', 'UNKNOWN', 'TRANSITION', 'UNDEFINED' ]
        InDetPixelConditionsSummarySvc.IsActiveStatus = [ 'OK', 'WARNING', 'ERROR', 'FATAL' ]
        # Force Lorentz angle calculation to use DCS for data
        if (globalflags.DataSource() == 'data'):
            pixelLorentzAngleSvcSetup.forceUseDB()
    else:
        pixelLorentzAngleSvcSetup.forceUseGeoModel()

    # Load Pixel BS errors service
    if ( globalflags.DataSource == 'geant4' ) :
        # Due to a "feature" in the BS encoder for simulation,
        # the information of the BS error service
        # is not reliable on MC.
        InDetPixelConditionsSummarySvc.UseByteStream = False 
    else :
        from PixelConditionsServices.PixelConditionsServicesConf import PixelByteStreamErrorsSvc
        InDetPixelByteStreamErrorsSvc = PixelByteStreamErrorsSvc()
        if ( globalflags.InputFormat != 'bytestream' ):
            InDetPixelByteStreamErrorsSvc.ReadingESD = True
        InDetPixelConditionsSummarySvc.UseByteStream = True

        ServiceMgr += InDetPixelByteStreamErrorsSvc
        if (InDetFlags.doPrintConfigurables()):
            print InDetPixelByteStreamErrorsSvc
            
    # Load Pixel reconstruction parameters
    from PixelConditionsTools.PixelConditionsToolsConf import PixelRecoDbTool
    ToolSvc += PixelRecoDbTool(name = "PixelRecoDbTool")

    if athenaCommonFlags.isOnline() :
        ToolSvc.PixelRecoDbTool.InputSource = 1
    else :
        if not conddb.folderRequested('/PIXEL/Pixreco'):
            conddb.addFolder("PIXEL_OFL","/PIXEL/PixReco")
        ToolSvc.PixelRecoDbTool.InputSource = 2

    if not athenaCommonFlags.isOnline():
        if not conddb.folderRequested('/PIXEL/PixdEdx'):
            if (globalflags.DataSource() == 'data'):
                conddb.addFolder("PIXEL_OFL","/PIXEL/PixdEdx")
            else:
                conddb.addFolder("PIXEL_OFL","/PIXEL/PixdEdx")

    from PixelConditionsServices.PixelConditionsServicesConf import PixelOfflineCalibSvc
    InDetPixelOfflineCalibSvc = PixelOfflineCalibSvc( PixelRecoDbTool = ToolSvc.PixelRecoDbTool)
    ServiceMgr += InDetPixelOfflineCalibSvc
    if (InDetFlags.doPrintConfigurables()):
        print InDetPixelOfflineCalibSvc

    # Register and printout configuration of  PixelConditionsSummarySvc
    ServiceMgr += InDetPixelConditionsSummarySvc
    if (InDetFlags.doPrintConfigurables()):
        print InDetPixelConditionsSummarySvc
    
#
# --- Load SCT Conditions Services
#
if DetFlags.haveRIO.SCT_on():
    SCTConfigurationFolderPath='/SCT/DAQ/Config/'
    #if its COMP200, use old folders...
    if (conddb.dbdata == "COMP200"):
        SCTConfigurationFolderPath='/SCT/DAQ/Configuration/'
    #...but now check if we want to override that decision with explicit flag (if there is one)
    try:
        if InDetFlags.ForceCoraCool():
            SCTConfigurationFolderPath='/SCT/DAQ/Configuration/'
    except:
        pass
    
    try:
        if InDetFlags.ForceCoolVectorPayload():
            SCTConfigurationFolderPath='/SCT/DAQ/Config/'
    except:
        pass
        
    try:
        if (InDetFlags.ForceCoolVectorPayload() and InDetFlags.ForceCoraCool()):
            print '*** SCT DB CONFIGURATION FLAG CONFLICT: Both CVP and CoraCool selected****'
            SCTConfigurationFolderPath=''
    except:
        pass
    # Load folders that have to exist for both MC and Data
    SCTChipConfigurationPath=SCTConfigurationFolderPath+'Chip'
    SCTModuleConfigurationPath=SCTConfigurationFolderPath+'Module'
    SCTMurConfigurationPath=SCTConfigurationFolderPath+'MUR'
    if not conddb.folderRequested(SCTChipConfigurationPath):
        conddb.addFolderSplitMC("SCT",SCTChipConfigurationPath,SCTChipConfigurationPath)
    if not conddb.folderRequested(SCTModuleConfigurationPath):
        conddb.addFolderSplitMC("SCT",SCTModuleConfigurationPath,SCTModuleConfigurationPath)

    if not conddb.folderRequested(SCTMurConfigurationPath):
        conddb.addFolderSplitMC("SCT",SCTMurConfigurationPath,SCTMurConfigurationPath)

    if not conddb.folderRequested('/SCT/DAQ/Calibration/NPtGainDefects'):
        conddb.addFolderSplitMC("SCT","/SCT/DAQ/Calibration/NPtGainDefects","/SCT/DAQ/Calibration/NPtGainDefects")
    if not conddb.folderRequested('/SCT/DAQ/Calibration/NoiseOccupancyDefects'):
        conddb.addFolderSplitMC("SCT","/SCT/DAQ/Calibration/NoiseOccupancyDefects","/SCT/DAQ/Calibration/NoiseOccupancyDefects")

    if not athenaCommonFlags.isOnline():
        if not conddb.folderRequested('/SCT/Derived/Monitoring'):
            conddb.addFolder("SCT_OFL","/SCT/Derived/Monitoring")
    
    
    # Load conditions summary service
    from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_ConditionsSummarySvc
    InDetSCT_ConditionsSummarySvc = SCT_ConditionsSummarySvc(name = "InDetSCT_ConditionsSummarySvc")
    ServiceMgr += InDetSCT_ConditionsSummarySvc
    if (InDetFlags.doPrintConfigurables()):
        print InDetSCT_ConditionsSummarySvc
    
    # Load conditions configuration service
    from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_ConfigurationConditionsSvc
    InDetSCT_ConfigurationConditionsSvc = SCT_ConfigurationConditionsSvc(name = "InDetSCT_ConfigurationConditionsSvc")
    ServiceMgr += InDetSCT_ConfigurationConditionsSvc
    if (InDetFlags.doPrintConfigurables()):
        print InDetSCT_ConfigurationConditionsSvc

    # Load calibration conditions service
    from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_ReadCalibDataSvc
    InDetSCT_ReadCalibDataSvc = SCT_ReadCalibDataSvc(name = "InDetSCT_ReadCalibDataSvc")
    ServiceMgr += InDetSCT_ReadCalibDataSvc
    if (InDetFlags.doPrintConfigurables()):
        print InDetSCT_ReadCalibDataSvc
    
    # Load flagged condition service
    from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_FlaggedConditionSvc
    InDetSCT_FlaggedConditionSvc = SCT_FlaggedConditionSvc(name = "InDetSCT_FlaggedConditionSvc")
    ServiceMgr += InDetSCT_FlaggedConditionSvc
    if (InDetFlags.doPrintConfigurables()):
        print InDetSCT_FlaggedConditionSvc
    
    # Load conditions Monitoring service
    if not athenaCommonFlags.isOnline():
        from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_MonitorConditionsSvc
        InDetSCT_MonitorConditionsSvc = SCT_MonitorConditionsSvc(name          = "InDetSCT_MonitorConditionsSvc",
                                                                 OutputLevel   = INFO,
                                                                 WriteCondObjs = False,
                                                                 RegisterIOV   = False,
                                                                 ReadWriteCool = True)
        ServiceMgr += InDetSCT_MonitorConditionsSvc
        if (InDetFlags.doPrintConfigurables()):
            print InDetSCT_MonitorConditionsSvc

    if InDetFlags.doSCTModuleVeto():
      from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_ModuleVetoSvc
      InDetSCT_ModuleVetoSvc = SCT_ModuleVetoSvc(name = "InDetSCT_ModuleVetoSvc")
      ServiceMgr += InDetSCT_ModuleVetoSvc
      if (InDetFlags.doPrintConfigurables()):
        print InDetSCT_ModuleVetoSvc

    # Load bytestream errors service
    from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_ByteStreamErrorsSvc
    InDetSCT_ByteStreamErrorsSvc = SCT_ByteStreamErrorsSvc(name = "InDetSCT_ByteStreamErrorsSvc")
    ServiceMgr += InDetSCT_ByteStreamErrorsSvc
    if (InDetFlags.doPrintConfigurables()):
        print InDetSCT_ByteStreamErrorsSvc
    
    if InDetFlags.useSctDCS():
        if not conddb.folderRequested('/SCT/DCS/CHANSTAT'):
            conddb.addFolder("DCS_OFL","/SCT/DCS/CHANSTAT")
        if not conddb.folderRequested('/SCT/DCS/MODTEMP'):
            conddb.addFolder("DCS_OFL","/SCT/DCS/MODTEMP")
        if not conddb.folderRequested('/SCT/DCS/HV'):
            conddb.addFolder("DCS_OFL","/SCT/DCS/HV")
               
        from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_DCSConditionsSvc
        InDetSCT_DCSConditionsSvc = SCT_DCSConditionsSvc(name = "InDetSCT_DCSConditionsSvc")        
        if InDetFlags.useHVForSctDCS(): InDetSCT_DCSConditionsSvc.UseDefaultHV = True  #Hack to use ~20V cut for SCT DCS rather than ChanStat for startup
        ServiceMgr += InDetSCT_DCSConditionsSvc
        if (InDetFlags.doPrintConfigurables()):
            print InDetSCT_DCSConditionsSvc
    print "Conditions db instance is ", conddb.dbdata
    # Load Tdaq enabled services for data only and add some to summary svc for data only
    tdaqFolder = '/TDAQ/EnabledResources/ATLAS/SCT/Robins'
    if (conddb.dbdata == "CONDBR2"):
      tdaqFolder = '/TDAQ/Resources/ATLAS/SCT/Robins'
    
    if (globalflags.DataSource() == 'data'):       
        # Load TdaqEnabled service
        if not conddb.folderRequested(tdaqFolder):
            conddb.addFolder("TDAQ",tdaqFolder)
            #conddb.addFolder("","<db>COOLONL_TDAQ/COMP200</db> /TDAQ/EnabledResources/ATLAS/SCT/Robins")

        from SCT_ConditionsServices.SCT_ConditionsServicesConf import SCT_TdaqEnabledSvc
        InDetSCT_TdaqEnabledSvc = SCT_TdaqEnabledSvc(name = "InDetSCT_TdaqEnabledSvc")

        ServiceMgr += InDetSCT_TdaqEnabledSvc
        if (InDetFlags.doPrintConfigurables()):
            print InDetSCT_TdaqEnabledSvc
        
        # Configure summary service
        InDetSCT_ConditionsSummarySvc.ConditionsServices= [ "InDetSCT_ConfigurationConditionsSvc",
                                                            "InDetSCT_FlaggedConditionSvc",
                                                            "InDetSCT_ByteStreamErrorsSvc",
                                                            "InDetSCT_ReadCalibDataSvc",
                                                            "InDetSCT_TdaqEnabledSvc"]
        if not athenaCommonFlags.isOnline():
            InDetSCT_ConditionsSummarySvc.ConditionsServices += [ "InDetSCT_MonitorConditionsSvc" ]

        if InDetFlags.useSctDCS():
            InDetSCT_ConditionsSummarySvc.ConditionsServices += ["InDetSCT_DCSConditionsSvc"]
       
    # switch conditions off for SLHC usage
    elif InDetFlags.doSLHC():
        InDetSCT_ConditionsSummarySvc.ConditionsServices= []
      
    else :
        InDetSCT_ConditionsSummarySvc.ConditionsServices= [ "InDetSCT_ConfigurationConditionsSvc",
                                                            "InDetSCT_FlaggedConditionSvc",
                                                            "InDetSCT_MonitorConditionsSvc",
                                                            "InDetSCT_ByteStreamErrorsSvc",
                                                            "InDetSCT_ReadCalibDataSvc"]


    if InDetFlags.doSCTModuleVeto():
      InDetSCT_ConditionsSummarySvc.ConditionsServices += ["InDetSCT_ModuleVetoSvc"]
        
    
    if (InDetFlags.doPrintConfigurables()):
        print InDetSCT_ConditionsSummarySvc
        
    # Setup Lorentz angle service.
    from SiLorentzAngleSvc.SCTLorentzAngleSvcSetup import sctLorentzAngleSvcSetup
    
    if InDetFlags.useSctDCS():
        # Force Lorentz angle calculation to use DCS for data
        # (Not actually using DCS yet but rather temperature and voltage from joboptions.)
        if (globalflags.DataSource() == 'data'):            
            sctLorentzAngleSvcSetup.forceUseDB()
    else:
        sctLorentzAngleSvcSetup.forceUseGeoModel()
            
#
# --- Load necessary TRT conditions folders
#
if DetFlags.haveRIO.TRT_on():
    # Compression table
    if (globalflags.DataSource() == 'data'): 
        if not conddb.folderRequested('/TRT/Onl/ROD/Compress'):
            conddb.addFolder("TRT_ONL","/TRT/Onl/ROD/Compress")
    # Calibration constants
    if not conddb.folderRequested('/TRT/Calib/RT'):
        conddb.addFolderSplitOnline("TRT","/TRT/Onl/Calib/RT","/TRT/Calib/RT")
    if not conddb.folderRequested('/TRT/Calib/T0'):
        conddb.addFolderSplitOnline("TRT","/TRT/Onl/Calib/T0","/TRT/Calib/T0")

    # --- reenambe new TRT errors      
    if not conddb.folderRequested('/TRT/Calib/errors2d'):
        conddb.addFolderSplitOnline ("TRT","/TRT/Onl/Calib/errors2d","/TRT/Calib/errors2d")

    if not conddb.folderRequested('/TRT/Calib/slopes'):
        conddb.addFolderSplitOnline ("TRT","/TRT/Onl/Calib/slopes","/TRT/Calib/slopes")
        
    if not conddb.folderRequested('/TRT/Calib/ToTCalib'):
        conddb.addFolderSplitOnline("TRT","/TRT/Onl/Calib/ToTCalib","/TRT/Calib/ToTCalib")
#        conddb.addFolder("TRT_OFL","/TRT/Calib/ToTCalib")

    if not conddb.folderRequested('/TRT/Calib/HTCalib'):
      conddb.addFolderSplitOnline("TRT","/TRT/Onl/Calib/HTCalib","/TRT/Calib/HTCalib")

    # Calibration DB Service
    from TRT_ConditionsServices.TRT_ConditionsServicesConf import TRT_CalDbSvc
    InDetTRTCalDbSvc = TRT_CalDbSvc()
    ServiceMgr += InDetTRTCalDbSvc
    if(InDetFlags.doPrintConfigurables()):
        print InDetTRTCalDbSvc
    
    # Dead/Noisy Straw Lists
    if not conddb.folderRequested('/TRT/Cond/Status'):
        conddb.addFolderSplitOnline("TRT","/TRT/Onl/Cond/Status","/TRT/Cond/Status")
    if not conddb.folderRequested('/TRT/Cond/StatusPermanent'):
        conddb.addFolderSplitOnline("TRT","/TRT/Onl/Cond/StatusPermanent","/TRT/Cond/StatusPermanent")
    # Argon straw list
    if not conddb.folderRequested('/TRT/Cond/StatusHT'):
       conddb.addFolderSplitOnline("TRT","/TRT/Onl/Cond/StatusHT","/TRT/Cond/StatusHT")

    # DCS Data Folders
    if (globalflags.InputFormat() == 'bytestream' and globalflags.DataSource() == 'data'):
        if InDetFlags.useTrtDCS():
            conddb.addFolder('DCS_OFL',"/TRT/DCS/HV/BARREL <cache>600</cache>")
            conddb.addFolder('DCS_OFL',"/TRT/DCS/HV/ENDCAPA <cache>600</cache>")
            conddb.addFolder('DCS_OFL',"/TRT/DCS/HV/ENDCAPC <cache>600</cache>")

    # TRT PID tools        
    if not conddb.folderRequested( "/TRT/Calib/PID" ):
        conddb.addFolderSplitOnline("TRT","/TRT/Onl/Calib/PID","/TRT/Calib/PID")
    if not conddb.folderRequested( "/TRT/Calib/PID_RToT" ):
        conddb.addFolderSplitOnline("TRT","/TRT/Onl/Calib/PID_RToT","/TRT/Calib/PID_RToT")
    # TRT PID tools        
    if not conddb.folderRequested( "/TRT/Calib/PIDver_New" ):
        conddb.addFolderSplitOnline("TRT","/TRT/Onl/Calib/PIDver_New","/TRT/Calib/PIDver_New")
    if not conddb.folderRequested( "/TRT/Calib/PID_RToTver_New" ):
        conddb.addFolderSplitOnline("TRT","/TRT/Onl/Calib/PID_RToTver_New","/TRT/Calib/PID_RToTver_New")

		# Added for run2. Clean the unsed ones!!!
    if not conddb.folderRequested( "/TRT/Calib/PID_vector" ):
        conddb.addFolderSplitOnline("TRT","/TRT/Onl/Calib/PID_vector", "/TRT/Calib/PID_vector")

    if not conddb.folderRequested( "/TRT/Calib/ToT/ToTVectors"):
       conddb.addFolderSplitOnline( "TRT", "/TRT/Onl/Calib/ToT/ToTVectors", "/TRT/Calib/ToT/ToTVectors")

    if not conddb.folderRequested( "/TRT/Calib/ToT/ToTValue"):
       conddb.addFolderSplitOnline( "TRT", "/TRT/Onl/Calib/ToT/ToTValue", "/TRT/Calib/ToT/ToTValue")


    #
    # now do the services
    #
    InDetTRT_DAQ_ConditionsSvc = None
    if (InDetFlags.doMonitoringTRT() and globalflags.DataSource() == 'data'):
        tdaqFolder = '/TDAQ/EnabledResources/ATLAS/TRT/Robins'
        if (conddb.dbdata == "CONDBR2"):
            tdaqFolder = '/TDAQ/Resources/ATLAS/TRT/Robins'
        # TDAQ Enabled Service (only for monitoring on data)
        conddb.addFolder('TDAQ_ONL',tdaqFolder)
        from TRT_ConditionsServices.TRT_ConditionsServicesConf import TRT_DAQ_ConditionsSvc
        InDetTRT_DAQ_ConditionsSvc = TRT_DAQ_ConditionsSvc( name = "InDetTRT_DAQ_ConditionsSvc" )
        ServiceMgr += InDetTRT_DAQ_ConditionsSvc
        if (InDetFlags.doPrintConfigurables()):
            print InDetTRT_DAQ_ConditionsSvc
    
    #
    # Load and Configure TRT Conditions Services
    #
    InDetTRTConditionsServices=[]

    # Dead/Noisy Straw Service
    from TRT_ConditionsServices.TRT_ConditionsServicesConf import TRT_StrawStatusSummarySvc
    InDetTRTStrawStatusSummarySvc = TRT_StrawStatusSummarySvc(name = "InDetTRTStrawStatusSummarySvc")
    ServiceMgr += InDetTRTStrawStatusSummarySvc
    if (InDetFlags.doPrintConfigurables()):
        print InDetTRTStrawStatusSummarySvc
    InDetTRTConditionsServices.append(InDetTRTStrawStatusSummarySvc)
    
    # Services which only run on raw data
    if (globalflags.InputFormat() == 'bytestream' and globalflags.DataSource() == 'data'):
                    
        # DCS Conditions Service
        if InDetFlags.useTrtDCS():

            # Hardware Mapping Service
            from TRT_ConditionsServices.TRT_ConditionsServicesConf import TRT_HWMappingSvc
            InDetTRT_HWMappingSvc = TRT_HWMappingSvc(name="InDetTRT_HWMappingSvc")
            ServiceMgr += InDetTRT_HWMappingSvc
            if (InDetFlags.doPrintConfigurables()):
                print InDetTRT_HWMappingSvc

            from TRT_ConditionsServices.TRT_ConditionsServicesConf import TRT_DCS_ConditionsSvc
            InDetTRT_DCS_ConditionsSvc = TRT_DCS_ConditionsSvc(name          = "InDetTRT_DCS_ConditionsSvc",
                                                               HWMapSvc      = InDetTRT_HWMappingSvc,
                                                               #OutputLevel  = VERBOSE,
                                                               EventInfoKey  = "ByteStreamEventInfo",
                                                               DoIOVChecking = False,
                                                               IOVmaxLength  = 7*24*60*60,
                                                               #FallBackOnCOOLChanName = False,
                                                               )
            ServiceMgr += InDetTRT_DCS_ConditionsSvc
            if (InDetFlags.doPrintConfigurables()):
                print InDetTRT_DCS_ConditionsSvc
            #InDetTRTConditionsServices.append(InDetTRT_DCS_ConditionsSvc)
    
    # TRT Conditions Summary Service
    from TRT_ConditionsServices.TRT_ConditionsServicesConf import TRT_ConditionsSummarySvc
    InDetTRTConditionsSummaryService = TRT_ConditionsSummarySvc(name        = "InDetTRTConditionsSummaryService",
                                                                ServiceList = InDetTRTConditionsServices)
    ServiceMgr += InDetTRTConditionsSummaryService
    if (InDetFlags.doPrintConfigurables()):
        print InDetTRTConditionsSummaryService 

    from TRT_RecoConditionsServices.TRT_RecoConditionsServicesConf import TRT_ActiveFractionSvc
    InDetTRT_ActiveFractionSvc = TRT_ActiveFractionSvc(name = "InDetTRTActiveFractionSvc")

    ServiceMgr += InDetTRT_ActiveFractionSvc
    if (InDetFlags.doPrintConfigurables()):
        print InDetTRT_ActiveFractionSvc
