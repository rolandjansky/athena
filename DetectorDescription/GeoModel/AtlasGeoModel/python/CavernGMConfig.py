#
#  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

def CavernGeometryCfg( flags ):
    from AtlasGeoModel.GeoModelConfig import GeoModelCfg
    acc = GeoModelCfg( flags )
    geoModelSvc=acc.getPrimary()
    if (flags.Detector.Simulate and flags.Beam.Type == "cosmics") or flags.Detector.SimulateCavern:
        from CavernInfraGeoModel.CavernInfraGeoModelConf import CavernInfraDetectorTool
        geoModelSvc.DetectorTools += [ CavernInfraDetectorTool() ]
    return acc
