from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AtlasGeoModel.GeoModelConfig import GeoModelCfg

def TileGMCfg(configFlags):
    result,gms=GeoModelCfg(configFlags)

    from TileGeoModel.TileGeoModelConf import TileDetectorTool
    gms.DetectorTools += [ TileDetectorTool() ]

    result.addService(gms)

    return result
