# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

def HGTD_GeoModelCfg(flags):
    from AtlasGeoModel.GeoModelConfig import GeoModelCfg
    acc = GeoModelCfg(flags)
    geoModelSvc = acc.getPrimary()

    from AthenaConfiguration.ComponentFactory import CompFactory
    hgtdDetectorTool = CompFactory.HGTD_GMX_DetectorTool()
    hgtdDetectorTool.Alignable = False
    hgtdDetectorTool.DetectorName = "HGTD"
    if flags.HGTD.Geometry.isLocal:
        hgtdDetectorTool.GmxFilename = flags.HGTD.Geometry.Filename
    if flags.HGTD.Geometry.ClobOutputName:
        hgtdDetectorTool.ClobOutputName = flags.HGTD.Geometry.ClobOutputName
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
