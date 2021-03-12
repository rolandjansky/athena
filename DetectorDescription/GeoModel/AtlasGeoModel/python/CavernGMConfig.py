#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#

def CavernGeometryCfg( flags ):
    from AtlasGeoModel.GeoModelConfig import GeoModelCfg
    acc = GeoModelCfg( flags )
    geoModelSvc=acc.getPrimary()

    from AthenaConfiguration.Enums import ProductionStep
    if (flags.Common.ProductionStep == ProductionStep.Simulation and flags.Beam.Type == "cosmics") or flags.Detector.GeometryCavern:
        from CavernInfraGeoModel.CavernInfraGeoModelConf import CavernInfraDetectorTool
        geoModelSvc.DetectorTools += [ CavernInfraDetectorTool() ]
    return acc
