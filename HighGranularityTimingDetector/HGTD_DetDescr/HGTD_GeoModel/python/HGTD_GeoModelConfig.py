# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import ProductionStep

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
    
    # Alignment corrections and DetElements to conditions
    if flags.Common.Project != "AthSimulation" and (flags.Common.ProductionStep != ProductionStep.Simulation or flags.Overlay.DataOverlay):
        from HGTD_ConditionsAlgorithms.HGTD_ConditionsAlgorithmsConfig import HGTD_DetectorElementCondAlgCfg
        acc.merge(HGTD_DetectorElementCondAlgCfg(flags))

    return acc
