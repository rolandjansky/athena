# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#from AthenaCommon.Logging import logging 

#Note: This tool is used as CaloCellCorrection tool and by the CaloNoiseTool
def LArCellHVCorrDefault(name="LArCellHVCorrDefault"):
     # check if tool already exists
    from AthenaCommon.AppMgr import ServiceMgr as svcMgr
    if hasattr(svcMgr.ToolSvc, name):
        # re-use previously configured (public) tool
        return getattr(svcMgr.ToolSvc, name)
    
    
    from LArRecUtils.LArHVCorrToolDefault import LArHVCorrToolDefault
    theLArHVCorrTool = LArHVCorrToolDefault()
    svcMgr.ToolSvc += theLArHVCorrTool

    from LArCellRec.LArCellRecConf import LArCellHVCorr
    theLArCellHVCorr = LArCellHVCorr(name)
    theLArCellHVCorr.HVCorrTool = theLArHVCorrTool
    
    svcMgr.ToolSvc += theLArCellHVCorr
    return theLArCellHVCorr
