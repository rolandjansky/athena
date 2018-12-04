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
    ConfigFlags.Input.Files = ["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigP1Test/data17_13TeV.00327265.physics_EnhancedBias.merge.RAW._lb0100._SFO-1._0001.1"]

    acc = TileGMCfg( ConfigFlags )
    acc.store( file( "test.pkl", "w" ) )
    print "All OK"
