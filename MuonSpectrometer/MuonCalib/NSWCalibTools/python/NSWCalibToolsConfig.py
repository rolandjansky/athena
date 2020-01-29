# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration 
#
# Import NSWCalibTools job properties
#
from AthenaCommon import CfgMgr

def MMCalibSmearingTool(name="MMCalibSmearingTool",extraFlags=None,**kwargs):
    return CfgMgr.Muon__NSWCalibSmearingTool(name,**kwargs)

def STgcCalibSmearingTool(name="STgcCalibSmearingTool",extraFlags=None,**kwargs):
    return CfgMgr.Muon__NSWCalibSmearingTool(name,**kwargs)


