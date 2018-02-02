from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AtlasGeoModel.GeoModelConfig import GeoModelCfg
from IOVDbSvc.IOVDbSvcConfig import addFolders

def LArGMCfg(ConfigFlags):
    result=ComponentAccumulator()
    
    result.executeModule(GeoModelCfg,ConfigFlags)

    doAlignment=ConfigFlags.get("LArGeoAlgsNV.LArGeoModelFlags.doAlign")
    
    from LArGeoAlgsNV.LArGeoAlgsNVConf import LArDetectorToolNV
    result.getService("GeoModelSvc").DetectorTools += [ LArDetectorToolNV(ApplyAlignments=doAlignment) ]

    if doAlignment:
        if ConfigFlags.get("AthenaConfiguration.GlobalConfigFlags.isMC"):
            #Monte Carlo case:
            result.executeModule(addFolders,ConfigFlags,["/LAR/Align","/LAR/LArCellPositionShift"],"LAR_OFL")
        else:
            #Regular offline data processing
            result.executeModule(addFolders, ConfigFlags,["/LAR/Align","/LAR/LArCellPositionShift"],"LAR_ONL")

        
    return result
