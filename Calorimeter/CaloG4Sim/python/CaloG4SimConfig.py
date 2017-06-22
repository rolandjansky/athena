# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr

def getCalibrationDefaultProcessingTool(name="G4UA::CaloG4::CalibrationDefaultProcessing", **kwargs):
    #kwargs.setdefault("TargetRegions", ["EMEC","EMB"])
    # this SD name is for ATLAS, H8
    kwargs.setdefault("SDName", "Default::Dead::Uninstrumented::Calibration::Region")
    # this is for H6
    #kwargs.setdefault("SDName", "Default::Dead::Uninstrumented::Calibration::H6")
    return CfgMgr.G4UA__CaloG4__CalibrationDefaultProcessingTool(name, **kwargs)
