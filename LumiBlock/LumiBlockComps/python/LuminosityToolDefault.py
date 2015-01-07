# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
from AthenaCommon.SystemOfUnits import *
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

from LumiBlockComps.LumiBlockCompsConf import LuminosityTool

# Wrapper script to provide existing offline tool if defined or new tool if not
def LuminosityToolDefault(name="LuminosityTool"):
    mlog = logging.getLogger(name)

    if hasattr(svcMgr.ToolSvc, name):
        # re-use previously supported tool
        mlog.info("LuminosityToolDefault returning existing tool %s" % name)
        return getattr(svcMgr.ToolSvc, name)

    from IOVDbSvc.CondDB import conddb
    if conddb.dbdata == "COMP200":
        return LuminosityToolOfflineRun1(name)

    elif conddb.dbdata == "CONDBR2":
        return LuminosityToolOfflineRun2(name)

    else:
        mlog.warning("LuminosityToolDefault can't resolve conddb.dbdata = %s, assume Run2!" % conddb.dbdata)
        return LuminosityToolOfflineRun2(name)

# Configuration for offline default tool used in Run1
# Change logic so that folders names are blank by default and must be configured
def LuminosityToolOfflineRun1(name="LuminosityTool"):
        
    mlog = logging.getLogger(name)

    # Instantiate new tool
    lumiTool = LuminosityTool(name)

    # Now configure DB based on the environment
    from IOVDbSvc.CondDB import conddb
    from InDetRecExample.InDetJobProperties import InDetFlags

    # Check if this is express stream or bulk
    if not InDetFlags.useBeamConstraint():
        lumiFolder  = "/TRIGGER/LUMI/LBLESTONL"
        if not conddb.folderRequested( lumiFolder ):
            conddb.addFolder('TRIGGER_ONL', lumiFolder)
            mlog.info("LuminosityToolDefault requested %s", lumiFolder)

    else:
        lumiFolder = "/TRIGGER/OFLLUMI/LBLESTOFL"
        if not conddb.folderRequested( lumiFolder ):
            conddb.addFolder('TRIGGER_OFL', lumiFolder)
            mlog.info("LuminosityToolDefault requested %s", lumiFolder)

    lumiTool.LumiFolderName = lumiFolder

    # Other folders needed by LuminosityTool
    folder = "/TRIGGER/LUMI/LBLB"
    if not conddb.folderRequested( folder ):
        conddb.addFolder('TRIGGER', folder)
        mlog.info("LuminosityToolDefault requested %s", folder)

    lumiTool.LBLBFolderName = folder


    # Configure tools
    toolName = "FillParamsTool"
    lumiTool.FillParamsTool = toolName
    if not hasattr(svcMgr.ToolSvc, toolName):
        from CoolLumiUtilities.FillParamsToolDefault import FillParamsToolDefault
        svcMgr.ToolSvc += FillParamsToolDefault(toolName)
        mlog.info("LuminosityToolDefault added tool %s", toolName)

    toolName = "BunchLumisTool"
    lumiTool.BunchLumisTool = toolName
    if not hasattr(svcMgr.ToolSvc, toolName):
        from CoolLumiUtilities.BunchLumisToolDefault import BunchLumisToolDefault
        svcMgr.ToolSvc += BunchLumisToolDefault(toolName)
        mlog.info("LuminosityToolDefault added tool %s", toolName)

    toolName = "BunchGroupTool"
    lumiTool.BunchGroupTool = toolName
    if not hasattr(svcMgr.ToolSvc, toolName):
        from CoolLumiUtilities.BunchGroupToolDefault import BunchGroupToolDefault
        svcMgr.ToolSvc += BunchGroupToolDefault(toolName)
        mlog.info("LuminosityToolDefault added tool %s", toolName)

    toolName = "OnlineLumiCalibrationTool"
    lumiTool.OnlineLumiCalibrationTool = toolName
    if not hasattr(svcMgr.ToolSvc, toolName):
        from CoolLumiUtilities.OnlineLumiCalibrationToolDefault import OnlineLumiCalibrationToolDefault
        svcMgr.ToolSvc += OnlineLumiCalibrationToolDefault(toolName)
        mlog.info("LuminosityToolDefault added tool %s", toolName)


    mlog.info("Created Run1 %s using folder %s" % (name, lumiFolder))
    return lumiTool

# Configuration for offline default tool used in Run2
def LuminosityToolOfflineRun2(name="LuminosityTool"):
        
    mlog = logging.getLogger(name)

    # Set up DB configuration
    #from IOVDbSvc.CondDB import conddb
    #from InDetRecExample.InDetJobProperties import InDetFlags

    # Will eventually use our new online/offline folder, but not ready
    # Set up with dummy configuration for now (this is also now the default)
    lumiTool = LuminosityTool(name)
    lumiTool.LBLBFolderName = ''

    lumiTool.LumiFolderName = ''
    lumiTool.FillParamsTool = ''
    lumiTool.BunchLumisTool = ''
    lumiTool.BunchGroupTool = ''
    lumiTool.OnlineLumiCalibrationTool = ''
    
    mlog.info("Created Run2 %s using a dummy Run2 configuration!" % name)

    return lumiTool

class LuminosityToolOnline(LuminosityTool):
    """LuminosityTool for use in the online/HLT"""
    
    __slots__ = []
    def __init__(self, name='LuminosityTool'):
        super (LuminosityToolOnline, self).__init__(name)

        mlog = logging.getLogger(name)
        
        from IOVDbSvc.CondDB import conddb
        if conddb.dbdata == "COMP200": # Run1
            folder  = "/TRIGGER/LUMI/LBLESTONL"
            conddb.addFolder('TRIGGER_ONL', folder)
            #conddb.addFolder('TDAQ', '/TDAQ/OLC/CALIBRATIONS')
            #conddb.addFolder('TDAQ', '/TDAQ/OLC/BUNCHLUMIS')

            self.LumiFolderName = folder
            self.LBLBFolderName = ''

            self.FillParamsTool = ''
            self.BunchLumisTool = ''
            self.BunchGroupTool = ''
            self.OnlineLumiCalibrationTool = ''
        
            mlog.info("Created online %s using folder %s" % (name, folder))

        
        elif conddb.dbdata == "CONDBR2": # Run2
            # Folder names are now blank by default
            mlog.info("Created online %s using a dummy Run2 configuration!" % name)

        else:
            mlog.warning("LuminosityToolDefault can't resolve conddb.dbdata = %s, assume Run2!" % conddb.dbdata)
            mlog.info("Created online %s using a dummy Run2 configuration!" % name)



