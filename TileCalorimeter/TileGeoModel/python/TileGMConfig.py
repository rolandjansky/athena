from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AtlasGeoModel.GeoModelConfig import GeoModelCfg

def TileGMCfg(ConfigFlags):
    result=ComponentAccumulator()
    
    result.executeModule(GeoModelCfg,ConfigFlags)

    from TileGeoModel.TileGeoModelConf import TileDetectorTool
    result.getService("GeoModelSvc").DetectorTools += [ TileDetectorTool() ]

    return result
