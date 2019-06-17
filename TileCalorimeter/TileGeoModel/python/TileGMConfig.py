from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AtlasGeoModel.GeoModelConfig import GeoModelCfg
from AthenaCommon.Configurable import Configurable
Configurable.configurableRun3Behavior=1

def TileGMCfg(configFlags):
    result=GeoModelCfg(configFlags)

    from TileGeoModel.TileGeoModelConf import TileDetectorTool
    result.getPrimary().DetectorTools += [ TileDetectorTool() ]
    if not configFlags.Detector.SimulateCalo:
        result.getPrimary().DetectorTools["TileDetectorTool"].GeometryConfig = "RECO"

    return result


if __name__ == "__main__":
    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.TestDefaults import defaultTestFiles

    ConfigFlags.Input.Files = defaultTestFiles.RAW
    ConfigFlags.lock()

    acc = TileGMCfg( ConfigFlags )
    acc.store( file( "test.pkl", "w" ) )
    print "All OK"
