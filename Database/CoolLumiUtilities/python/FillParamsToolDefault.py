# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
from AthenaCommon.SystemOfUnits import *
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

from CoolLumiUtilities.CoolLumiUtilitiesConf import FillParamsTool

# Wrapper script to provide existing tool if defined or properly configured new tool if not
def FillParamsToolDefault(name="FillParamsTool"):
    mlog = logging.getLogger(name)

    if hasattr(svcMgr.ToolSvc, name):
        # re-use previously configured tool
        mlog.info("FillParamsToolDefault returning existing tool %s", name)
        return getattr(svcMgr.ToolSvc, name)

    # Instantiate new tool, by default configuration will do nothing
    fpTool = FillParamsTool(name)

    # Now configure based on the environment
    from IOVDbSvc.CondDB import conddb

    # Run1
    if conddb.dbdata == "COMP200":

        folder = '/TDAQ/OLC/LHC/FILLPARAMS'
        fpTool.FillParamsFolderName = folder

        # Mistakenly created as multi-version folder, must specify HEAD 
        from IOVDbSvc.CondDB import conddb
        if not conddb.folderRequested( folder ):
            conddb.addFolder('TDAQ', folder)
            mlog.info("FillParamsToolDefault requested %s", folder)

    # Run2 - do nothing
    elif conddb.dbdata == "CONDBR2":
        pass

    # Unknown, complain and do nothing
    else:
        mlog.warning("FillParamsToolDefault can't resolve conddb.dbdata = %s, assume Run2!" % conddb.dbdata)
        pass

    return fpTool


