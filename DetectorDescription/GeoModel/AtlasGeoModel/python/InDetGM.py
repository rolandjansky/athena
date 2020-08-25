# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

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
            GeoModelSvc.DetectorTools['PixelDetectorTool'].BCM_Tool = bcmTool

            from BLM_GeoModel.BLM_GeoModelConf import InDetDD__BLM_Builder
            blmTool = InDetDD__BLM_Builder()
            GeoModelSvc.DetectorTools['PixelDetectorTool'].BLM_Tool = blmTool

        GeoModelSvc.DetectorTools['PixelDetectorTool'].useDynamicAlignFolders = InDetGeometryFlags.useDynamicAlignFolders()

    if ( DetFlags.detdescr.SCT_on() ):
        if InDetGeometryFlags.isSLHC():
            #SLHC specific code
            if "GMX" == CommonGeometryFlags.StripGeoType():
                from SCT_GeoModelXml.SCT_GeoModelXmlConf import SCT_GMX_DetectorTool
                sctSLHCTool = SCT_GMX_DetectorTool()
            else:
                from SCT_SLHC_GeoModel.SCT_SLHC_GeoModelConf import SCT_SLHC_DetectorTool
                sctSLHCTool = SCT_SLHC_DetectorTool()
                sctSLHCTool.ServiceBuilderTool = InDetServMatBuilderToolSLHC
            GeoModelSvc.DetectorTools += [ sctSLHCTool ]
        else:
            # Current atlas specific code
            from AthenaCommon import CfgGetter
            GeoModelSvc.DetectorTools += [ CfgGetter.getPrivateTool("SCT_DetectorTool", checkType=True) ]

        GeoModelSvc.DetectorTools['SCT_DetectorTool'].useDynamicAlignFolders = InDetGeometryFlags.useDynamicAlignFolders()

    if ( DetFlags.detdescr.TRT_on() ):
        from TRT_GeoModel.TRT_GeoModelConf import TRT_DetectorTool
        trtDetectorTool = TRT_DetectorTool()
        trtDetectorTool.useDynamicAlignFolders = InDetGeometryFlags.useDynamicAlignFolders()
        GeoModelSvc.DetectorTools += [ trtDetectorTool ]

    from InDetServMatGeoModel.InDetServMatGeoModelConf import InDetServMatTool
    if InDetGeometryFlags.isSLHC():
        #SLHC specific code
        servMatTool = InDetServMatTool()
        GeoModelSvc.DetectorTools += [ servMatTool ]
        servMatTool.ServiceBuilderTool = InDetServMatBuilderToolSLHC
    else:
        GeoModelSvc.DetectorTools += [ InDetServMatTool() ]

    # Make alignable
    from InDetCondFolders import InDetAlignFolders

