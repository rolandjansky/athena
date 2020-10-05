
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
   # If express processing, point beam spot to online folder results
   from RecExConfig.RecFlags import rec
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
# --- Load PixelConditionsTools
#
if DetFlags.pixel_on():
    # Load pixel conditions summary service
    from AthenaCommon.AppMgr import ToolSvc
    from AtlasGeoModel.CommonGMJobProperties import CommonGeometryFlags as commonGeoFlags
    from AtlasGeoModel.InDetGMJobProperties import InDetGeometryFlags as geoFlags

    #################
    # Module status #
    #################
    useNewDeadmapFormat = False
    useNewChargeFormat  = False

    if not useNewDeadmapFormat:
        if not (conddb.folderRequested("/PIXEL/PixMapOverlay") or conddb.folderRequested("/PIXEL/Onl/PixMapOverlay")):
            conddb.addFolderSplitOnline("PIXEL","/PIXEL/Onl/PixMapOverlay","/PIXEL/PixMapOverlay", className='CondAttrListCollection')

    if not hasattr(condSeq, "PixelConfigCondAlg"):
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
        if athenaCommonFlags.isOnline():
            alg.ReadDeadMapKey = ""
        condSeq += alg

    if useNewDeadmapFormat:
        if not conddb.folderRequested("/PIXEL/PixelModuleFeMask"):
            conddb.addFolder("PIXEL_OFL", "/PIXEL/PixelModuleFeMask", className="CondAttrListCollection")
        if not hasattr(condSeq, "PixelDeadMapCondAlg"):
            from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelDeadMapCondAlg
            alg = PixelDeadMapCondAlg(name="PixelDeadMapCondAlg")
            if athenaCommonFlags.isOnline():
                alg.ReadKey = ""
            condSeq += alg

    if globalflags.DataSource=='data' and InDetFlags.usePixelDCS():
        if not conddb.folderRequested("/PIXEL/DCS/FSMSTATE"):
            conddb.addFolder("DCS_OFL", "/PIXEL/DCS/FSMSTATE", className="CondAttrListCollection")
        if not conddb.folderRequested("/PIXEL/DCS/FSMSTATUS"):
            conddb.addFolder("DCS_OFL", "/PIXEL/DCS/FSMSTATUS", className="CondAttrListCollection")

    if not hasattr(condSeq, "PixelDCSCondStateAlg"):
        from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelDCSCondStateAlg
        alg = PixelDCSCondStateAlg(name="PixelDCSCondStateAlg")
        if athenaCommonFlags.isOnline():
            alg.ReadKeyState = ''
        condSeq += alg

    if not hasattr(condSeq, "PixelDCSCondStatusAlg"):
        from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelDCSCondStatusAlg
        alg = PixelDCSCondStatusAlg(name="PixelDCSCondStatusAlg")
        if athenaCommonFlags.isOnline():
            alg.ReadKeyStatus = ''
        condSeq += alg

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

    if (globalflags.DataSource=='data' and conddb.dbdata == 'CONDBR2'):
        if not conddb.folderRequested("/PIXEL/CablingMap"):
            conddb.addFolderSplitOnline("PIXEL", "/PIXEL/Onl/CablingMap","/PIXEL/CablingMap", className="AthenaAttributeList")

    if not hasattr(condSeq, 'PixelCablingCondAlg'):
        from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelCablingCondAlg
        alg = PixelCablingCondAlg(name="PixelCablingCondAlg",
                                  MappingFile=IdMappingDat,
                                  RodIDForSingleLink40=rodIDForSingleLink40)
        if (not conddb.folderRequested("/PIXEL/CablingMap") and
            not conddb.folderRequested("/PIXEL/Onl/CablingMap")):
            alg.ReadKey = ''
        condSeq += alg

    if not athenaCommonFlags.isOnline():
        if not conddb.folderRequested('/PIXEL/PixdEdx'):
            conddb.addFolder("PIXEL_OFL", "/PIXEL/PixdEdx", className="AthenaAttributeList")

        if not conddb.folderRequested("/PIXEL/PixReco"):
            conddb.addFolder("PIXEL_OFL", "/PIXEL/PixReco", className="DetCondCFloat")

        if not conddb.folderRequested("/Indet/PixelDist"):
            conddb.addFolder("INDET", "/Indet/PixelDist", className="DetCondCFloat")

    if not hasattr(condSeq, 'PixelOfflineCalibCondAlg'):
        from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelOfflineCalibCondAlg
        condSeq += PixelOfflineCalibCondAlg(name="PixelOfflineCalibCondAlg", ReadKey="/PIXEL/PixReco")
        if athenaCommonFlags.isOnline():
          PixelOfflineCalibCondAlg.InputSource = 1
        else :
          PixelOfflineCalibCondAlg.InputSource = 2

    if not hasattr(ToolSvc, "PixelLorentzAngleTool"):
        from SiLorentzAngleTool.PixelLorentzAngleToolSetup import PixelLorentzAngleToolSetup
        pixelLorentzAngleToolSetup = PixelLorentzAngleToolSetup()

    if not hasattr(condSeq, 'PixelDistortionAlg'):
        from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelDistortionAlg
        condSeq += PixelDistortionAlg(name="PixelDistortionAlg")

    if not hasattr(condSeq, 'PixeldEdxAlg'):
        from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixeldEdxAlg
        condSeq += PixeldEdxAlg(name="PixeldEdxAlg")
        if not athenaCommonFlags.isOnline():
            PixeldEdxAlg.ReadFromCOOL = True
        else:
            PixeldEdxAlg.ReadFromCOOL = False
            if (globalflags.DataSource=='data'):
                PixeldEdxAlg.CalibrationFile="dtpar_signed_234.txt"
            else:
                PixeldEdxAlg.CalibrationFile="mcpar_signed_234.txt"

    ################
    # RUN-1 legacy #
    ################
    if commonGeoFlags.Run()=="RUN1" and athenaCommonFlags.isOnline():
        if not conddb.folderRequested("/TDAQ/Resources/ATLAS/PIXEL/Modules"):
            conddb.addFolder("TDAQ_ONL", "/TDAQ/Resources/ATLAS/PIXEL/Modules", className="CondAttrListCollection")
        if not hasattr(condSeq, "PixelTDAQCondAlg"):
            from PixelConditionsAlgorithms.PixelConditionsAlgorithmsConf import PixelTDAQCondAlg
            condSeq += PixelTDAQCondAlg(name="PixelTDAQCondAlg", ReadKey="/TDAQ/Resources/ATLAS/PIXEL/Modules")

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
        printfunc (InDetSCT_ConditionsSummaryTool)
    
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
            printfunc ('*** SCT DB CONFIGURATION FLAG CONFLICT: Both CVP and CoraCool selected****')
            SCTConfigurationFolderPath=''
    except:
        pass
    from SCT_ConditionsTools.SCT_ConfigurationConditionsToolSetup import SCT_ConfigurationConditionsToolSetup
    sct_ConfigurationConditionsToolSetup = SCT_ConfigurationConditionsToolSetup()
    if globalflags.DataSource() == "data":
        sct_ConfigurationConditionsToolSetup.setChannelFolder(SCTConfigurationFolderPath+"Chip")
    else:
        sct_ConfigurationConditionsToolSetup.setChannelFolder(SCTConfigurationFolderPath+"ChipSlim") # For MC (OFLP200)
    sct_ConfigurationConditionsToolSetup.setModuleFolder(SCTConfigurationFolderPath+"Module")
    sct_ConfigurationConditionsToolSetup.setMurFolder(SCTConfigurationFolderPath+"MUR")
    sct_ConfigurationConditionsToolSetup.setup()
    InDetSCT_ConfigurationConditionsTool = sct_ConfigurationConditionsToolSetup.getTool()
    if (InDetFlags.doPrintConfigurables()):
        printfunc (InDetSCT_ConfigurationConditionsTool)

    # Load calibration conditions tool
    from SCT_ConditionsTools.SCT_ReadCalibDataToolSetup import SCT_ReadCalibDataToolSetup
    sct_ReadCalibDataToolSetup = SCT_ReadCalibDataToolSetup()
    sct_ReadCalibDataToolSetup.setup()
    InDetSCT_ReadCalibDataTool = sct_ReadCalibDataToolSetup.getTool()
    if (InDetFlags.doPrintConfigurables()):
        printfunc (InDetSCT_ReadCalibDataTool)

    # Load flagged condition tool
    from SCT_ConditionsTools.SCT_FlaggedConditionToolSetup import SCT_FlaggedConditionToolSetup
    sct_FlaggedConditionToolSetup = SCT_FlaggedConditionToolSetup()
    sct_FlaggedConditionToolSetup.setup()
    InDetSCT_FlaggedConditionTool = sct_FlaggedConditionToolSetup.getTool()
    if (InDetFlags.doPrintConfigurables()):
        printfunc (InDetSCT_FlaggedConditionTool)
    
    # Load conditions Monitoring tool
    if not athenaCommonFlags.isOnline():
        from SCT_ConditionsTools.SCT_MonitorConditionsToolSetup import SCT_MonitorConditionsToolSetup
        sct_MonitorConditionsToolSetup = SCT_MonitorConditionsToolSetup()
        sct_MonitorConditionsToolSetup.setOutputLevel(INFO)
        sct_MonitorConditionsToolSetup.setup()
        InDetSCT_MonitorConditionsTool = sct_MonitorConditionsToolSetup.getTool()
        if (InDetFlags.doPrintConfigurables()):
            printfunc (InDetSCT_MonitorConditionsTool)

    if InDetFlags.doSCTModuleVeto():
        from SCT_ConditionsTools.SCT_ModuleVetoToolSetup import SCT_ModuleVetoToolSetup
        sct_ModuleVetoToolSetup = SCT_ModuleVetoToolSetup()
        sct_ModuleVetoToolSetup.setup()
        InDetSCT_ModuleVetoTool = sct_ModuleVetoToolSetup.getTool()
        if (InDetFlags.doPrintConfigurables()):
            printfunc (InDetSCT_ModuleVetoTool)

    # Load bytestream errors tool (use default instance without "InDet")
    # @TODO find a better to solution to get the correct tool for the current job.
    from SCT_ConditionsTools.SCT_ByteStreamErrorsToolSetup import SCT_ByteStreamErrorsToolSetup
    sct_ByteStreamErrorsToolSetup = SCT_ByteStreamErrorsToolSetup()
    sct_ByteStreamErrorsToolSetup.setConfigTool(InDetSCT_ConfigurationConditionsTool)
    sct_ByteStreamErrorsToolSetup.setup()
    SCT_ByteStreamErrorsTool = sct_ByteStreamErrorsToolSetup.getTool()
    if (InDetFlags.doPrintConfigurables()):
        printfunc (SCT_ByteStreamErrorsTool)
    
    if InDetFlags.useSctDCS():
        from SCT_ConditionsTools.SCT_DCSConditionsToolSetup import SCT_DCSConditionsToolSetup
        sct_DCSConditionsToolSetup = SCT_DCSConditionsToolSetup()

        # For HLT and online monitoring
        if athenaCommonFlags.isOnline():
            sct_DCSConditionsToolSetup.setReadAllDBFolders(False)
            if globalflags.DataSource() == "data":
                sct_DCSConditionsToolSetup.setDbInstance("SCT")
                dcs_folder="/SCT/HLT/DCS"
                sct_DCSConditionsToolSetup.setStateFolder(dcs_folder+"/CHANSTAT")
                sct_DCSConditionsToolSetup.setHVFolder(dcs_folder+"/HV")
                sct_DCSConditionsToolSetup.setTempFolder(dcs_folder+"/MODTEMP")

        sct_DCSConditionsToolSetup.setup()
        InDetSCT_DCSConditionsTool = sct_DCSConditionsToolSetup.getTool()
        if InDetFlags.useHVForSctDCS():
            sct_DCSConditionsToolSetup.getStateAlg().UseDefaultHV = True  #Hack to use ~20V cut for SCT DCS rather than ChanStat for startup
        if (InDetFlags.doPrintConfigurables()):
            printfunc (InDetSCT_DCSConditionsTool)
    
    if (globalflags.DataSource() == 'data'):       
        printfunc ("Conditions db instance is ", conddb.dbdata)
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
            printfunc (InDetSCT_TdaqEnabledTool)
        
        # Configure summary tool
        InDetSCT_ConditionsSummaryTool.ConditionsTools= [ InDetSCT_ConfigurationConditionsTool,
                                                          InDetSCT_FlaggedConditionTool,
                                                          SCT_ByteStreamErrorsTool,
                                                          InDetSCT_ReadCalibDataTool,
                                                          InDetSCT_TdaqEnabledTool ]
        if not athenaCommonFlags.isOnline():
            InDetSCT_ConditionsSummaryTool.ConditionsTools += [ InDetSCT_MonitorConditionsTool ]

        if InDetFlags.useSctDCS():
            InDetSCT_ConditionsSummaryTool.ConditionsTools += [ InDetSCT_DCSConditionsTool ]
       
    # switch conditions off for SLHC usage
    elif InDetFlags.doSLHC():
        InDetSCT_ConditionsSummaryTool.ConditionsTools= []
      
    else :
        InDetSCT_ConditionsSummaryTool.ConditionsTools= [ InDetSCT_ConfigurationConditionsTool,
                                                          InDetSCT_FlaggedConditionTool,
                                                          InDetSCT_MonitorConditionsTool,
                                                          InDetSCT_ReadCalibDataTool ]
        if InDetFlags.useSctDCS():
            InDetSCT_ConditionsSummaryTool.ConditionsTools += [ InDetSCT_DCSConditionsTool ]

    if InDetFlags.doSCTModuleVeto():
        InDetSCT_ConditionsSummaryTool.ConditionsTools += [ InDetSCT_ModuleVetoTool ]

    # @TODO fix this temporary hack to make the configguration of the InDetSCT_ConditionsSummaryTool accessible to TrackingCommon
    import InDetRecExample.TrackingCommon as TrackingCommon
    TrackingCommon.def_InDetSCT_ConditionsSummaryTool=InDetSCT_ConditionsSummaryTool

    if (InDetFlags.doPrintConfigurables()):
        printfunc (InDetSCT_ConditionsSummaryTool)

    # Conditions summary tool without InDetSCT_FlaggedConditionTool
    sct_ConditionsSummaryToolSetupWithoutFlagged = SCT_ConditionsSummaryToolSetup("InDetSCT_ConditionsSummaryToolWithoutFlagged")
    sct_ConditionsSummaryToolSetupWithoutFlagged.setup()
    InDetSCT_ConditionsSummaryToolWithoutFlagged = sct_ConditionsSummaryToolSetupWithoutFlagged.getTool()    
    condTools = []
    for condToolHandle in InDetSCT_ConditionsSummaryTool.ConditionsTools:
        condTool = condToolHandle
        if condTool not in condTools:
            if condTool != InDetSCT_FlaggedConditionTool:
                condTools.append(condTool)
    InDetSCT_ConditionsSummaryToolWithoutFlagged.ConditionsTools = condTools
        
    # Setup Lorentz angle tool.
    from SiLorentzAngleTool.SCTLorentzAngleToolSetup import SCTLorentzAngleToolSetup

    forceUseDB = False
    forceUseGeoModel = False
    if InDetFlags.useSctDCS() or athenaCommonFlags.isOnline():
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


# Rt calibration coinstants
    if not conddb.folderRequested('/TRT/Calib/RT'):
        conddb.addFolderSplitOnline("TRT","/TRT/Onl/Calib/RT","/TRT/Calib/RT",className='TRTCond::RtRelationMultChanContainer')

    if not conddb.folderRequested('/TRT/Calib/T0'):
        conddb.addFolderSplitOnline("TRT","/TRT/Onl/Calib/T0","/TRT/Calib/T0",className='TRTCond::StrawT0MultChanContainer')

    if not conddb.folderRequested('/TRT/Calib/errors2d'):
        TRTErrorsFolder = conddb.addFolderSplitOnline ("TRT","/TRT/Onl/Calib/errors2d","/TRT/Calib/errors2d",className='TRTCond::RtRelationMultChanContainer')

    if not conddb.folderRequested('/TRT/Calib/slopes'):
        TRTSlopesFolder = conddb.addFolderSplitOnline ("TRT","/TRT/Onl/Calib/slopes","/TRT/Calib/slopes",className='TRTCond::RtRelationMultChanContainer')


    # Dead/Noisy Straw Lists
    if not conddb.folderRequested('/TRT/Cond/Status'):
        conddb.addFolderSplitOnline("TRT","/TRT/Onl/Cond/Status","/TRT/Cond/Status",className='TRTCond::StrawStatusMultChanContainer')

    if not conddb.folderRequested('/TRT/Cond/StatusPermanent'):
        conddb.addFolderSplitOnline("TRT","/TRT/Onl/Cond/StatusPermanent","/TRT/Cond/StatusPermanent",className='TRTCond::StrawStatusMultChanContainer')

    # Argon straw list
    if not conddb.folderRequested('/TRT/Cond/StatusHT'):
        conddb.addFolderSplitOnline("TRT","/TRT/Onl/Cond/StatusHT","/TRT/Cond/StatusHT",className='TRTCond::StrawStatusMultChanContainer')

    # TRT PID tools        
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
        conddb.addFolder('TDAQ_ONL',tdaqFolder,className='CondAttrListCollection')
        from TRT_ConditionsServices.TRT_ConditionsServicesConf import TRT_DAQ_ConditionsSvc
        InDetTRT_DAQ_ConditionsSvc = TRT_DAQ_ConditionsSvc( name = "InDetTRT_DAQ_ConditionsSvc" )
        ServiceMgr += InDetTRT_DAQ_ConditionsSvc
        if (InDetFlags.doPrintConfigurables()):
            printfunc (InDetTRT_DAQ_ConditionsSvc)
    
    #
    # Load and Configure TRT Conditions Services
    #
    InDetTRTConditionsServices=[]

    # Dead/Noisy Straw Service
    useOldStyle = False
    from AthenaCommon.AlgSequence import AlgSequence
    topSequence = AlgSequence()
    if DetFlags.simulate.any_on() or hasattr(topSequence,"OutputConditionsAlg"):
         useOldStyle = True

    # Straw status tool
    from TRT_ConditionsServices.TRT_ConditionsServicesConf import TRT_StrawStatusSummaryTool
    InDetTRTStrawStatusSummaryTool = TRT_StrawStatusSummaryTool(name = "TRT_StrawStatusSummaryTool",
                                                            isGEANT4 = useOldStyle)
    # CalDb tool
    from TRT_ConditionsServices.TRT_ConditionsServicesConf import TRT_CalDbTool
    InDetTRTCalDbTool = TRT_CalDbTool(name = "TRT_CalDbTool")

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

    if InDetFlags.doCosmics() :
        # Average T0 CondAlg
        from TRT_ConditionsAlgs.TRT_ConditionsAlgsConf import TRTPhaseCondAlg
        TRTPhaseCondAlg = TRTPhaseCondAlg(name = "TRTPhaseCondAlg",
                                          TRTCalDbTool = InDetTRTCalDbTool)
        condSeq += TRTPhaseCondAlg

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
                printfunc (InDetTRT_HWMappingSvc)

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
                printfunc (InDetTRT_DCS_ConditionsSvc)
            #InDetTRTConditionsServices.append(InDetTRT_DCS_ConditionsSvc)
    
    # TRT Conditions Summary Service
    from TRT_ConditionsServices.TRT_ConditionsServicesConf import TRT_ConditionsSummarySvc
    InDetTRTConditionsSummaryService = TRT_ConditionsSummarySvc(name        = "InDetTRTConditionsSummaryService",
                                                                ServiceList = InDetTRTConditionsServices)
    ServiceMgr += InDetTRTConditionsSummaryService
    if (InDetFlags.doPrintConfigurables()):
        printfunc (InDetTRTConditionsSummaryService)

        
