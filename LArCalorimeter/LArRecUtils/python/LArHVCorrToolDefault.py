# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
from AthenaCommon.AppMgr import ServiceMgr

# import the base class
from LArRecUtils.LArRecUtilsConf import LArHVCorrTool

def LArHVCorrToolDefault (name="LArHVCorrToolDefault", **kw):
    mlog = logging.getLogger( 'LArHVCorrToolDefault::__init__ ' )
    #Check if the tool exits already in ToolSvc
    if hasattr(ServiceMgr.ToolSvc,name):
        mlog.info("LArHVCorrTool with name "+ name+" already known to ToolSvc")
        return getattr(ServiceMgr.ToolSvc,name)
    

    # call base class constructor
    from AthenaCommon import CfgMgr
    kw['name'] = name
    tool = CfgMgr.LArHVCorrTool (**kw)


    mlog.info("entering")

    from AthenaCommon.GlobalFlags import globalflags 

    # do the configuration
    if globalflags.DataSource()=='data':
        from LArConditionsCommon import LArHVDB
        from LArCondUtils.LArCondUtilsConf import LArHVToolDB
        theLArHVToolDB = LArHVToolDB("LArHVToolDB")
        from AthenaCommon.AppMgr import ToolSvc
        ToolSvc += theLArHVToolDB

        tool.HVTool = theLArHVToolDB
        tool.doTdrift = False
        tool.DeltaTupdate = 0 # seconds (if 0, no time-dependent update applied)
        #tool.OutputLevel = 2 # 2 = DEBUG 

        # example to force some HV corection factors for some regions
        # tool.fixHVCorr = ["1 1 3 2.1 2.3 0.1963 0.3926 2.","1 1 3 2.1 2.3 0.7854 0.98175 2.","1 1 3 2.1 2.3 1.7672 1.9635 2.","1 1 3 -2.5 -2.3 1.3745 1.5708 2."]

    else:
        mlog.info("in MC case")

    ToolSvc+=tool
    return tool
