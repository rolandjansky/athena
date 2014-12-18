# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon.Logging import logging


# import the base class
from LArRecUtils.LArRecUtilsConf import LArHVScaleRetriever

def LArHVScaleRetrieverDefault(name="LArHVScaleRetrieverDefault", **kw):
    from AthenaCommon.AppMgr import ToolSvc

    mlog = logging.getLogger( 'LArHVScaleRetrieverDefault' )
    #Check if the tool exits already in ToolSvc
    if hasattr(ToolSvc,name):
        mlog.info("LArHVScaleRetriever with name "+ name+" already known to ToolSvc")
        return getattr(ToolSvc,name)
    

    # call base class constructor
    from AthenaCommon import CfgMgr
    from AthenaCommon.GlobalFlags import globalflags 

    # if not provided in kw, get the defaut LArHVCorrTool
    if 'LArHVCorrTool' not in kw:
        from LArRecUtils.LArHVCorrToolDefault import LArHVCorrToolDefault
        kw['LArHVCorrTool'] = LArHVCorrToolDefault()
    kw['name'] = name
    kw.setdefault('IsMC', globalflags.DataSource()!='data' )
    tool = CfgMgr.LArHVScaleRetriever(**kw)

    mlog.info('configured for MC : '+ str(kw['IsMC']))

    ToolSvc += tool
    return tool
    
