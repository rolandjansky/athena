# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr

def getHGTD_DetectorTool(name="HGTD_DetectorTool", **kwargs):
    kwargs.setdefault( "DetectorName",     "HGTD");
    kwargs.setdefault( "Alignable",        True);
    kwargs.setdefault( "RDBAccessSvc",     "RDBAccessSvc");
    kwargs.setdefault( "GeometryDBSvc",    "InDetGeometryDBSvc");
    kwargs.setdefault( "GmxFilename", "" );
    kwargs.setdefault( "GeoModelSvc",      "GeoModelSvc");
    kwargs.setdefault( "PrintModuleNumberPerRow",      False );
    from AthenaCommon.DetFlags      import DetFlags
    if DetFlags.digitize.HGTD_on() or DetFlags.haveRDO.HGTD_on() or DetFlags.haveRIO.HGTD_on():
        # HGTDLorentzAngleSvc needed for digi and reco
        from AthenaCommon.AppMgr        import ServiceMgr
        if not hasattr(ServiceMgr,'HGTDLorentzAngleSvc'):
            from SiLorentzAngleSvc.SiLorentzAngleSvcConf import SiLorentzAngleSvc
            HGTDLorentzAngleSvc = SiLorentzAngleSvc(  name = "HGTDLorentzAngleSvc",
                                                      SiConditionsServices = None,
                                                      UseMagFieldSvc = False,
                                                      DetectorName = "HGTD")
            ServiceMgr+=HGTDLorentzAngleSvc
        kwargs.setdefault("LorentzAngleSvc",  "HGTDLorentzAngleSvc");
    else:
        # LorentzAngleSvc not needed for simulation
        kwargs.setdefault( "LorentzAngleSvc",  "" );
    return CfgMgr.HGTD_DetectorTool( name, **kwargs )
