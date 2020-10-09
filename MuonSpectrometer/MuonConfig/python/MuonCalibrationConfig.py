# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Based on : https://gitlab.cern.ch/atlas/athena/blob/master/MuonSpectrometer/MuonCnv/MuonCnvExample/python/MuonCalibConfig.py

from MuonConfig.MuonCondAlgConfig import CscCondDbAlgCfg
from MuonConfig.MuonGeometryConfig import MuonDetectorCondAlgCfg
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from IOVDbSvc.IOVDbSvcConfig import addFoldersSplitOnline
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
        ServiceMgr.TagInfoMgr.ExtraTagValuePairs.update({"MDTCalibrationSource": flags.Muon.Calib.mdtCalibrationSource()}) # TODO Check this.
        result.merge(addFoldersSplitOnline(flags, flags.Muon.Calib.mdtCalibrationSource, online_folders, offline_folders,
                                           className = 'CondAttrListCollection' ) )
        
    return result, mdt_folder_name_appendix
# end of function setupMdtCondDB()

def MdtCalibrationToolCfg(flags, **kwargs):
    result=MdtCalibrationDbToolCfg(flags, **kwargs)
    mdt_calibibration_db_tool = result.getPrimary()
    
    kwargs.setdefault("CalibrationDbTool", mdt_calibibration_db_tool)
    kwargs.setdefault("DoSlewingCorrection", flags.Muon.Calib.correctMdtRtForTimeSlewing)
    kwargs.setdefault("DoTemperatureCorrection", flags.Muon.Calib.applyRtScaling)
    kwargs.setdefault("DoWireSagCorrection", flags.Muon.Calib.correctMdtRtWireSag)
    kwargs.setdefault("DoTofCorrection",  flags.Beam.Type == 'collisions' ) # No TOF correction if not collisions
    
    acc = MagneticFieldSvcCfg(flags)
    result.merge(acc)
    
    MdtCalibrationTool = CompFactory.MdtCalibrationTool
    mdt_calibration_tool = MdtCalibrationTool(**kwargs)
    result.setPrivateTools(mdt_calibration_tool)
    return result

def MdtCalibrationDbToolCfg(flags, **kwargs):
    # We need the conditions objects to have been created.
    result = MdtCalibDbAlgCfg(flags, **kwargs)
        
    kwargs.setdefault("CreateBFieldFunctions", flags.Muon.Calib.correctMdtRtForBField)
    kwargs.setdefault("CreateWireSagFunctions", flags.Muon.Calib.correctMdtRtWireSag)
    kwargs.setdefault("CreateSlewingFunctions", flags.Muon.Calib.correctMdtRtForTimeSlewing)
    
    MdtCalibrationDbTool = CompFactory.MdtCalibrationDbTool
    mdt_calibration_db_tool = MdtCalibrationDbTool(**kwargs)
    result.setPrivateTools(mdt_calibration_db_tool)
    return result
    
def MdtCalibDbAlgCfg(flags,name="MdtCalibDbAlg",**kwargs):
    result = MuonDetectorCondAlgCfg(flags)

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
    # the same as MdtCalibrationDbTool
    kwargs.setdefault("CreateBFieldFunctions", flags.Muon.Calib.correctMdtRtForBField)
    kwargs.setdefault("CreateWireSagFunctions", flags.Muon.Calib.correctMdtRtWireSag)
    kwargs.setdefault("CreateSlewingFunctions", flags.Muon.Calib.correctMdtRtForTimeSlewing)

    MdtCalibDbAlg=CompFactory.MdtCalibDbAlg
    alg = MdtCalibDbAlg (name, **kwargs)

    result.addCondAlgo (alg)
    return result
