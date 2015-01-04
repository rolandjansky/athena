# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
from AthenaCommon.SystemOfUnits import *
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

from CoolLumiUtilities.CoolLumiUtilitiesConf import BunchGroupTool

# Wrapper script to provide existing tool if defined or properly configured new tool if not
def BunchGroupToolDefault(name="BunchGroupTool"):
    mlog = logging.getLogger(name)

    if hasattr(svcMgr.ToolSvc, name):
        # re-use previously configured tool
        mlog.info("BunchGroupToolDefault returning existing tool %s", name)
        return getattr(svcMgr.ToolSvc, name)

    # Instantiate new tool, by default configuration will do nothing
    bgTool = BunchGroupTool(name)

    # Now configure based on the environment
    from IOVDbSvc.CondDB import conddb

    # Run1
    if conddb.dbdata == "COMP200":

        folder = '/TRIGGER/LVL1/BunchGroupContent'
        bgTool.BunchGroupFolderName = folder

        # Mistakenly created as multi-version folder, must specify HEAD 
        from IOVDbSvc.CondDB import conddb
        if not conddb.folderRequested( folder ):
            conddb.addFolderWithTag('TRIGGER', folder, 'HEAD')
            mlog.info("BunchGroupToolDefault requested %s", folder)

    # Run2 - do nothing
    elif conddb.dbdata == "CONDBR2":
        pass

    # Unknown, complain and do nothing
    else:
        mlog.warning("BunchGroupToolDefault can't resolve conddb.dbdata = %s, assume Run2!" % conddb.dbdata)
        pass

    return bgTool


