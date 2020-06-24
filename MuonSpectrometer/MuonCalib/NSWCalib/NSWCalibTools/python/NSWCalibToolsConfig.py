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
    from AthenaCommon.AppMgr import ToolSvc
    if hasattr(ToolSvc,"MuonIdHelperTool"):
        kwargs.setdefault("MuonIdHelperTool", getattr(ToolSvc,"MuonIdHelperTool"))
    else:
        from AtlasGeoModel.MuonGMJobProperties import MuonGeometryFlags
        from MuonIdHelpers.MuonIdHelpersConf import Muon__MuonIdHelperTool
        muIdHelp = Muon__MuonIdHelperTool("MuonIdHelperTool")
        muIdHelp.HasCSC=MuonGeometryFlags.hasCSC()
        muIdHelp.HasSTgc=MuonGeometryFlags.hasSTGC()
        muIdHelp.HasMM=MuonGeometryFlags.hasMM()
        ToolSvc += muIdHelp
        kwargs.setdefault("MuonIdHelperTool", muIdHelp)
    return CfgMgr.Muon__NSWCalibTool(name,**kwargs)
