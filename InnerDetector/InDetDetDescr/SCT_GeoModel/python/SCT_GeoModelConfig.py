# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr

def getSCT_DetectorTool(name="SCT_DetectorTool", **kwargs):
    kwargs.setdefault("DetectorName",     "SCT");
    kwargs.setdefault("InitialLayout",    False);
    kwargs.setdefault("Alignable",        True);
    kwargs.setdefault("RDBAccessSvc",     "RDBAccessSvc");
    kwargs.setdefault("GeometryDBSvc",    "InDetGeometryDBSvc");
    kwargs.setdefault("GeoModelSvc",      "GeoModelSvc");
    from AthenaCommon.DetFlags      import DetFlags
    if DetFlags.digitize.SCT_on() or DetFlags.haveRDO.SCT_on() or DetFlags.haveRIO.SCT_on():
        # SCTLorentzAngleSvc needed for digi and reco
        from AthenaCommon.AppMgr        import ServiceMgr
        if not hasattr(ServiceMgr,'SCTLorentzAngleSvc'):
            from SiLorentzAngleSvc.SiLorentzAngleSvcConf import SiLorentzAngleSvc
            SCTLorentzAngleSvc = SiLorentzAngleSvc(name = "SCTLorentzAngleSvc",
                                                   SiConditionsServices = None,
                                                   UseMagFieldSvc = False,
                                                   DetectorName = "SCT")
            ServiceMgr+=SCTLorentzAngleSvc
        kwargs.setdefault("LorentzAngleSvc",  "SCTLorentzAngleSvc");
    else:
        # SCTLorentzAngleSvc not needed for simulation
        kwargs.setdefault("LorentzAngleSvc",  "");
    return CfgMgr.SCT_DetectorTool(name, **kwargs)
