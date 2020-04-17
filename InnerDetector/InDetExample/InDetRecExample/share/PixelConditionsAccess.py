###  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# block include of file, this is used by many packages
include.block ("InDetRecExample/PixelConditionsAccess.py")

isData = (globalflags.DataSource == 'data')

if not ('conddb' in dir()):
  IOVDbSvc = Service("IOVDbSvc")
  from IOVDbSvc.CondDB import conddb

#
# --- Load Pixel Conditions Services
#

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
