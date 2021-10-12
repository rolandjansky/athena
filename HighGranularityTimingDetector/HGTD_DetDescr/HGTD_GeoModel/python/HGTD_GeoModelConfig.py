# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

def HGTD_GeometryCfg(flags):
    from AtlasGeoModel.GeoModelConfig import GeoModelCfg
    acc = GeoModelCfg(flags)
    geoModelSvc = acc.getPrimary()

    from AthenaConfiguration.ComponentFactory import CompFactory
    hgtdDetectorTool = CompFactory.HGTD_DetectorTool("HGTD_DetectorTool")
    hgtdDetectorTool.Alignable = False
    hgtdDetectorTool.DetectorName = "HGTD"
    hgtdDetectorTool.PrintModuleNumberPerRow = False
    geoModelSvc.DetectorTools += [ hgtdDetectorTool ]

def HGTD_ReadoutGeometryCfg(flags):
    # main GeoModel config
    acc = HGTD_GeometryCfg(flags)
    from HGTD_ConditionsAlgorithms.HGTD_ConditionsAlgorithmsConfig import HGTD_DetectorElementCondAlgCfg
    acc.merge(HGTD_DetectorElementCondAlgCfg(flags))
    return acc
