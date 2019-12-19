# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

# Based on : https://gitlab.cern.ch/atlas/athena/blob/master/MuonSpectrometer/MuonCnv/MuonCnvExample/python/MuonCalibConfig.py

from MuonConfig.MuonCondAlgConfig import CscCondDbAlgCfg
from AthenaConfiguration.ComponentFactory import CompFactory
MuonCalib__CscCoolStrSvc=CompFactory.MuonCalib__CscCoolStrSvc
MuonCalib__MdtCalibDbCoolStrTool=CompFactory.MuonCalib__MdtCalibDbCoolStrTool
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from IOVDbSvc.IOVDbSvcConfig import addFolders, addFoldersSplitOnline
from MagFieldServices.MagFieldServicesConfig import MagneticFieldSvcCfg
CscCalibTool=CompFactory.CscCalibTool

from AthenaCommon.Logging import logging
log = logging.getLogger('MuonCalibConfig')

################################################################################
# CSC calibration
################################################################################

def CscCalibToolCfg(flags, name="CscCalibTool", **kwargs):
    """Return ComponentAccumulator configured for CSC calibration with CscCalibTool as PrivateTools"""

    acc = CscCondDbAlgCfg(flags)

    kwargs.setdefault("Slope", 0.19)
    kwargs.setdefault("Noise", 3.5)
    kwargs.setdefault("Pedestal", 2048.0)
    kwargs.setdefault("ReadFromDatabase", True)
    kwargs.setdefault("SlopeFromDatabase", False)
    kwargs.setdefault("integrationNumber", 12.0)
    kwargs.setdefault("integrationNumber2", 11.66)
    kwargs.setdefault("samplingTime", 50.0)
    kwargs.setdefault("signalWidth", 14.40922)
    kwargs.setdefault("timeOffset", 71.825)
    kwargs.setdefault("IsOnline", flags.Common.isOnline)
    kwargs.setdefault("Latency", 119)

    acc.setPrivateTools(CscCalibTool(name, **kwargs))

    return acc


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
        log.error("Failed to recognize database: " + database + " for parameter " + name)
        return acc 

    if override:
        if useLocal:
            #assume when local, the folder will be in offline location. 
            #Maybe add more options in future
            overfolder = oflFolderString.split()[0] #Get folder without suffix
        elif database=='CSC':
            if flags.Common.isOnline:
                overfolder = onlFolderString.split()[0] #Get folder without suffix
            else:
                overfolder = oflFolderString.split()[0] #Get folder without suffix
        elif database=='CSC_OFL':
            overfolder = oflFolderString.split()[0] #Get folder without suffix
        else:
            log.error("Failed to recognize database: " + database + " for parameter " + name)
            return acc
        log.info("Overriding folder for " + name + "(" + overfolder + ") to " + override)
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
    #gainFolder      = "GAIN"
    #runSlopeFolder  = "RSLOPE"
    #tholdFolder     = "THOLD"
    #peaktFolder     = "PEAKT"
    #widthFolder     = "WIDTH"
    #sat1Folder      = "SAT1"
    #sat2Folder      = "SAT2"

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


    return acc

################################################################################
# MDT calibration
################################################################################

def _setupMdtCondDB(flags):
    result=ComponentAccumulator()
    
    if flags.Muon.Calib.readMDTCalibFromBlob:
        mdt_folder_name_appendix = "BLOB" 
    else:
        mdt_folder_name_appendix=""
    
    online_folders = ['/MDT/Onl/RT'+ mdt_folder_name_appendix,'/MDT/Onl/T0' + mdt_folder_name_appendix]
    offline_folders = ['/MDT/RT' + mdt_folder_name_appendix, '/MDT/T0' + mdt_folder_name_appendix]

    if flags.Muon.Calib.mdtCalibrationSource=="MDT":
        result.merge(addFoldersSplitOnline(flags, 'MDT', online_folders , offline_folders,
                                           className = 'CondAttrListCollection' ) )
    else:
        from AthenaCommon.AppMgr import ServiceMgr
        ServiceMgr.TagInfoMgr.ExtraTagValuePairs += ["MDTCalibrationSource", flags.Muon.Calib.mdtCalibrationSource()] # TODO Check this.
        result.merge(addFoldersSplitOnline(flags, flags.Muon.Calib.mdtCalibrationSource, online_folders, offline_folders,
                                           className = 'CondAttrListCollection' ) )
        
    return result, mdt_folder_name_appendix
# end of function setupMdtCondDB()

def MdtCalibrationToolCfg(flags, **kwargs):
    from MdtCalibSvc.MdtCalibSvcConf import MdtCalibrationTool    
    result=MdtCalibrationDbToolCfg(flags, **kwargs)
    mdt_calibibration_db_tool = result.getPrimary()
    
    kwargs.setdefault("CalibrationDbTool", mdt_calibibration_db_tool)
    kwargs.setdefault("DoSlewingCorrection", flags.Muon.Calib.correctMdtRtForTimeSlewing)
    kwargs.setdefault("DoTemperatureCorrection", flags.Muon.Calib.applyRtScaling)
    kwargs.setdefault("DoWireSagCorrection", flags.Muon.Calib.correctMdtRtWireSag)
    kwargs.setdefault("DoTofCorrection",  flags.Beam.Type == 'collisions' ) # No TOF correction if not collisions
    
    acc = MagneticFieldSvcCfg(flags)
    mag_field_svc = acc.getPrimary()
    result.merge(acc)
    kwargs.setdefault("MagFieldSvc",  mag_field_svc )
    
    mdt_calibration_tool = MdtCalibrationTool(**kwargs)
    result.setPrivateTools(mdt_calibration_tool)
    return result

def MdtCalibrationDbToolCfg(flags, **kwargs):
    from MdtCalibSvc.MdtCalibSvcConf import MdtCalibrationDbTool    
    # We need the conditions objects to have been created.
    result = MdtCalibDbAlgCfg(flags, **kwargs)
        
    kwargs.setdefault("CreateBFieldFunctions", flags.Muon.Calib.correctMdtRtForBField)
    kwargs.setdefault("CreateWireSagFunctions", flags.Muon.Calib.correctMdtRtWireSag)
    kwargs.setdefault("CreateSlewingFunctions", flags.Muon.Calib.correctMdtRtForTimeSlewing)
    
    mdt_calibration_db_tool = MdtCalibrationDbTool(**kwargs)
    result.setPrivateTools(mdt_calibration_db_tool)
    return result
    

def MdtCalibDbCoolStrToolCfg(flags,name="MdtCalibDbTool",**kwargs):    
    # result.merge( IOVDbSvcCfg(flags) )
    
    # setup COOL folders
    result, mdt_folder_name_appendix = _setupMdtCondDB(flags)
    
    # set some default proper ties
    from IOVDbSvc.CondDB import conddb
    if conddb.isOnline and not conddb.isMC:
       kwargs.setdefault("TubeFolder", "/MDT/T0")
       kwargs.setdefault("RtFolder",  "/MDT/RT")
    else:
       kwargs.setdefault("TubeFolder", "/MDT/T0"+ mdt_folder_name_appendix)
       kwargs.setdefault("RtFolder",  "/MDT/RT"+ mdt_folder_name_appendix)
    kwargs.setdefault("RT_InputFiles" , ["Muon_RT_default.data"])
    if flags.Input.isMC is False: # Should be " if flags.Input.isMC=='data' " ?
        kwargs.setdefault("defaultT0", 40)
    else:
        kwargs.setdefault("defaultT0", 799)
    kwargs.setdefault("UseMLRt",  flags.Muon.Calib.useMLRt )
    kwargs.setdefault("TimeSlewingCorrection", flags.Muon.Calib.correctMdtRtForTimeSlewing)
    kwargs.setdefault("MeanCorrectionVsR", [ -5.45973, -4.57559, -3.71995, -3.45051, -3.4505, -3.4834, -3.59509, -3.74869, -3.92066, -4.10799, -4.35237, -4.61329, -4.84111, -5.14524 ])
    kwargs.setdefault("PropagationSpeedBeta", flags.Muon.Calib.mdtPropagationSpeedBeta)
    result.setPrivateTools(MuonCalib__MdtCalibDbCoolStrTool(name,**kwargs))
    return result

def MdtCalibDbAlgCfg(flags,name="MdtCalibDbAlg",**kwargs):
    result = ComponentAccumulator()

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
    kwargs.setdefault("ReadKeyTube", kwargs["TubeFolder"])
    kwargs.setdefault("ReadKeyRt",   kwargs["RtFolder"])
    kwargs.setdefault("RT_InputFiles" , ["Muon_RT_default.data"])
    if flags.Input.isMC is False: # Should be " if flags.Input.isMC=='data' " ?
        kwargs.setdefault("defaultT0", 40)
    else:
        kwargs.setdefault("defaultT0", 799)
    kwargs.setdefault("UseMLRt",  flags.Muon.Calib.useMLRt )
    kwargs.setdefault("TimeSlewingCorrection", flags.Muon.Calib.correctMdtRtForTimeSlewing)
    kwargs.setdefault("MeanCorrectionVsR", [ -5.45973, -4.57559, -3.71995, -3.45051, -3.4505, -3.4834, -3.59509, -3.74869, -3.92066, -4.10799, -4.35237, -4.61329, -4.84111, -5.14524 ])
    kwargs.setdefault("PropagationSpeedBeta", flags.Muon.Calib.mdtPropagationSpeedBeta)

    kwargs.setdefault("CreateBFieldFunctions", flags.Muon.Calib.correctMdtRtForBField)
    kwargs.setdefault("CreateWireSagFunctions", flags.Muon.Calib.correctMdtRtWireSag)
    kwargs.setdefault("CreateSlewingFunctions", flags.Muon.Calib.correctMdtRtForTimeSlewing)

    MdtCalibDbAlg=CompFactory.MdtCalibDbAlg
    alg = MdtCalibDbAlg (name, **kwargs)

    result.addCondAlgo (alg)
    return result
