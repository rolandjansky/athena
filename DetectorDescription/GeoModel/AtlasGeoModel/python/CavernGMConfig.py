#
#  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#
from AthenaConfiguration.ComponentFactory import CompFactory

def CavernGeometryCfg( flags ):
    from AtlasGeoModel.GeoModelConfig import GeoModelCfg
    acc = GeoModelCfg( flags )

    from AthenaConfiguration.Enums import ProductionStep
    if (flags.Common.ProductionStep == ProductionStep.Simulation and flags.Beam.Type == "cosmics") or flags.Detector.GeometryCavern:

        CavernInfraDetectorTool=CompFactory.CavernInfraDetectorTool
        acc.getPrimary().DetectorTools += [ CavernInfraDetectorTool() ]
    return acc
