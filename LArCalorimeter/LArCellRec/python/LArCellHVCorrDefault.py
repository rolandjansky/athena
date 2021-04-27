# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

#Note: This tool is used by CaloCellCorrection tool
def LArCellHVCorrDefault(name="LArCellHVCorrDefault"):
    # check if tool already exists
    from AthenaCommon.AppMgr import ServiceMgr as svcMgr
    if hasattr(svcMgr.ToolSvc, name):
        # re-use previously configured (public) tool
        return getattr(svcMgr.ToolSvc, name)
    
    
    from LArConditionsCommon import LArHVDB  # noqa: F401

    from LArCellRec.LArCellRecConf import LArCellHVCorr
    theLArCellHVCorr = LArCellHVCorr(name)

    svcMgr.ToolSvc += theLArCellHVCorr
    return theLArCellHVCorr
