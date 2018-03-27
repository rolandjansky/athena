from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AtlasGeoModel.GeoModelConfig import GeoModelCfg

def TileGMCfg(configFlags):
    result=ComponentAccumulator()
    
    result.addConfig(GeoModelCfg,configFlags)

    from TileGeoModel.TileGeoModelConf import TileDetectorTool
    result.getService("GeoModelSvc").DetectorTools += [ TileDetectorTool() ]

    return result
