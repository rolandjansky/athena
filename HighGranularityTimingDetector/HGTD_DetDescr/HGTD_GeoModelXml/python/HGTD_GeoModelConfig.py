# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

def HGTD_GeoModelCfg(flags):
    from AtlasGeoModel.GeoModelConfig import GeoModelCfg
    acc = GeoModelCfg(flags)
    geoModelSvc = acc.getPrimary()

    from AthenaConfiguration.ComponentFactory import CompFactory
    GeometryDBSvc=CompFactory.GeometryDBSvc
    acc.addService(GeometryDBSvc("InDetGeometryDBSvc"))
    HGTD_GMX_DetectorTool=CompFactory.HGTD_GMX_DetectorTool
    hgtdDetectorTool = HGTD_GMX_DetectorTool("HGTD_GMX_DetectorTool")
    hgtdDetectorTool.Alignable = False
    hgtdDetectorTool.DetectorName = "HGTD"
    hgtdDetectorTool.GmxFilename = "HGTD.gmx"
    geoModelSvc.DetectorTools += [ hgtdDetectorTool ]
    return acc

def HGTD_SimulationGeometryCfg(flags):
    # main GeoModel config
    acc = HGTD_GeoModelCfg(flags)
    return acc


def HGTD_ReadoutGeometryCfg(flags):
    # main GeoModel config
    acc = HGTD_GeoModelCfg(flags)
    from HGTD_ConditionsAlgorithms.HGTD_ConditionsAlgorithmsConfig import HGTD_DetectorElementCondAlgCfg
    acc.merge(HGTD_DetectorElementCondAlgCfg(flags))
    return acc