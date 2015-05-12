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
    

    tool=LArHVScaleRetriever(name)
    from AthenaCommon.GlobalFlags import globalflags 
    from AthenaCommon import CfgMgr
    if (globalflags.DataSource()!='data'):
        tool.IsMC=True
        mlog.info("%s configured for MC" % name)
    else:
        #Data case, need also LArHVCorrTool
        tool.IsMC=False
        mlog.info("%s configured for real data" % name)
        if 'LArHVCorrTool' in kw:
            tool.LArHVCorrTool= kw['LArHVCorrTool']
        else:
            from LArRecUtils.LArHVCorrToolDefault import LArHVCorrToolDefault
            theLArHVCorrTool=LArHVCorrToolDefault()
            tool.LArHVCorrTool=theLArHVCorrTool
            pass

    ToolSvc += tool
    return tool
    
