# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr

def getPixelDetectorTool(name="PixelDetectorTool", **kwargs):
    from AtlasGeoModel.InDetGMJobProperties import InDetGeometryFlags
    if not InDetGeometryFlags.isSLHC():
      #ITK uses a different method in InDetDetDesr/PixelLayouts for building services
     kwargs.setdefault("ServiceBuilderTool",    "");
    from AthenaCommon.DetFlags      import DetFlags
    if DetFlags.digitize.pixel_on() or DetFlags.haveRDO.pixel_on() or DetFlags.haveRIO.pixel_on():
        # PixelLorentzAngleSvc needed for digi and reco
        from AthenaCommon.AppMgr        import ServiceMgr
        if not hasattr(ServiceMgr,'PixelLorentzAngleSvc'):
            from SiLorentzAngleSvc.SiLorentzAngleSvcConf import SiLorentzAngleSvc
            PixelLorentzAngleSvc = SiLorentzAngleSvc(name = "PixelLorentzAngleSvc",
                                                     SiConditionsServices = None,
                                                     UseMagFieldSvc = False,
                                                     DetectorName = "Pixel")
            ServiceMgr+=PixelLorentzAngleSvc
        kwargs.setdefault("LorentzAngleSvc",  "PixelLorentzAngleSvc");
    else:
        # PixelLorentzAngleSvc not needed for simulation
        kwargs.setdefault("LorentzAngleSvc",  "");
    return CfgMgr.PixelDetectorTool(name, **kwargs)
