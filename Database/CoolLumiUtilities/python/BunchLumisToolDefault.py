# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
from AthenaCommon.SystemOfUnits import *
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

from CoolLumiUtilities.CoolLumiUtilitiesConf import BunchLumisTool

# Wrapper script to provide existing tool if defined or properly configured new tool if not
def BunchLumisToolDefault(name="BunchLumisTool"):
    mlog = logging.getLogger(name)

    if hasattr(svcMgr.ToolSvc, name):
        # re-use previously configured tool
        mlog.info("BunchLumisToolDefault returning existing tool %s", name)
        return getattr(svcMgr.ToolSvc, name)

    # Instantiate new tool, by default configuration will do nothing
    blTool = BunchLumisTool(name)

    # Now configure based on the environment
    from IOVDbSvc.CondDB import conddb

    # Run1
    if conddb.dbdata == "COMP200":

        folder = '/TDAQ/OLC/BUNCHLUMIS'
        blTool.BunchLumisFolderName = folder

        from IOVDbSvc.CondDB import conddb
        if not conddb.folderRequested( folder ):
            from AthenaCommon.GlobalFlags import globalflags
            if globalflags.isOverlay():
                # Load reduced channel list for overlay jobs to try to reduce COOL access
                # Need Lucid AND, OR, HitOR, BcmH OR, BcmV OR
                conddb.addFolder('TDAQ', '<channelSelection>101,102,103,201,211</channelSelection> /TDAQ/OLC/BUNCHLUMIS')

            else:
                conddb.addFolder('TDAQ', folder)

            mlog.info("BunchLumisToolDefault requested %s", folder)

        # Also take care of the FillParamsTool 
        fpToolName = "FillParamsTool"
        blTool.FillParamsTool = fpToolName

        # Make sure tool exists also
        if not hasattr(svcMgr.ToolSvc, fpToolName):
            from CoolLumiUtilities.FillParamsToolDefault import FillParamsToolDefault
            svcMgr.ToolSvc += FillParamsToolDefault(fpToolName)
            mlog.info("BunchLumisToolDefault added tool %s", fpToolName)

    # Run2 - do nothing
    elif conddb.dbdata == "CONDBR2":
        pass

    # Unknown, complain and do nothing
    else:
        mlog.warning("BunchLumisToolDefault can't resolve conddb.dbdata = %s, assume Run2!" % conddb.dbdata)
        pass

    return blTool


