# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

#
# InDet GeoModel initialization
#
from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.JobProperties import jobproperties
from AthenaCommon.DetFlags      import DetFlags
from AthenaCommon.AppMgr        import ServiceMgr as svcMgr

from AtlasGeoModel.CommonGMJobProperties import CommonGeometryFlags
from AtlasGeoModel.InDetGMJobProperties import InDetGeometryFlags
if InDetGeometryFlags.isSLHC():
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

        GeoModelSvc.DetectorTools['PixelDetectorTool'].useDynamicAlignFolders = InDetGeometryFlags.useDynamicAlignFolders()

    if ( DetFlags.detdescr.SCT_on() ):

        if InDetGeometryFlags.isSLHC():
            #SLHC specific code
            if "GMX" == CommonGeometryFlags.StripGeoType():
                from SCT_GeoModelXml.SCT_GeoModelXmlConf import SCT_GMX_DetectorTool
                sctSLHCTool = SCT_GMX_DetectorTool()
            else:
                print "Error: You are using a no longer supported version of the ITK Strip geoemetry building... please check your configuration!"
                exit(1)
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

            GeoModelSvc.DetectorTools['SCT_DetectorTool'].useDynamicAlignFolders = InDetGeometryFlags.useDynamicAlignFolders()

    if ( DetFlags.detdescr.TRT_on() ):
        from TRT_GeoModel.TRT_GeoModelConf import TRT_DetectorTool
        trtDetectorTool = TRT_DetectorTool()
        if ( DetFlags.simulate.TRT_on() ):
            trtDetectorTool.DoXenonArgonMixture = True
            trtDetectorTool.DoKryptonMixture = True
        trtDetectorTool.useDynamicAlignFolders = InDetGeometryFlags.useDynamicAlignFolders()
        GeoModelSvc.DetectorTools += [ trtDetectorTool ]

    if ( DetFlags.detdescr.HGTD_on() ) :
        GeoModelSvc.DetectorTools += [ CfgGetter.getPrivateTool("HGTD_DetectorTool", checkType=True)  ]


    if not InDetGeometryFlags.isSLHC():
     #ITK uses a different method in InDetDetDesr/PixelLayouts for building services
     from InDetServMatGeoModel.InDetServMatGeoModelConf import InDetServMatTool
     GeoModelSvc.DetectorTools += [ InDetServMatTool() ]

    # Make alignable
    from InDetCondFolders import InDetAlignFolders
