
# block include of file, this is used by many packages
include.block ("InDetRecExample/InDetRecConditionsAccess.py")

from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.DetFlags import DetFlags

isData = (globalflags.DataSource == 'data')

if not ('conddb' in dir()):
    IOVDbSvc = Service("IOVDbSvc")
    from IOVDbSvc.CondDB import conddb

#
# --- Setup BeamSpot data
#
try:
   from RecExConfig.RecFlags import rec
   # If express processing, point beam spot to online folder results
   if (rec.doExpressProcessing()):
        conddb.addFolder('INDET_ONL', '/Indet/Onl/Beampos <key>/Indet/Beampos</key>', className="AthenaAttributeList")
   else:
        conddb.addFolderSplitOnline("INDET", "/Indet/Onl/Beampos", "/Indet/Beampos", className="AthenaAttributeList")

except ImportError:
    # Protection for AthSimulationBase release which does not contain RecExConfig
    conddb.addFolderSplitOnline("INDET", "/Indet/Onl/Beampos", "/Indet/Beampos", className="AthenaAttributeList")

# Conditions sequence for Athena MT
from AthenaCommon.AlgSequence import AthSequencer
condSeq = AthSequencer("AthCondSeq")
if not hasattr(condSeq, "BeamSpotCondAlg"):
   from BeamSpotConditions.BeamSpotConditionsConf import BeamSpotCondAlg
   condSeq += BeamSpotCondAlg( "BeamSpotCondAlg" )


#
# --- Load PixelConditionsServices
#
if DetFlags.haveRIO.pixel_on():
    # Load pixel conditions summary service
    from AthenaCommon.AppMgr import ToolSvc
    if not hasattr(ToolSvc, "PixelConditionsSummaryTool"):
        from PixelConditionsTools.PixelConditionsSummaryToolSetup import PixelConditionsSummaryToolSetup
        pixelConditionsSummaryToolSetup = PixelConditionsSummaryToolSetup()
        pixelConditionsSummaryToolSetup.setUseConditions(True)
        pixelConditionsSummaryToolSetup.setUseDCSState(isData  and InDetFlags.usePixelDCS())
        pixelConditionsSummaryToolSetup.setUseByteStream(isData)
        pixelConditionsSummaryToolSetup.setUseTDAQ(False)
        pixelConditionsSummaryToolSetup.setUseDeadMap((not athenaCommonFlags.isOnline()))
        pixelConditionsSummaryToolSetup.setup()

    InDetPixelConditionsSummaryTool = ToolSvc.PixelConditionsSummaryTool

    if InDetFlags.usePixelDCS():
        InDetPixelConditionsSummaryTool.IsActiveStates = [ 'READY', 'ON', 'UNKNOWN', 'TRANSITION', 'UNDEFINED' ]
        InDetPixelConditionsSummaryTool.IsActiveStatus = [ 'OK', 'WARNING', 'ERROR', 'FATAL' ]

    if (InDetFlags.doPrintConfigurables()):
        print InDetPixelConditionsSummaryTool

 
    # Load pixel calibration service
    if not athenaCommonFlags.isOnline():
        if not conddb.folderRequested('/PIXEL/PixCalib'):
            conddb.addFolder("PIXEL_OFL","/PIXEL/PixCalib")
        from PixelConditionsServices.PixelConditionsServicesConf import PixelCalibSvc
        InDetPixelCalibSvc = PixelCalibSvc()
        ServiceMgr += InDetPixelCalibSvc
        if InDetFlags.doPrintConfigurables():
            print InDetPixelCalibSvc

    # Load Pixel BS errors service
    if not (globalflags.DataSource=='geant4'):
        from PixelConditionsServices.PixelConditionsServicesConf import PixelByteStreamErrorsSvc
        InDetPixelByteStreamErrorsSvc = PixelByteStreamErrorsSvc()
        if ( globalflags.InputFormat != 'bytestream' ):
            InDetPixelByteStreamErrorsSvc.ReadingESD = True
        ServiceMgr += InDetPixelByteStreamErrorsSvc
        if (InDetFlags.doPrintConfigurables()):
            print InDetPixelByteStreamErrorsSvc
            
    if not athenaCommonFlags.isOnline():
        if not conddb.folderRequested('/PIXEL/PixdEdx'):
            if (globalflags.DataSource() == 'data'):
                conddb.addFolder("PIXEL_OFL","/PIXEL/PixdEdx")
            else:
                conddb.addFolder("PIXEL_OFL","/PIXEL/PixdEdx")
                
        if not conddb.folderRequested("/PIXEL/PixReco"):
            conddb.addFolder("PIXEL_OFL", "/PIXEL/PixReco", className="DetCondCFloat")


    if not hasattr(condSeq, 'PixelOfflineCalibCondAlg'):
        from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelOfflineCalibCondAlg
        condSeq += PixelOfflineCalibCondAlg(name="PixelOfflineCalibCondAlg", ReadKey="/PIXEL/PixReco")
        if athenaCommonFlags.isOnline() :
          PixelOfflineCalibCondAlg.InputSource = 1
        else :
          PixelOfflineCalibCondAlg.InputSource = 2

    if not hasattr(condSeq, 'PixelConfigCondAlg'):
      from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelConfigCondAlg
      condSeq += PixelConfigCondAlg(name="PixelConfigCondAlg")

    if not hasattr(ToolSvc, "PixelLorentzAngleTool"):
        from SiLorentzAngleTool.PixelLorentzAngleToolSetup import PixelLorentzAngleToolSetup
        pixelLorentzAngleToolSetup = PixelLorentzAngleToolSetup()


#
# --- Load SCT Conditions Services
#
if DetFlags.haveRIO.SCT_on():
    # Set up SCT cabling
    include( 'InDetRecExample/InDetRecCabling.py' )

    # Load conditions summary tool
    from SCT_ConditionsTools.SCT_ConditionsSummaryToolSetup import SCT_ConditionsSummaryToolSetup
    sct_ConditionsSummaryToolSetup = SCT_ConditionsSummaryToolSetup()
    sct_ConditionsSummaryToolSetup.setup()
    InDetSCT_ConditionsSummaryTool = sct_ConditionsSummaryToolSetup.getTool()
    if (InDetFlags.doPrintConfigurables()):
        print InDetSCT_ConditionsSummaryTool
    
    # Load conditions configuration service and load folders and algorithm for it
    # Load folders that have to exist for both MC and Data
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
    from SCT_ConditionsTools.SCT_ConfigurationConditionsToolSetup import SCT_ConfigurationConditionsToolSetup
    sct_ConfigurationConditionsToolSetup = SCT_ConfigurationConditionsToolSetup()
    sct_ConfigurationConditionsToolSetup.setChannelFolder(SCTConfigurationFolderPath+"Chip")
    sct_ConfigurationConditionsToolSetup.setModuleFolder(SCTConfigurationFolderPath+"Module")
    sct_ConfigurationConditionsToolSetup.setMurFolder(SCTConfigurationFolderPath+"MUR")
    sct_ConfigurationConditionsToolSetup.setup()
    InDetSCT_ConfigurationConditionsTool = sct_ConfigurationConditionsToolSetup.getTool()
    if (InDetFlags.doPrintConfigurables()):
        print InDetSCT_ConfigurationConditionsTool

    # Load calibration conditions tool
    from SCT_ConditionsTools.SCT_ReadCalibDataToolSetup import SCT_ReadCalibDataToolSetup
    sct_ReadCalibDataToolSetup = SCT_ReadCalibDataToolSetup()
    sct_ReadCalibDataToolSetup.setup()
    InDetSCT_ReadCalibDataTool = sct_ReadCalibDataToolSetup.getTool()
    if (InDetFlags.doPrintConfigurables()):
        print InDetSCT_ReadCalibDataTool
    
    # Load flagged condition tool
    from SCT_ConditionsTools.SCT_FlaggedConditionToolSetup import SCT_FlaggedConditionToolSetup
    sct_FlaggedConditionToolSetup = SCT_FlaggedConditionToolSetup()
    sct_FlaggedConditionToolSetup.setup()
    InDetSCT_FlaggedConditionTool = sct_FlaggedConditionToolSetup.getTool()
    if (InDetFlags.doPrintConfigurables()):
        print InDetSCT_FlaggedConditionTool
    
    # Load conditions Monitoring tool
    if not athenaCommonFlags.isOnline():
        from SCT_ConditionsTools.SCT_MonitorConditionsToolSetup import SCT_MonitorConditionsToolSetup
        sct_MonitorConditionsToolSetup = SCT_MonitorConditionsToolSetup()
        sct_MonitorConditionsToolSetup.setOutputLevel(INFO)
        sct_MonitorConditionsToolSetup.setup()
        InDetSCT_MonitorConditionsTool = sct_MonitorConditionsToolSetup.getTool()
        if (InDetFlags.doPrintConfigurables()):
            print InDetSCT_MonitorConditionsTool

    if InDetFlags.doSCTModuleVeto():
        from SCT_ConditionsTools.SCT_ModuleVetoToolSetup import SCT_ModuleVetoToolSetup
        sct_ModuleVetoToolSetup = SCT_ModuleVetoToolSetup()
        sct_ModuleVetoToolSetup.setup()
        InDetSCT_ModuleVetoTool = sct_ModuleVetoToolSetup.getTool()
        if (InDetFlags.doPrintConfigurables()):
            print InDetSCT_ModuleVetoTool

    # Load bytestream errors tool (use default instance without "InDet")
    # @TODO find a better to solution to get the correct tool for the current job.
    from SCT_ConditionsTools.SCT_ByteStreamErrorsToolSetup import SCT_ByteStreamErrorsToolSetup
    sct_ByteStreamErrorsToolSetup = SCT_ByteStreamErrorsToolSetup()
    sct_ByteStreamErrorsToolSetup.setConfigTool(InDetSCT_ConfigurationConditionsTool)
    sct_ByteStreamErrorsToolSetup.setup()
    if (InDetFlags.doPrintConfigurables()):
        print sct_ByteStreamErrorsToolSetup.getTool()
    
    if InDetFlags.useSctDCS():
        from SCT_ConditionsTools.SCT_DCSConditionsToolSetup import SCT_DCSConditionsToolSetup
        sct_DCSConditionsToolSetup = SCT_DCSConditionsToolSetup()
        sct_DCSConditionsToolSetup.setup()
        InDetSCT_DCSConditionsTool = sct_DCSConditionsToolSetup.getTool()
        if InDetFlags.useHVForSctDCS():
            sct_DCSConditionsToolSetup.getStateAlg().UseDefaultHV = True  #Hack to use ~20V cut for SCT DCS rather than ChanStat for startup
        if (InDetFlags.doPrintConfigurables()):
            print InDetSCT_DCSConditionsTool
    
    if (globalflags.DataSource() == 'data'):       
        print "Conditions db instance is ", conddb.dbdata
        # Load Tdaq enabled tools for data only and add some to summary tool for data only
        tdaqFolder = '/TDAQ/EnabledResources/ATLAS/SCT/Robins'
        if (conddb.dbdata == "CONDBR2"):
            tdaqFolder = '/TDAQ/Resources/ATLAS/SCT/Robins'
        # Load TdaqEnabled tool
        from SCT_ConditionsTools.SCT_TdaqEnabledToolSetup import SCT_TdaqEnabledToolSetup
        sct_TdaqEnabledToolSetup = SCT_TdaqEnabledToolSetup()
        sct_TdaqEnabledToolSetup.setFolder(tdaqFolder)
        sct_TdaqEnabledToolSetup.setup()
        InDetSCT_TdaqEnabledTool = sct_TdaqEnabledToolSetup.getTool()
        if (InDetFlags.doPrintConfigurables()):
            print InDetSCT_TdaqEnabledTool
        
        # Configure summary tool
        InDetSCT_ConditionsSummaryTool.ConditionsTools= [ sct_ConfigurationConditionsToolSetup.getTool().getFullName(),
                                                          sct_FlaggedConditionToolSetup.getTool().getFullName(),
                                                          sct_ByteStreamErrorsToolSetup.getTool().getFullName(),
                                                          sct_ReadCalibDataToolSetup.getTool().getFullName(),
                                                          sct_TdaqEnabledToolSetup.getTool().getFullName()]
        if not athenaCommonFlags.isOnline():
            InDetSCT_ConditionsSummaryTool.ConditionsTools += [ sct_MonitorConditionsToolSetup.getTool().getFullName() ]

        if InDetFlags.useSctDCS():
            InDetSCT_ConditionsSummaryTool.ConditionsTools += [ sct_DCSConditionsToolSetup.getTool().getFullName() ]
       
    # switch conditions off for SLHC usage
    elif InDetFlags.doSLHC():
        InDetSCT_ConditionsSummaryTool.ConditionsTools= []
      
    else :
        InDetSCT_ConditionsSummaryTool.ConditionsTools= [ sct_ConfigurationConditionsToolSetup.getTool().getFullName(),
                                                          sct_FlaggedConditionToolSetup.getTool().getFullName(),
                                                          sct_MonitorConditionsToolSetup.getTool().getFullName(),
                                                          sct_ReadCalibDataToolSetup.getTool().getFullName()]


    if InDetFlags.doSCTModuleVeto():
        InDetSCT_ConditionsSummaryTool.ConditionsTools += [ sct_MonitorConditionsToolSetup.getTool().getFullName() ]
        
    
    if (InDetFlags.doPrintConfigurables()):
        print InDetSCT_ConditionsSummaryTool

    # Conditions summary tool without InDetSCT_FlaggedConditionTool
    sct_ConditionsSummaryToolSetupWithoutFlagged = SCT_ConditionsSummaryToolSetup("InDetSCT_ConditionsSummaryToolWithoutFlagged")
    sct_ConditionsSummaryToolSetupWithoutFlagged.setup()
    InDetSCT_ConditionsSummaryToolWithoutFlagged = sct_ConditionsSummaryToolSetupWithoutFlagged.getTool()    
    condTools = []
    for condToolHandle in InDetSCT_ConditionsSummaryTool.ConditionsTools:
        condTool = condToolHandle.typeAndName
        if condTool not in condTools:
            if condTool != InDetSCT_FlaggedConditionTool.getFullName():
                condTools.append(condTool)
    InDetSCT_ConditionsSummaryToolWithoutFlagged.ConditionsTools = condTools
        
    # Setup Lorentz angle tool.
    from SiLorentzAngleTool.SCTLorentzAngleToolSetup import SCTLorentzAngleToolSetup

    forceUseDB = False
    forceUseGeoModel = False
    if InDetFlags.useSctDCS():
        # Force Lorentz angle calculation to use DCS for data
        # (Not actually using DCS yet but rather temperature and voltage from joboptions.)
        if (globalflags.DataSource() == 'data'):
            forceUseDB = True
    else:
        forceUseGeoModel = True

    sctLorentzAngleToolSetup = SCTLorentzAngleToolSetup(forceUseDB=forceUseDB, forceUseGeoModel=forceUseGeoModel)
    SCTLorentzAngleTool = sctLorentzAngleToolSetup.SCTLorentzAngleTool
            
#
# --- Load necessary TRT conditions folders
#
if DetFlags.haveRIO.TRT_on():
    # Compression table
    if (globalflags.DataSource() == 'data'): 
        if not conddb.folderRequested('/TRT/Onl/ROD/Compress'):
            conddb.addFolder("TRT_ONL","/TRT/Onl/ROD/Compress",className='CondAttrListCollection')

    # Calibration constants
    # Block folders if they are to be read from or written to text files
    #conddb.blockFolder("/TRT/Calib/RT")
    #conddb.blockFolder("/TRT/Calib/T0")

    if not conddb.folderRequested('/TRT/Calib/RT'):
        conddb.addFolderSplitOnline("TRT","/TRT/Onl/Calib/RT","/TRT/Calib/RT",className='TRTCond::RtRelationMultChanContainer')

    if not conddb.folderRequested('/TRT/Calib/T0'):
        conddb.addFolderSplitOnline("TRT","/TRT/Onl/Calib/T0","/TRT/Calib/T0",className='TRTCond::StrawT0MultChanContainer')

    if not conddb.folderRequested('/TRT/Calib/errors2d'):
        TRTErrorsFolder = conddb.addFolderSplitOnline ("TRT","/TRT/Onl/Calib/errors2d","/TRT/Calib/errors2d",className='TRTCond::RtRelationMultChanContainer')

    if not conddb.folderRequested('/TRT/Calib/slopes'):
        TRTSlopesFolder = conddb.addFolderSplitOnline ("TRT","/TRT/Onl/Calib/slopes","/TRT/Calib/slopes",className='TRTCond::RtRelationMultChanContainer')

    if not conddb.folderRequested('/TRT/Calib/ToTCalib'):
        conddb.addFolderSplitOnline("TRT","/TRT/Onl/Calib/ToTCalib","/TRT/Calib/ToTCalib",className='CondAttrListCollection')

    if not conddb.folderRequested('/TRT/Calib/HTCalib'):
      conddb.addFolderSplitOnline("TRT","/TRT/Onl/Calib/HTCalib","/TRT/Calib/HTCalib",className='CondAttrListCollection')


    # Calibration DB Service
    from TRT_ConditionsServices.TRT_ConditionsServicesConf import TRT_CalDbSvc
    InDetTRTCalDbSvc = TRT_CalDbSvc()
    ServiceMgr += InDetTRTCalDbSvc
    if(InDetFlags.doPrintConfigurables()):
        print InDetTRTCalDbSvc


    # Dead/Noisy Straw Lists
    if not conddb.folderRequested('/TRT/Cond/Status'):
        conddb.addFolderSplitOnline("TRT","/TRT/Onl/Cond/Status","/TRT/Cond/Status",className='TRTCond::StrawStatusMultChanContainer')

    if not conddb.folderRequested('/TRT/Cond/StatusPermanent'):
        conddb.addFolderSplitOnline("TRT","/TRT/Onl/Cond/StatusPermanent","/TRT/Cond/StatusPermanent",className='TRTCond::StrawStatusMultChanContainer')

    # Argon straw list
    if not conddb.folderRequested('/TRT/Cond/StatusHT'):
        conddb.addFolderSplitOnline("TRT","/TRT/Onl/Cond/StatusHT","/TRT/Cond/StatusHT",className='TRTCond::StrawStatusMultChanContainer')

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
        conddb.addFolderSplitOnline("TRT","/TRT/Onl/Calib/PID_vector", "/TRT/Calib/PID_vector",className='CondAttrListVec')
    if not conddb.folderRequested( "/TRT/Calib/ToT/ToTVectors"):
       conddb.addFolderSplitOnline( "TRT", "/TRT/Onl/Calib/ToT/ToTVectors", "/TRT/Calib/ToT/ToTVectors",className='CondAttrListVec')

    if not conddb.folderRequested( "/TRT/Calib/ToT/ToTValue"):
       conddb.addFolderSplitOnline( "TRT", "/TRT/Onl/Calib/ToT/ToTValue", "/TRT/Calib/ToT/ToTValue",className='CondAttrListCollection')


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
    useOldStyle = False
    if DetFlags.simulate.any_on() or athenaCommonFlags.EvtMax==1:
         useOldStyle = True
    from TRT_ConditionsServices.TRT_ConditionsServicesConf import TRT_StrawStatusSummarySvc
    InDetTRTStrawStatusSummarySvc = TRT_StrawStatusSummarySvc(name = "InDetTRTStrawStatusSummarySvc",
                                                              isGEANT4 = useOldStyle)
    ServiceMgr += InDetTRTStrawStatusSummarySvc
    if (InDetFlags.doPrintConfigurables()):
        print InDetTRTStrawStatusSummarySvc
    InDetTRTConditionsServices.append(InDetTRTStrawStatusSummarySvc)

    # Straw status tool
    from TRT_ConditionsServices.TRT_ConditionsServicesConf import TRT_StrawStatusSummaryTool
    InDetTRTStrawStatusSummaryTool = TRT_StrawStatusSummaryTool(name = "TRT_StrawStatusSummaryTool",
                                                            isGEANT4 = useOldStyle)
    # Alive straws algorithm
    from TRT_ConditionsAlgs.TRT_ConditionsAlgsConf import TRTStrawCondAlg
    TRTStrawCondAlg = TRTStrawCondAlg(name = "TRTStrawCondAlg",
                                      TRTStrawStatusSummaryTool = InDetTRTStrawStatusSummaryTool,
                                      isGEANT4 = useOldStyle)
    # Active Fraction algorithm
    from TRT_ConditionsAlgs.TRT_ConditionsAlgsConf import TRTActiveCondAlg
    TRTActiveCondAlg = TRTActiveCondAlg(name = "TRTActiveCondAlg",
                                      TRTStrawStatusSummaryTool = InDetTRTStrawStatusSummaryTool)

    # HT probability algorithm
    from TRT_ConditionsAlgs.TRT_ConditionsAlgsConf import TRTHTCondAlg
    TRTHTCondAlg = TRTHTCondAlg(name = "TRTHTCondAlg")

    # dEdx probability algorithm
    from TRT_ConditionsAlgs.TRT_ConditionsAlgsConf import TRTToTCondAlg
    TRTToTCondAlg = TRTToTCondAlg(name = "TRTToTCondAlg")

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

        
