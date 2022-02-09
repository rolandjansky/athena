#
#  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#
from AthenaConfiguration.ComponentFactory import CompFactory

def CavernGeometryCfg( flags ):
    from AtlasGeoModel.GeoModelConfig import GeoModelCfg
    acc = GeoModelCfg( flags )

    from AthenaConfiguration.Enums import BeamType, ProductionStep
    if (flags.Common.ProductionStep == ProductionStep.Simulation and flags.Beam.Type is BeamType.Cosmics) or flags.Detector.GeometryCavern:

        CavernInfraDetectorTool=CompFactory.CavernInfraDetectorTool
        acc.getPrimary().DetectorTools += [ CavernInfraDetectorTool() ]
    return acc
