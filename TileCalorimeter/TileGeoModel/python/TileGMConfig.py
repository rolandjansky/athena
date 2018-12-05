from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AtlasGeoModel.GeoModelConfig import GeoModelCfg
from AthenaCommon.Configurable import Configurable
Configurable.configurableRun3Behavior=1

def TileGMCfg(configFlags):
    result,gms=GeoModelCfg(configFlags)

    from TileGeoModel.TileGeoModelConf import TileDetectorTool
    gms.DetectorTools += [ TileDetectorTool() ]

    result.addService(gms)

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
