# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

# Based on : https://gitlab.cern.ch/atlas/athena/blob/master/MuonSpectrometer/MuonCnv/MuonCnvExample/python/MuonCalibConfig.py

from MdtCalibSvc.MdtCalibSvcConf import MdtCalibrationDbSvc, MdtCalibrationSvc
from MdtCalibDbCoolStrTool.MdtCalibDbCoolStrToolConf import MuonCalib__MdtCalibDbCoolStrTool
from MuonCnvExample.MuonCnvUtils import mdtCalibWindowNumber, mdtCalibWindowName, specialAddFolderSplitOnline # TODO should maybe shift this elsewhere?
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from IOVDbSvc.IOVDbSvcConfig import IOVDbSvcCfg, addFolders, addFoldersSplitOnline
from MagFieldServices.MagFieldServicesConfig import MagneticFieldSvcCfg
        

################################################################################
# CSC calibration
################################################################################

# def setupCscCondDB():
#     # Access to the CSC calibration database
#     # Copied and modified from:
#     # include( "CscCalibTools/CscCalibTool_jobOptions.py" )
#     from MuonCondSvc.CscCondDB import cscCondDB
#
#     ## Load Ped File
#     if cscCalibFlags.CscPedFromLocalFile:
#         cscCondDB.useLocalFile( True )     #All following "add" db folder commands are from local files
#         cscCondDB.addPedFolder()    #<--- Adds pedestal and noise folders
#         cscCondDB.useLocalFile( False )     #To make sure to stop refering to local sqlite DB file
#     else:
#         cscCondDB.addPedFolder()    #<--- Adds pedestal and noise folders
#
#     ## Load Noise File
#     if cscCalibFlags.CscNoiseFromLocalFile:
#         cscCondDB.useLocalFile( True )     #All following "add" db folder commands are from local files
#         cscCondDB.addNoiseFolder()    #<--- Adds pedestal and noise folders
#         cscCondDB.useLocalFile( False )     #To make sure to stop refering to local sqlite DB file
#     else:
#         cscCondDB.addNoiseFolder()    #<--- Adds pedestal and noise folders
#
#     ## Load PSlope File
#     if cscCalibFlags.CscPSlopeFromLocalFile:
#         cscCondDB.useLocalFile( True )     #All following "add" db folder commands are from local files
#         cscCondDB.addPSlopeFolder()    #<--- Adds pedestal and noise folders
#         cscCondDB.useLocalFile( False )     #To make sure to stop refering to local sqlite DB file
#     else:
#         cscCondDB.addPSlopeFolder()    #<--- Adds pedestal and noise folders
#
#     ## Load Status File
#     if cscCalibFlags.CscStatusFromLocalFile:
#         cscCondDB.useLocalFile( True )     #All following "add" db folder commands are from local files
#         cscCondDB.addStatusFolder()    #<--- Adds pedestal and noise folders
#         cscCondDB.useLocalFile( False )     #To make sure to stop refering to local sqlite DB file
#     else:
#         cscCondDB.addStatusFolder()    #<--- Adds pedestal and noise folders
#
#     ## Load Rms File
#     if cscCalibFlags.CscRmsFromLocalFile:
#         cscCondDB.useLocalFile( True )     #All following "add" db folder commands are from local files
#         cscCondDB.addRmsFolder()    #<--- Adds pedestal and noise folders
#         cscCondDB.useLocalFile( False )     #To make sure to stop refering to local sqlite DB file
#     else:
#         cscCondDB.addRmsFolder()    #<--- Adds pedestal and noise folders
#
#     ## Load F001 File
#     if cscCalibFlags.CscF001FromLocalFile:
#         cscCondDB.useLocalFile( True )     #All following "add" db folder commands are from local files
#         cscCondDB.addF001Folder()
#         cscCondDB.useLocalFile( False )     #To make sure to stop refering to local sqlite DB file
#     else:
#         cscCondDB.addF001Folder()
#
#
#     if not athenaCommonFlags.isOnline():
#         log = logging.getLogger(__name__+".setupCscCondDB()")
#         log.info("This is for OffLine so T0Base and T0Phase folders are added!!")
#
#         ## Load T0Base File
#         if cscCalibFlags.CscT0BaseFromLocalFile:
#             cscCondDB.useLocalFile( True )     #All following "add" db folder commands are from local files
#             cscCondDB.addT0BaseFolder()
#             cscCondDB.useLocalFile( False )     #To make sure to stop refering to local sqlite DB file
#         else:
#             cscCondDB.addT0BaseFolder()
#
#
#         ## Load T0Phase File
#         if cscCalibFlags.CscT0PhaseFromLocalFile:
#             cscCondDB.useLocalFile( True )     #All following "add" db folder commands are from local files
#             cscCondDB.addT0PhaseFolder()
#             cscCondDB.useLocalFile( False )     #To make sure to stop refering to local sqlite DB file
#         else:
#             cscCondDB.addT0PhaseFolder()
#
#     #cscCondDB.addPedFolders()    #<--- Adds pedestal and noise folders
#     #cscCondDB.addAllFolders()
#
# # end of function setupCscCondDB()
#
# #
# def CscCalibTool(name,**kwargs):
#     # setup condDB folders
#     setupCscCondDB()
#     # make tool
#     return CfgMgr.CscCalibTool(
#         Slope=0.19,
#         Noise=3.5,
#         Pedestal=2048.0,
#         ReadFromDatabase=True,
#         SlopeFromDatabase=False,
#         integrationNumber=12.0,
#         integrationNumber2=11.66,
#         samplingTime=50.0,
#         signalWidth=14.40922,
#         timeOffset=71.825,
#         IsOnline = athenaCommonFlags.isOnline(),
#         Latency = 119
#         )
#     #               timeOffset=46.825) + 25 SimHIT digit time


################################################################################
# MDT calibration
################################################################################

def _setupMdtCondDB(flags):
    result=ComponentAccumulator()
    
    # print '_setupMdtCondDB flags.Muon.Calib.readMDTCalibFromBlob:',flags.Muon.Calib.readMDTCalibFromBlob
    if flags.Muon.Calib.readMDTCalibFromBlob:
        mdt_folder_name_appendix = "BLOB" 
    else:
        mdt_folder_name_appendix=""
    
    online_folders = ['/MDT/Onl/RT'+ mdt_folder_name_appendix,'/MDT/Onl/T0' + mdt_folder_name_appendix]
    offline_folders = ['/MDT/RT' + mdt_folder_name_appendix, '/MDT/T0' + mdt_folder_name_appendix]

    if flags.Muon.Calib.mdtCalibrationSource=="MDT":
        result.merge(addFoldersSplitOnline(flags, 'MDT', online_folders , offline_folders ) )       
    else:
        from AthenaCommon.AppMgr import ServiceMgr
        ServiceMgr.TagInfoMgr.ExtraTagValuePairs += ["MDTCalibrationSource", flags.Muon.Calib.mdtCalibrationSource()] # TODO Check this.
        addMCString=None
        result.merge(addFoldersSplitOnline(flags, flags.Muon.Calib.mdtCalibrationSource, online_folders, offline_folders ) )
        
    return result, mdt_folder_name_appendix
# end of function setupMdtCondDB()

def MdtCalibDbToolCfg(flags,**kwargs):
    result=ComponentAccumulator()
    
    # result.merge( IOVDbSvcCfg(flags) )
    
    # setup COOL folders
    acc, mdt_folder_name_appendix = _setupMdtCondDB(flags)
    result.merge(acc)
    
    # set some default proper ties
    from IOVDbSvc.CondDB import conddb
    if conddb.isOnline and not conddb.isMC:
       kwargs.setdefault("TubeFolder", "/MDT/T0")
       kwargs.setdefault("RtFolder",  "/MDT/RT")
    else:
       kwargs.setdefault("TubeFolder", "/MDT/T0"+ mdt_folder_name_appendix)
       kwargs.setdefault("RtFolder",  "/MDT/RT"+ mdt_folder_name_appendix)
    kwargs.setdefault("RT_InputFiles" , ["Muon_RT_default.data"])
    if not flags.Input.isMC == 'data':
        kwargs.setdefault("defaultT0", 40)
    else:
        kwargs.setdefault("defaultT0", 799)
    kwargs.setdefault("UseMLRt",  flags.Muon.Calib.useMLRt )
    kwargs.setdefault("TimeSlewingCorrection", flags.Muon.Calib.correctMdtRtForTimeSlewing)
    kwargs.setdefault("MeanCorrectionVsR", [ -5.45973, -4.57559, -3.71995, -3.45051, -3.4505, -3.4834, -3.59509, -3.74869, -3.92066, -4.10799, -4.35237, -4.61329, -4.84111, -5.14524 ])
    kwargs.setdefault("PropagationSpeedBeta", flags.Muon.Calib.mdtPropagationSpeedBeta)
    return result, MuonCalib__MdtCalibDbCoolStrTool(**kwargs)

def MdtCalibrationDbSvcCfg(flags, **kwargs):
    result=ComponentAccumulator()

    kwargs.setdefault( "CreateBFieldFunctions", flags.Muon.Calib.correctMdtRtForBField )
    kwargs.setdefault( "CreateWireSagFunctions", flags.Muon.Calib.correctMdtRtWireSag )
    kwargs.setdefault( "CreateSlewingFunctions", flags.Muon.Calib.correctMdtRtForTimeSlewing)
    
    acc, mdt_calib_db_tool = MdtCalibDbToolCfg(flags)
    acc.addPublicTool(mdt_calib_db_tool)
    result.merge(acc)
    # kwargs.setdefault( "DBTool", mdt_calib_db_tool )
    
    mdt_calib_db_svc = MdtCalibrationDbSvc(**kwargs)
    result.addService(mdt_calib_db_svc)
    
    return result, mdt_calib_db_svc

def MdtCalibrationSvcCfg(flags, **kwargs):
    result=ComponentAccumulator()
    
    # call dependent tools. TODO: fix in C++ (move to ServiceHandle + declareProperty)
    # C++ code is missing Property declaration of MagFieldSvc, but it does depend on it.
    
    acc, mag_field_svc = MagneticFieldSvcCfg(flags)
    result.merge( acc )
    
    acc, mdt_calibration_db_svc = MdtCalibrationDbSvcCfg(flags)
    result.merge(acc)
    
    kwargs.setdefault( "DoSlewingCorrection",  flags.Muon.Calib.correctMdtRtForTimeSlewing )

# Hack to use DoTemperatureCorrection for applyRtScaling; but applyRtScaling should not be used anyway, since MLRT can be used
    kwargs.setdefault( "DoTemperatureCorrection", flags.Muon.Calib.applyRtScaling ) #TODO get MDT expert to help fix this.
    kwargs.setdefault( "DoWireSagCorrection",  flags.Muon.Calib.correctMdtRtWireSag )
    if flags.Beam.Type== 'collisions':
        kwargs.setdefault("DoTofCorrection",True)
        # for collisions cut away hits that are far outside of the MDT time window
        kwargs.setdefault( "TimeWindowSetting", mdtCalibWindowNumber('Collision_G4') )

    else: # cosmics or single beam
        kwargs.setdefault("DoTofCorrection",False)

    mdt_calib_svc = MdtCalibrationSvc(**kwargs)
    result.addService(mdt_calib_svc)
    return result, MdtCalibrationSvc(**kwargs)
