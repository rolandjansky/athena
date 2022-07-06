# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import ProductionStep
from AtlasGeoModel.GeoModelConfig import GeoModelCfg

def TileGMCfg(configFlags):
    result=GeoModelCfg(configFlags)

    TileDetectorTool=CompFactory.TileDetectorTool
    result.getPrimary().DetectorTools += [ TileDetectorTool() ]
    if configFlags.Common.ProductionStep != ProductionStep.Simulation and configFlags.Common.ProductionStep != ProductionStep.FastChain:
        result.getPrimary().DetectorTools["TileDetectorTool"].GeometryConfig = "RECO"

    return result


if __name__ == "__main__":
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.TestDefaults import defaultTestFiles

    ConfigFlags.Input.Files = defaultTestFiles.RAW
    ConfigFlags.lock()

    acc = TileGMCfg( ConfigFlags )
    acc.store( open( "test.pkl", "wb" ) )
    print("All OK")
