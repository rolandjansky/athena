# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration 
#
# Import NSWCalibTools job properties
#
from AthenaCommon import CfgMgr

def MMCalibSmearingTool(name="MMCalibSmearingTool",extraFlags=None,**kwargs):
    return CfgMgr.Muon__NSWCalibSmearingTool(name,**kwargs)

def STgcCalibSmearingTool(name="STgcCalibSmearingTool",extraFlags=None,**kwargs):
    return CfgMgr.Muon__NSWCalibSmearingTool(name,**kwargs)

def NSWCalibTool(name="NSWCalibTool",extraFlags=None,**kwargs):
    from MuonRecExample.MuonRecTools import MuonIdHelperTool
    kwargs.setdefault("MuonIdHelperTool", MuonIdHelperTool())
    return CfgMgr.Muon__NSWCalibTool(name,**kwargs)
