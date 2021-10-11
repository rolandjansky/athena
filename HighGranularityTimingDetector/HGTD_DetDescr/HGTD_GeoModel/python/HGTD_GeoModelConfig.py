# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentFactory import CompFactory

def HGTD_GeometryCfg( flags ):
    from AtlasGeoModel.GeoModelConfig import GeoModelCfg
    acc = GeoModelCfg( flags )
    geoModelSvc=acc.getPrimary()
    GeometryDBSvc=CompFactory.GeometryDBSvc
    acc.addService(GeometryDBSvc("InDetGeometryDBSvc"))
    HGTD_DetectorTool=CompFactory.HGTD_DetectorTool
    hgtdDetectorTool = HGTD_DetectorTool("HGTD_DetectorTool")
    hgtdDetectorTool.Alignable = False
    hgtdDetectorTool.DetectorName = "HGTD"
    hgtdDetectorTool.PrintModuleNumberPerRow = False
    geoModelSvc.DetectorTools += [ hgtdDetectorTool ]
    return acc
