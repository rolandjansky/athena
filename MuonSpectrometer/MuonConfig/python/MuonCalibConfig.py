# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

# Based on : https://gitlab.cern.ch/atlas/athena/blob/master/MuonSpectrometer/MuonCnv/MuonCnvExample/python/MuonCalibConfig.py

from MuonCondSvc.MuonCondSvcConf import MuonCalib__CscCoolStrSvc
from CscCalibTools.CscCalibToolsConf import CscCalibTool
from MdtCalibSvc.MdtCalibSvcConf import MdtCalibrationDbSvc, MdtCalibrationSvc
from MdtCalibDbCoolStrTool.MdtCalibDbCoolStrToolConf import MuonCalib__MdtCalibDbCoolStrTool
from MuonCnvExample.MuonCnvUtils import mdtCalibWindowNumber, mdtCalibWindowName, specialAddFolderSplitOnline # TODO should maybe shift this elsewhere?
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from IOVDbSvc.IOVDbSvcConfig import IOVDbSvcCfg, addFolders, addFoldersSplitOnline
from MagFieldServices.MagFieldServicesConfig import MagneticFieldSvcCfg
        

################################################################################
# CSC calibration
################################################################################

#
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

def _setupCscCondDB( flags, name, key, dataType, cat, default, folder, database, useLocal, override="" ):

    # The followng codes should be cleaned up by CSC experts, 
    #   since they quate a few verses from the following two files
    #   https://gitlab.cern.ch/atlas/athena/blob/master/MuonSpectrometer/MuonCnv/MuonCnvExample/python/MuonCalibConfig.py
    #   https://gitlab.cern.ch/atlas/athena/blob/master/MuonSpectrometer/MuonConditions/MuonCondGeneral/MuonCondSvc/python/CscCondDB.py

    acc = ComponentAccumulator()

    # This need to adapt FolderSuffix variable
    #fullSuffix = " <key>" + key + "</key> " + self.FolderSuffix
    fullSuffix = " <key>" + key + "</key> "

    oflFolderString = '/CSC/' + folder + fullSuffix
    onlFolderString = '/CSC/ONL/' + folder + fullSuffix

    if useLocal:
        #assume when local, the folder will be in offline location. 
        #Maybe add more options in future
        acc.merge( addFolders( flags, oflFolderString, "LOCAL" ) )
    elif database=='CSC':
        acc.merge( addFoldersSplitOnline( flags, database, onlFolderString, oflFolderString ) )
    elif database=='CSC_OFL':
        acc.merge( addFolders( flags, oflFolderString, database ) )
    else:
        print "Failed to recognize database: " + database + " for parameter " + name
        return acc 

    if override:
        if useLocal:
            #assume when local, the folder will be in offline location. 
            #Maybe add more options in future
            overfolder = oflFolderString.split()[0] #Get folder without suffix
        elif datebase=='CSC':
            if flags.Common.isOnline:
                overfolder = onlFolderString.split()[0] #Get folder without suffix
            else:
                overfolder = oflFolderString.split()[0] #Get folder without suffix
        elif database=='CSC_OFL':
            overfolder = oflFolderString.split()[0] #Get folder without suffix
        else:
            print "Failed to recognize database: " + database + " for parameter " + name
            return acc
        print "Overriding folder for " + name + "(" + overfolder + ") to " + override 
        #acc.merge( addOverride( flags, overfolder, override ) )

    return acc

def CscCoolStrSvcCfg( flags ):

    acc = ComponentAccumulator()

    # used folders
    pslopeFolder    = "PSLOPE"   # Pulser run gain
    pedFolder       = "PED"      # Pedestals
    noiseFolder     = "NOISE"    # Noise (Sigma from pedestal gaussian fit)
    rmsFolder       = "RMS"      # Rms of pedestal histograms (not from fit)
    f001Folder      = "FTHOLD"   # F001 threshold values
    statusFolder    = "STAT"     # Status Bits
    t0baseFolder    = "T0BASE"
    t0phaseFolder   = "T0PHASE"

    # unused folders
    gainFolder      = "GAIN"
    runSlopeFolder  = "RSLOPE"
    tholdFolder     = "THOLD"
    peaktFolder     = "PEAKT"
    widthFolder     = "WIDTH"
    sat1Folder      = "SAT1"
    sat2Folder      = "SAT2"

    # Set CscCoolStr Svc to prepare condDB for pedestal, noise and so on
    CscCoolStrSvc = MuonCalib__CscCoolStrSvc()
    acc.addService( CscCoolStrSvc )
 
    # The followng codes should be cleaned up by CSC experts, 
    #   since they quate a few verses from the following two files
    #   https://gitlab.cern.ch/atlas/athena/blob/master/MuonSpectrometer/MuonCnv/MuonCnvExample/python/MuonCalibConfig.py
    #   https://gitlab.cern.ch/atlas/athena/blob/master/MuonSpectrometer/MuonConditions/MuonCondGeneral/MuonCondSvc/python/CscCondDB.py

    # Adds pedestal and noise folders
    if flags.Muon.Calib.CscPedFromLocalFile:
        acc.merge(  _setupCscCondDB( flags, name="ped", key="CSC_PED", dataType="float", cat="CHANNEL", 
                                     default="2048", folder=pedFolder, database="CSC", useLocal=True, override="" ) )

        CscCoolStrSvc.ParNames.append( "ped" )
        CscCoolStrSvc.ParSGKeys.append( "CSC_PED" )
        CscCoolStrSvc.ParDataTypes.append( "float" )
        CscCoolStrSvc.ParCats.append( "CHANNEL" )
        CscCoolStrSvc.ParDefaults.append( "2048" )

        if flags.Common.isOnline:
            CscCoolStrSvc.ParFolders.append('/CSC/ONL/' + pedFolder)
        else:
            CscCoolStrSvc.ParFolders.append('/CSC/' + pedFolder)

    else:
        acc.merge(  _setupCscCondDB( flags, name="ped", key="CSC_PED", dataType="float", cat="CHANNEL", 
                                     default="2048", folder=pedFolder, database="CSC", useLocal=False, override="" ) )

        CscCoolStrSvc.ParNames.append( "ped" )
        CscCoolStrSvc.ParSGKeys.append( "CSC_PED" )
        CscCoolStrSvc.ParDataTypes.append( "float" )
        CscCoolStrSvc.ParCats.append( "CHANNEL" )
        CscCoolStrSvc.ParDefaults.append( "2048" )

        if flags.Common.isOnline:
            CscCoolStrSvc.ParFolders.append('/CSC/ONL/' + pedFolder)
        else:
            CscCoolStrSvc.ParFolders.append('/CSC/' + pedFolder)

    # Load Noise File
    if flags.Muon.Calib.CscNoiseFromLocalFile:
        acc.merge(  _setupCscCondDB( flags, name="noise", key="CSC_NOISE", dataType="float", cat="CHANNEL", 
                                     default="3.5", folder=noiseFolder, database="CSC", useLocal=True, override="" ) )

        CscCoolStrSvc.ParNames.append( "noise" )
        CscCoolStrSvc.ParSGKeys.append( "CSC_NOISE" )
        CscCoolStrSvc.ParDataTypes.append( "float" )
        CscCoolStrSvc.ParCats.append( "CHANNEL" )
        CscCoolStrSvc.ParDefaults.append( "3.5" )

        if flags.Common.isOnline:
            CscCoolStrSvc.ParFolders.append('/CSC/ONL/' + noiseFolder)
        else:
            CscCoolStrSvc.ParFolders.append('/CSC/' + noiseFolder)

    else:
        acc.merge(  _setupCscCondDB( flags, name="noise", key="CSC_NOISE", dataType="float", cat="CHANNEL", 
                                     default="3.5", folder=noiseFolder, database="CSC", useLocal=False, override="" ) )

        CscCoolStrSvc.ParNames.append( "noise" )
        CscCoolStrSvc.ParSGKeys.append( "CSC_NOISE" )
        CscCoolStrSvc.ParDataTypes.append( "float" )
        CscCoolStrSvc.ParCats.append( "CHANNEL" )
        CscCoolStrSvc.ParDefaults.append( "3.5" )

        if flags.Common.isOnline:
            CscCoolStrSvc.ParFolders.append('/CSC/ONL/' + noiseFolder)
        else:
            CscCoolStrSvc.ParFolders.append('/CSC/' + noiseFolder)

    ### Load PSlope File
    if flags.Muon.Calib.CscPSlopeFromLocalFile:
        acc.merge(  _setupCscCondDB( flags, name="pslope", key="CSC_PSLOPE", dataType="float", cat="CHANNEL", 
                                     default="0.189", folder=pslopeFolder, database="CSC", useLocal=True, override="" ) )

        CscCoolStrSvc.ParNames.append( "pslope" )
        CscCoolStrSvc.ParSGKeys.append( "CSC_PSLOPE" )
        CscCoolStrSvc.ParDataTypes.append( "float" )
        CscCoolStrSvc.ParCats.append( "CHANNEL" )
        CscCoolStrSvc.ParDefaults.append( "0.189" )

        if flags.Common.isOnline:
            CscCoolStrSvc.ParFolders.append('/CSC/ONL/' + pslopeFolder)
        else:
            CscCoolStrSvc.ParFolders.append('/CSC/' + pslopeFolder)
 
    else:
        acc.merge(  _setupCscCondDB( flags, name="pslope", key="CSC_PSLOPE", dataType="float", cat="CHANNEL", 
                                     default="0.189", folder=pslopeFolder, database="CSC", useLocal=False, override="" ) )

        CscCoolStrSvc.ParNames.append( "pslope" )
        CscCoolStrSvc.ParSGKeys.append( "CSC_PSLOPE" )
        CscCoolStrSvc.ParDataTypes.append( "float" )
        CscCoolStrSvc.ParCats.append( "CHANNEL" )
        CscCoolStrSvc.ParDefaults.append( "0.189" )

        if flags.Common.isOnline:
            CscCoolStrSvc.ParFolders.append('/CSC/ONL/' + pslopeFolder)
        else:
            CscCoolStrSvc.ParFolders.append('/CSC/' + pslopeFolder)

    ## Load Status File
    if flags.Muon.Calib.CscStatusFromLocalFile:
        acc.merge(  _setupCscCondDB( flags, name="status", key="CSC_STAT", dataType="uint32_t", cat="CHANNEL", 
                                     default="0", folder=statusFolder, database="CSC", useLocal=True, override="" ) )

        CscCoolStrSvc.ParNames.append( "status" )
        CscCoolStrSvc.ParSGKeys.append( "CSC_STAT" )
        CscCoolStrSvc.ParDataTypes.append( "uint32_t" )
        CscCoolStrSvc.ParCats.append( "CHANNEL" )
        CscCoolStrSvc.ParDefaults.append( "0" )

        if flags.Common.isOnline:
            CscCoolStrSvc.ParFolders.append('/CSC/ONL/' + statusFolder)
        else:
            CscCoolStrSvc.ParFolders.append('/CSC/' + statusFolder)

    else:
        acc.merge(  _setupCscCondDB( flags, name="status", key="CSC_STAT", dataType="uint32_t", cat="CHANNEL", 
                                     default="0", folder=statusFolder, database="CSC", useLocal=False, override="" ) )

        CscCoolStrSvc.ParNames.append( "status" )
        CscCoolStrSvc.ParSGKeys.append( "CSC_STAT" )
        CscCoolStrSvc.ParDataTypes.append( "uint32_t" )
        CscCoolStrSvc.ParCats.append( "CHANNEL" )
        CscCoolStrSvc.ParDefaults.append( "0" )

        if flags.Common.isOnline:
            CscCoolStrSvc.ParFolders.append('/CSC/ONL/' + statusFolder)
        else:
            CscCoolStrSvc.ParFolders.append('/CSC/' + statusFolder)

    ## Load Rms File
    if flags.Muon.Calib.CscRmsFromLocalFile:
        acc.merge(  _setupCscCondDB( flags, name="rms", key="CSC_RMS", dataType="float", cat="CHANNEL", 
                                     default="2.56", folder=rmsFolder, database="CSC", useLocal=True, override="" ) )

        CscCoolStrSvc.ParNames.append( "rms" )
        CscCoolStrSvc.ParSGKeys.append( "CSC_RMS" )
        CscCoolStrSvc.ParDataTypes.append( "float" )
        CscCoolStrSvc.ParCats.append( "CHANNEL" )
        CscCoolStrSvc.ParDefaults.append( "2.56" )

        if flags.Common.isOnline:
            CscCoolStrSvc.ParFolders.append('/CSC/ONL/' + rmsFolder)
        else:
            CscCoolStrSvc.ParFolders.append('/CSC/' + rmsFolder)

    else:
        acc.merge(  _setupCscCondDB( flags, name="rms", key="CSC_RMS", dataType="float", cat="CHANNEL", 
                                     default="2.56", folder=rmsFolder, database="CSC", useLocal=False, override="" ) )

        CscCoolStrSvc.ParNames.append( "rms" )
        CscCoolStrSvc.ParSGKeys.append( "CSC_RMS" )
        CscCoolStrSvc.ParDataTypes.append( "float" )
        CscCoolStrSvc.ParCats.append( "CHANNEL" )
        CscCoolStrSvc.ParDefaults.append( "2.56" )

        if flags.Common.isOnline:
            CscCoolStrSvc.ParFolders.append('/CSC/ONL/' + rmsFolder)
        else:
            CscCoolStrSvc.ParFolders.append('/CSC/' + rmsFolder)

    ## Load F001 File
    if flags.Muon.Calib.CscF001FromLocalFile:
        acc.merge(  _setupCscCondDB( flags, name="f001", key="CSC_FTHOLD", dataType="float", cat="CHANNEL", 
                                     default="0", folder=f001Folder, database="CSC", useLocal=True, override="" ) )

        CscCoolStrSvc.ParNames.append( "f001" )
        CscCoolStrSvc.ParSGKeys.append( "CSC_FTHOLD" )
        CscCoolStrSvc.ParDataTypes.append( "float" )
        CscCoolStrSvc.ParCats.append( "CHANNEL" )
        CscCoolStrSvc.ParDefaults.append( "0" )

        if flags.Common.isOnline:
            CscCoolStrSvc.ParFolders.append('/CSC/ONL/' + f001Folder)
        else:
            CscCoolStrSvc.ParFolders.append('/CSC/' + f001Folder)

    else:
        acc.merge(  _setupCscCondDB( flags, name="f001", key="CSC_FTHOLD", dataType="float", cat="CHANNEL", 
                                     default="0", folder=f001Folder, database="CSC", useLocal=False, override="" ) )

        CscCoolStrSvc.ParNames.append( "f001" )
        CscCoolStrSvc.ParSGKeys.append( "CSC_FTHOLD" )
        CscCoolStrSvc.ParDataTypes.append( "float" )
        CscCoolStrSvc.ParCats.append( "CHANNEL" )
        CscCoolStrSvc.ParDefaults.append( "0" )

        if flags.Common.isOnline:
            CscCoolStrSvc.ParFolders.append('/CSC/ONL/' + f001Folder)
        else:
            CscCoolStrSvc.ParFolders.append('/CSC/' + f001Folder)

    # Offline only 
    if not flags.Common.isOnline:

        ## Load T0Base File
        if flags.Muon.Calib.CscT0BaseFromLocalFile:
            acc.merge(  _setupCscCondDB( flags, name="t0base", key="CSC_T0BASE", dataType="float", cat="CHANNEL", 
                                         default="0", folder=t0baseFolder, database="CSC_OFL", useLocal=True, override="" ) )
    
            CscCoolStrSvc.ParNames.append( "t0base" )
            CscCoolStrSvc.ParSGKeys.append( "CSC_T0BASE" )
            CscCoolStrSvc.ParDataTypes.append( "float" )
            CscCoolStrSvc.ParCats.append( "CHANNEL" )
            CscCoolStrSvc.ParDefaults.append( "0" )
            CscCoolStrSvc.ParFolders.append('/CSC/' + t0baseFolder)

        else:
            acc.merge(  _setupCscCondDB( flags, name="t0base", key="CSC_T0BASE", dataType="float", cat="CHANNEL", 
                                         default="0", folder=t0baseFolder, database="CSC_OFL", useLocal=False, override="" ) )
    
            CscCoolStrSvc.ParNames.append( "t0base" )
            CscCoolStrSvc.ParSGKeys.append( "CSC_T0BASE" )
            CscCoolStrSvc.ParDataTypes.append( "float" )
            CscCoolStrSvc.ParCats.append( "CHANNEL" )
            CscCoolStrSvc.ParDefaults.append( "0" )
            CscCoolStrSvc.ParFolders.append('/CSC/' + t0baseFolder)

        ## Load T0Phase File
        if flags.Muon.Calib.CscT0PhaseFromLocalFile:
            acc.merge(  _setupCscCondDB( flags, name="t0phase", key="CSC_T0PHASE", dataType="float", cat="ASM", 
                                         default="0", folder=t0phaseFolder, database="CSC_OFL", useLocal=True, override="" ) )
    
            CscCoolStrSvc.ParNames.append( "t0phase" )
            CscCoolStrSvc.ParSGKeys.append( "CSC_T0PHASE" )
            CscCoolStrSvc.ParDataTypes.append( "float" )
            CscCoolStrSvc.ParCats.append( "ASM" )
            CscCoolStrSvc.ParDefaults.append( "0" )
            CscCoolStrSvc.ParFolders.append('/CSC/' + t0phaseFolder)

        else:
            acc.merge(  _setupCscCondDB( flags, name="t0phase", key="CSC_T0PHASE", dataType="float", cat="ASM", 
                                         default="0", folder=t0phaseFolder, database="CSC_OFL", useLocal=False, override="" ) )
    
            CscCoolStrSvc.ParNames.append( "t0phase" )
            CscCoolStrSvc.ParSGKeys.append( "CSC_T0PHASE" )
            CscCoolStrSvc.ParDataTypes.append( "float" )
            CscCoolStrSvc.ParCats.append( "ASM" )
            CscCoolStrSvc.ParDefaults.append( "0" )
            CscCoolStrSvc.ParFolders.append('/CSC/' + t0phaseFolder)


    return acc, CscCoolStrSvc

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

def MdtCalibDbToolCfg(flags,name="MdtCalibDbTool",**kwargs):
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
    if flags.Input.isMC == False: # Should be " if flags.Input.isMC=='data' " ?
        kwargs.setdefault("defaultT0", 40)
    else:
        kwargs.setdefault("defaultT0", 799)
    kwargs.setdefault("UseMLRt",  flags.Muon.Calib.useMLRt )
    kwargs.setdefault("TimeSlewingCorrection", flags.Muon.Calib.correctMdtRtForTimeSlewing)
    kwargs.setdefault("MeanCorrectionVsR", [ -5.45973, -4.57559, -3.71995, -3.45051, -3.4505, -3.4834, -3.59509, -3.74869, -3.92066, -4.10799, -4.35237, -4.61329, -4.84111, -5.14524 ])
    kwargs.setdefault("PropagationSpeedBeta", flags.Muon.Calib.mdtPropagationSpeedBeta)
    return result, MuonCalib__MdtCalibDbCoolStrTool(name,**kwargs)

def MdtCalibrationDbSvcCfg(flags, **kwargs):
    result=ComponentAccumulator()

    kwargs.setdefault( "CreateBFieldFunctions", flags.Muon.Calib.correctMdtRtForBField )
    kwargs.setdefault( "CreateWireSagFunctions", flags.Muon.Calib.correctMdtRtWireSag )
    kwargs.setdefault( "CreateSlewingFunctions", flags.Muon.Calib.correctMdtRtForTimeSlewing)
    kwargs.setdefault( "DBTool", "MuonCalib::MdtCalibDbCoolStrTool/MdtCalibDbTool")
 
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
