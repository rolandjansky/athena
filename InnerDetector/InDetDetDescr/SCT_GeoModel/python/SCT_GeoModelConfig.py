# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr

def getSCT_DetectorTool(name="SCT_DetectorTool", **kwargs):
    kwargs.setdefault("DetectorName",     "SCT");
    kwargs.setdefault("InitialLayout",    False);
    kwargs.setdefault("Alignable",        True);
    kwargs.setdefault("RDBAccessSvc",     "RDBAccessSvc");
    kwargs.setdefault("GeometryDBSvc",    "InDetGeometryDBSvc");
    kwargs.setdefault("GeoDbTagSvc",      "GeoDbTagSvc");
    from AthenaCommon.DetFlags      import DetFlags
    if DetFlags.digitize.SCT_on() or DetFlags.haveRDO.SCT_on() or DetFlags.haveRIO.SCT_on():
        # SCTLorentzAngleTool needed for digi and reco
        from AthenaCommon.AppMgr import ToolSvc
        if not hasattr(ToolSvc, "SCTLorentzAngleTool"):
            from SiLorentzAngleSvc.SCTLorentzAngleToolSetup import SCTLorentzAngleToolSetup
            sctLorentzAngleToolSetup = SCTLorentzAngleToolSetup()
        kwargs.setdefault("LorentzAngleTool", ToolSvc.SCTLorentzAngleTool)
    else:
        # SCTLorentzAngleTool not needed for simulation
        kwargs.setdefault("LorentzAngleTool", "")
    return CfgMgr.SCT_DetectorTool(name, **kwargs)
