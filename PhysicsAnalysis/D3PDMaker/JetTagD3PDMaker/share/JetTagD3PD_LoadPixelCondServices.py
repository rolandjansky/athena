

# Load pixel conditions summary service
from PixelConditionsServices.PixelConditionsServicesConf import PixelConditionsSummarySvc
InDetPixelConditionsSummarySvc = PixelConditionsSummarySvc()
  
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
    InDetSpecialPixelMapSvc = SpecialPixelMapSvc(DBFolders  = [ "/PIXEL/PixMapShort", "/PIXEL/PixMapLong" , "/PIXEL/NoiseMapShort", "/PIXEL/NoiseMapLong" ] ,
                                                 SpecialPixelMapKeys = [ "SpecialPixelMap", "SpecialPixelMapLong", "NoiseMapShort", "NoiseMapLong" ] ,
                                                 OverlayFolder       = "/PIXEL/PixMapOverlay",
                                                 OverlayKey          = "PixMapOverlay")
    ServiceMgr += InDetSpecialPixelMapSvc
    if InDetFlags.doPrintConfigurables():
        print InDetSpecialPixelMapSvc
    
    # Load pixel DCS information
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

    InDetPixelConditionsSummarySvc.UseDCS         = True
    InDetPixelConditionsSummarySvc.IsActiveStates = [ 'READY' ]
    InDetPixelConditionsSummarySvc.IsActiveStatus = [ 'OK', 'WARNING' ]


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
            
# Register and printout configuration of  PixelConditionsSummarySvc
ServiceMgr += InDetPixelConditionsSummarySvc
if (InDetFlags.doPrintConfigurables()):
    print InDetPixelConditionsSummarySvc
    
from InDetTestBLayer.InDetTestBLayerConf import InDet__InDetTestBLayerTool
InDetRecTestBLayerTool = InDet__InDetTestBLayerTool(name            = "InDetRecTestBLayerTool",
                                                    PixelSummarySvc = InDetPixelConditionsSummarySvc,
                                                    Extrapolator    = InDetExtrapolator)
ToolSvc += InDetRecTestBLayerTool
if (InDetFlags.doPrintConfigurables()):
    print  InDetRecTestBLayerTool

