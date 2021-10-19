# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
from IOVDbSvc.IOVDbSvcConfig import addFoldersSplitOnline
from AthenaConfiguration.ComponentFactory import CompFactory

def ForDetGeometryCfg( ConfigFlags ):
    from AtlasGeoModel.GeoModelConfig import GeoModelCfg
    acc = GeoModelCfg( ConfigFlags )
    geoModelSvc=acc.getPrimary()
    # LUCID
    if ConfigFlags.Detector.GeometryLucid:
        LUCID_DetectorTool=CompFactory.LUCID_DetectorTool
        geoModelSvc.DetectorTools += [ LUCID_DetectorTool() ]
    # ALFA
    if ConfigFlags.Detector.GeometryALFA:
        #from ALFA_GeoModel.ALFA_GeoModelConf import ALFA_DetectorTool 
        ALFA_DetectorTool=CompFactory.ALFA_DetectorTool
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
        ForwardRegionGeoModelTool=CompFactory.ForwardRegionGeoModelTool
        geoModelSvc.DetectorTools += [ ForwardRegionGeoModelTool() ]
    # ZDC
    if ConfigFlags.Detector.GeometryZDC:
        ZDC_DetTool=CompFactory.ZDC_DetTool
        geoModelSvc.DetectorTools += [ ZDC_DetTool() ]
    # AFP
    if ConfigFlags.Detector.GeometryAFP:
        AFP_GeoModelTool=CompFactory.AFP_GeoModelTool
        geoModelSvc.DetectorTools += [ AFP_GeoModelTool() ]
    acc.addService(geoModelSvc)
    return acc
