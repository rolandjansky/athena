# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging
from AthenaCommon.SystemOfUnits import *
from AthenaCommon.Constants import *
from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.AppMgr import ToolSvc,ServiceMgr
from LArConditionsCommon.LArCondFlags import larCondFlags


def LArADC2MeVToolDefault (name="LArADC2MeVToolDefault", **kw): 
    mlog = logging.getLogger( 'LArADC2MeVToolDefault::__init__ ' )
    mlog.warning("The LArADC2MeVTool is deprecated! Use LArADC2MeVCondAlg!") 
    #Check if the tool exits already in ToolSvc
    if hasattr(ServiceMgr.ToolSvc,name):
        mlog.info("LArADC2MeV Tool with name" + name + "already known to ToolSvc")
        return getattr(ServiceMgr.ToolSvc,name)
    
    # call base class constructor
    from AthenaCommon import CfgMgr
    kw['name'] = name
    tool = CfgMgr.LArADC2MeVTool (**kw)


    mlog.info("entering")

    # do the configuration
    if globalflags.DataSource()=='data':
        tool.MCSym = False
        tool.UseMphysOverMcal = True
        # to be changed to True when everything is ready
        tool.UseHVScaleCorr = True

        if larCondFlags.useLArFEBGainThresholds():
            from LArRecUtils.LArFebConfigCondAlgDefault import LArFebConfigCondAlgDefault
            LArFebConfigCondAlgDefault()
            tool.UseFEBGainTresholds=True
        

    else:
        mlog.info("in MC case")
        tool.MCSym = True
        tool.UseMphysOverMcal = False
        tool.UseHVScaleCorr = False
        from AthenaCommon.AppMgr import ServiceMgr as svcMgr
        if not hasattr (svcMgr, 'IOVDbSvc'):
            svcMgr += CfgMgr.IOVDbSvc()
        for lines in svcMgr.IOVDbSvc.Folders :
            if (lines.__contains__("MphysOverMcal")) :
                mlog.info("MphysOverMcal found")
                tool.UseMphysOverMcal = True
            if (lines.__contains__("HVScaleCorr")) :
                mlog.info("HVScaleCorr found")
                tool.UseHVScaleCorr = True
                tool.MCSym = False
    return tool
