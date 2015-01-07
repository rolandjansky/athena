# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
from AthenaCommon.SystemOfUnits import *
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

# Configuration for LivefractionTool
# Also needs LuminosityTool to be configured (should be in RecExCommon) 
def TrigLivefractionToolDefault(name='TrigLivefractionTool'):
    mlog = logging.getLogger(name)
    # mlog.warning("TrigLivefractionToolDefault called")

    # Check if tool already exists
    if hasattr(svcMgr.ToolSvc, name):
        # re-use previously configured tool
        mlog.info("TrigLivefractionToolDefault returning existing tool %s" % name)
        return getattr(svcMgr.ToolSvc, name)

    # Set up DB configuration
    from IOVDbSvc.CondDB import conddb
    

    # Add the luminosity tool as a public tool
    from LumiBlockComps.LumiBlockCompsConf import TrigLivefractionTool
    liveTool = TrigLivefractionTool(name)

    if conddb.dbdata == "COMP200":
        liveTool.DeadtimeFolderName = '/TRIGGER/LUMI/PerBcidDeadtime'
        # Mistakenly created as multi-version folder, must specify HEAD
        conddb.addFolderWithTag('TRIGGER', '/TRIGGER/LUMI/PerBcidDeadtime', 'HEAD')
        liveTool.LuminosityTool = 'LuminosityTool'

    elif conddb.dbdata == "CONDBR2":
        liveTool.DeadtimeFolderName = ''
        liveTool.LuminosityTool = ''

    else:
        mlog.warning("TrigLivefractionToolDefault can't resolve conddb.dbdata = %s, assume Run2!" % conddb.dbdata)
        liveTool.DeadtimeFolderName = ''
        liveTool.LuminosityTool = ''



    return liveTool

