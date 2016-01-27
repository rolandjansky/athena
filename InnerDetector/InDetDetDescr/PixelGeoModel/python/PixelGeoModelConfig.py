# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaCommon import CfgMgr

def getPixelDetectorTool(name="PixelDetectorTool", **kwargs):
    from AtlasGeoModel.InDetGMJobProperties import GeometryFlags
    if GeometryFlags.isSLHC():
        kwargs.setdefault("ServiceBuilderTool",    "InDetServMatBuilderToolSLHC");

        envelopeToolName="none"
        
        if GeometryFlags.GeoType() in ["LoIFG","LoIVFFG","ECRingFG"]:
            envelopeToolName="GeoPixelEnvelopeLoITool"
        elif GeometryFlags.GeoType() in ["Alpine"]:
            envelopeToolName="GeoPixelEnvelopeAlpineTool"
        elif GeometryFlags.GeoType() in ["BrlExt3.2_ref","BrlExt4.0_ref"]:
            envelopeToolName="GeoPixelEnvelopeExtRefTool"
        elif GeometryFlags.GeoType() in ["InclBrl4.0_ref"]:
            envelopeToolName="GeoPixelEnvelopeInclRefTool"
        print "GEOTYPE : ",GeometryFlags.GeoType()," ",envelopeToolName

        if envelopeToolName!="none":
            kwargs.setdefault("FastBuildGeoModel",True)
            kwargs.setdefault("ConfigGeoAlgTool",True)

        kwargs.setdefault("ConfigGeoBase",envelopeToolName)

    else:
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
