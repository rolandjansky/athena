# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
from AthenaCommon.SystemOfUnits import *
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

from CoolLumiUtilities.CoolLumiUtilitiesConf import OnlineLumiCalibrationTool

# Wrapper script to provide existing tool if defined or properly configured new tool if not
def OnlineLumiCalibrationToolDefault(name="OnlineLumiCalibrationTool"):
    mlog = logging.getLogger(name)

    if hasattr(svcMgr.ToolSvc, name):
        # re-use previously configured tool
        mlog.info("OnlineLumiCalibrationToolDefault returning existing tool %s", name)
        return getattr(svcMgr.ToolSvc, name)

    # Instantiate new tool, by default configuration will do nothing
    olcTool = OnlineLumiCalibrationTool(name)

    # Now configure based on the environment
    from IOVDbSvc.CondDB import conddb

    # Run1
    if conddb.dbdata == "COMP200":

        folder = '/TDAQ/OLC/CALIBRATIONS'
        olcTool.CalibrationFolderName = folder

        # Mistakenly created as multi-version folder, must specify HEAD 
        if not conddb.folderRequested( folder ):
            conddb.addFolder('TDAQ', folder)
            mlog.info("OnlineLumiCalibrationToolDefault requested %s", folder)

    # Run2 - do nothing
    elif conddb.dbdata == "CONDBR2":
        pass

    # Unknown, complain and do nothing
    else:
        mlog.warning("OnlineLumiCalibrationToolDefault can't resolve conddb.dbdata = %s, assume Run2!" % conddb.dbdata)
        pass

    return olcTool


