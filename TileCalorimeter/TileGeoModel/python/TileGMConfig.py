# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import ProductionStep
from AtlasGeoModel.GeoModelConfig import GeoModelCfg
from AthenaCommon.Configurable import Configurable
Configurable.configurableRun3Behavior=1


def TileGMCfg(configFlags):
    result=GeoModelCfg(configFlags)

    TileDetectorTool=CompFactory.TileDetectorTool
    result.getPrimary().DetectorTools += [ TileDetectorTool() ]
    if configFlags.Common.ProductionStep != ProductionStep.Simulation:
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
