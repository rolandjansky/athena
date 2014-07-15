# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#
# InDet GeoModel initialization
#
from AthenaCommon.GlobalFlags import globalflags
from AthenaCommon.JobProperties import jobproperties
from AthenaCommon.DetFlags      import DetFlags
from AthenaCommon.AppMgr        import ServiceMgr as svcMgr

if "ATLAS-SLHC" in globalflags.DetDescrVersion():
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
        if "ATLAS-SLHC" in globalflags.DetDescrVersion():
           #SLHC specific code 
           # General service builder tool for SLHC
           from InDetServMatGeoModel.InDetServMatGeoModelConf import InDetServMatBuilderToolSLHC
           InDetServMatBuilderToolSLHC = InDetServMatBuilderToolSLHC()
           ToolSvc+=InDetServMatBuilderToolSLHC
    
    if ( DetFlags.detdescr.pixel_on() ):
        from PixelGeoModel.PixelGeoModelConf import PixelDetectorTool
        pixelTool =  PixelDetectorTool()
        GeoModelSvc.DetectorTools += [ pixelTool ]
        if "ATLAS-SLHC" in globalflags.DetDescrVersion():
           #SLHC specific code 
           pixelTool.ServiceBuilderTool = InDetServMatBuilderToolSLHC

        if not hasattr(svcMgr,'PixelLorentzAngleSvc'):
            from SiLorentzAngleSvc.SiLorentzAngleSvcConf import SiLorentzAngleSvc
            PixelLorentzAngleSvc = SiLorentzAngleSvc(name = "PixelLorentzAngleSvc",
                                                     SiConditionsServices = None,
                                                     UseMagFieldSvc = False,
                                                     DetectorName = "Pixel")
            svcMgr+=PixelLorentzAngleSvc
            
        if (DetFlags.detdescr.BCM_on() ) :
            from AthenaCommon.AppMgr import ToolSvc
            from BCM_GeoModel.BCM_GeoModelConf import InDetDD__BCM_Builder
            bcmTool = InDetDD__BCM_Builder()
            ToolSvc += bcmTool
            pixelTool.BCM_Tool = bcmTool 

            from BLM_GeoModel.BLM_GeoModelConf import InDetDD__BLM_Builder
            blmTool = InDetDD__BLM_Builder()
            ToolSvc += blmTool
            pixelTool.BLM_Tool = blmTool

    if ( DetFlags.detdescr.SCT_on() ):
        if "ATLAS-SLHC" in globalflags.DetDescrVersion():
           #SLHC specific code 
           from SCT_SLHC_GeoModel.SCT_SLHC_GeoModelConf import SCT_SLHC_DetectorTool
           sctSLHCTool = SCT_SLHC_DetectorTool()
           GeoModelSvc.DetectorTools += [ sctSLHCTool ]
           sctSLHCTool.ServiceBuilderTool = InDetServMatBuilderToolSLHC
        else:
           # Current atlas specific code 
           from SCT_GeoModel.SCT_GeoModelConf import SCT_DetectorTool
           GeoModelSvc.DetectorTools += [ SCT_DetectorTool() ]

        if not hasattr(svcMgr,'SCTLorentzAngleSvc'):
            from SiLorentzAngleSvc.SiLorentzAngleSvcConf import SiLorentzAngleSvc
            SCTLorentzAngleSvc = SiLorentzAngleSvc(name = "SCTLorentzAngleSvc",
                                                   SiConditionsServices = None,
                                                   UseMagFieldSvc = False,
                                                   DetectorName = "SCT")
            svcMgr+=SCTLorentzAngleSvc        
                
    if ( DetFlags.detdescr.TRT_on() ):
        from TRT_GeoModel.TRT_GeoModelConf import TRT_DetectorTool
        GeoModelSvc.DetectorTools += [ TRT_DetectorTool() ]

    from InDetServMatGeoModel.InDetServMatGeoModelConf import InDetServMatTool
    if "ATLAS-SLHC" in globalflags.DetDescrVersion():
        #SLHC specific code 
        servMatTool = InDetServMatTool()
        GeoModelSvc.DetectorTools += [ servMatTool ]
        servMatTool.ServiceBuilderTool = InDetServMatBuilderToolSLHC
    else:
        GeoModelSvc.DetectorTools += [ InDetServMatTool() ]
       
    # Make alignable 
    from InDetCondFolders import InDetAlignFolders

