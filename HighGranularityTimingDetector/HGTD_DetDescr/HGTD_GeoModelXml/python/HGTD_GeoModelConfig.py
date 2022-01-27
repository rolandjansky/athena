# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentFactory import CompFactory

def HGTD_GeometryCfg( flags ):
    from AtlasGeoModel.GeoModelConfig import GeoModelCfg
    acc = GeoModelCfg( flags )
    geoModelSvc=acc.getPrimary()
    GeometryDBSvc=CompFactory.GeometryDBSvc
    acc.addService(GeometryDBSvc("InDetGeometryDBSvc"))
    HGTD_GMX_DetectorTool=CompFactory.HGTD_GMX_DetectorTool
    hgtdDetectorTool = HGTD_GMX_DetectorTool("HGTD_GMX_DetectorTool")
    hgtdDetectorTool.Alignable = False
    hgtdDetectorTool.DetectorName = "HGTD"
    hgtdDetectorTool.GmxFilename = "HGTD.gmx"
    geoModelSvc.DetectorTools += [ hgtdDetectorTool ]
    return acc
