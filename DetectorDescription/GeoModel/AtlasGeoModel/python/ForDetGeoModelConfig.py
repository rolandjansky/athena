# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
from IOVDbSvc.IOVDbSvcConfig import addFoldersSplitOnline

def ForDetGeometryCfg( ConfigFlags ):
    from AtlasGeoModel.GeoModelConfig import GeoModelCfg
    acc = GeoModelCfg( ConfigFlags )
    geoModelSvc=acc.getPrimary()
    from GeometryDBSvc.GeometryDBSvcConf import GeometryDBSvc
    acc.addService(GeometryDBSvc("InDetGeometryDBSvc"))
    # LUCID
    if ConfigFlags.Detector.GeometryLucid:
        from LUCID_GeoModel.LUCID_GeoModelConf import LUCID_DetectorTool
        geoModelSvc.DetectorTools += [ LUCID_DetectorTool() ]
    # ALFA
    if ConfigFlags.Detector.GeometryALFA:
        from ALFA_GeoModel.ALFA_GeoModelConf import ALFA_DetectorTool 

        theALFA_DetectorTool=ALFA_DetectorTool(name="ALFA_DetectorTool")
        theALFA_DetectorTool.MetrologyType=3
        theALFA_DetectorTool.B7L1U_MDGeometryType = 2
        theALFA_DetectorTool.B7L1U_ODGeometryType = 2
        theALFA_DetectorTool.B7L1L_MDGeometryType = 2
        theALFA_DetectorTool.B7L1L_ODGeometryType = 2
        theALFA_DetectorTool.A7L1U_MDGeometryType = 2
        theALFA_DetectorTool.A7L1U_ODGeometryType = 2
        theALFA_DetectorTool.A7L1L_MDGeometryType = 2
        theALFA_DetectorTool.A7L1L_ODGeometryType = 2
        theALFA_DetectorTool.A7R1U_MDGeometryType = 2
        theALFA_DetectorTool.A7R1U_ODGeometryType = 2
        theALFA_DetectorTool.A7R1L_MDGeometryType = 2
        theALFA_DetectorTool.A7R1L_ODGeometryType = 2
        theALFA_DetectorTool.B7R1U_MDGeometryType = 2
        theALFA_DetectorTool.B7R1U_ODGeometryType = 2
        theALFA_DetectorTool.B7R1L_MDGeometryType = 2
        theALFA_DetectorTool.B7R1L_ODGeometryType = 2
        geoModelSvc.DetectorTools += [ theALFA_DetectorTool ]
        acc.merge(addFoldersSplitOnline(ConfigFlags,'FWD','/FWD/Onl/ALFA/position_calibration','/FWD/ALFA/position_calibration'))
    # ForwardRegion
    if ConfigFlags.Detector.GeometryFwdRegion:
        from ForwardRegionGeoModel.ForwardRegionGeoModelConf import ForwardRegionGeoModelTool
        geoModelSvc.DetectorTools += [ ForwardRegionGeoModelTool() ]
    # ZDC
    if ConfigFlags.Detector.GeometryZDC:
        from ZDC_GeoM.ZDC_GeoMConf import ZDC_DetTool
        geoModelSvc.DetectorTools += [ ZDC_DetTool() ]
    # AFP
    if ConfigFlags.Detector.GeometryAFP:
        from AFP_GeoModel.AFP_GeoModelConf import AFP_GeoModelTool
        geoModelSvc.DetectorTools += [ AFP_GeoModelTool() ]
    acc.addService(geoModelSvc)
    return acc
