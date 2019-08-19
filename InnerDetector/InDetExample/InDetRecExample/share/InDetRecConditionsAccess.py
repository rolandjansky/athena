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
    include ("InDetRecExample/SCTConditionsAccess.py")
            
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
