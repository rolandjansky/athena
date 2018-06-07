# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
# InDet GeoModel initialization
#
from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.JobProperties import jobproperties
from AthenaCommon.DetFlags      import DetFlags
from AthenaCommon.AppMgr        import ServiceMgr as svcMgr

from AtlasGeoModel.InDetGMJobProperties import GeometryFlags
if GeometryFlags.isSLHC():
    #SLHC specific code
    from AthenaCommon.AppMgr import ToolSvc

# Treat CTB separately
if ( jobproperties.Global.DetGeo() == "ctbh8" or jobproperties.Global.DetGeo() == "ctbh6" ):
    from AtlasGeoModel import InDetGMCTB

elif ( DetFlags.detdescr.ID_on() ):
    from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
    GeoModelSvc = GeoModelSvc()

    if not hasattr(svcMgr,'InDetGeometryDBSvc'):
        from GeometryDBSvc.GeometryDBSvcConf import GeometryDBSvc
        svcMgr+=GeometryDBSvc("InDetGeometryDBSvc")
        if GeometryFlags.isSLHC():
            #SLHC specific code
            # General service builder tool for SLHC
            from InDetServMatGeoModel.InDetServMatGeoModelConf import InDetServMatBuilderToolSLHC
            InDetServMatBuilderToolSLHC = InDetServMatBuilderToolSLHC()
            ToolSvc+=InDetServMatBuilderToolSLHC

    if ( DetFlags.detdescr.pixel_on() ):
        from AthenaCommon import CfgGetter
        GeoModelSvc.DetectorTools += [ CfgGetter.getPrivateTool("PixelDetectorTool", checkType=True) ]

        if (DetFlags.detdescr.BCM_on() ) :
            from AthenaCommon.AppMgr import ToolSvc
            from BCM_GeoModel.BCM_GeoModelConf import InDetDD__BCM_Builder
            bcmTool = InDetDD__BCM_Builder()
            ToolSvc += bcmTool
            GeoModelSvc.DetectorTools['PixelDetectorTool'].BCM_Tool = bcmTool

            from BLM_GeoModel.BLM_GeoModelConf import InDetDD__BLM_Builder
            blmTool = InDetDD__BLM_Builder()
            ToolSvc += blmTool
            GeoModelSvc.DetectorTools['PixelDetectorTool'].BLM_Tool = blmTool

    if ( DetFlags.detdescr.SCT_on() ):
        if GeometryFlags.isSLHC():
            #SLHC specific code
            if "GMX" == GeometryFlags.StripGeoType():
                from SCT_GeoModelXml.SCT_GeoModelXmlConf import SCT_GMX_DetectorTool
                sctSLHCTool = SCT_GMX_DetectorTool()
            else:
                from SCT_SLHC_GeoModel.SCT_SLHC_GeoModelConf import SCT_SLHC_DetectorTool
                sctSLHCTool = SCT_SLHC_DetectorTool()
                sctSLHCTool.ServiceBuilderTool = InDetServMatBuilderToolSLHC
            GeoModelSvc.DetectorTools += [ sctSLHCTool ]
            if not hasattr(svcMgr,'SCTLorentzAngleSvc'):
                from SiLorentzAngleSvc.SiLorentzAngleSvcConf import SiLorentzAngleSvc
                SCTLorentzAngleSvc = SiLorentzAngleSvc(name = "SCTLorentzAngleSvc",
                                                       SiConditionsServices = None,
                                                       UseMagFieldSvc = False,
                                                       DetectorName = "SCT")
                svcMgr+=SCTLorentzAngleSvc
        else:
            # Current atlas specific code
            from AthenaCommon import CfgGetter
            GeoModelSvc.DetectorTools += [ CfgGetter.getPrivateTool("SCT_DetectorTool", checkType=True) ]

    if ( DetFlags.detdescr.TRT_on() ):
        from TRT_GeoModel.TRT_GeoModelConf import TRT_DetectorTool
        trtDetectorTool = TRT_DetectorTool()
        if ( DetFlags.simulate.TRT_on() ):
            trtDetectorTool.DoXenonArgonMixture = True
            trtDetectorTool.DoKryptonMixture = True
        GeoModelSvc.DetectorTools += [ trtDetectorTool ]

    from InDetServMatGeoModel.InDetServMatGeoModelConf import InDetServMatTool
    if GeometryFlags.isSLHC():
        #SLHC specific code
        servMatTool = InDetServMatTool()
        GeoModelSvc.DetectorTools += [ servMatTool ]
        servMatTool.ServiceBuilderTool = InDetServMatBuilderToolSLHC
    else:
        GeoModelSvc.DetectorTools += [ InDetServMatTool() ]

    # Make alignable
    from InDetCondFolders import InDetAlignFolders
