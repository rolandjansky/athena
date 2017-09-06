from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AtlasGeoModel.GeoModelConfig import GeoModelCfg

def LArGMCfg():
    result=GeoModelCfg()
    from LArGeoAlgsNV.LArGeoAlgsNVConf import LArDetectorToolNV
    gm=result.getService("GeoModelSvc")[0]
    gm.DetectorTools.append(LArDetectorToolNV())

    return result

