from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AtlasGeoModel.GeoModelConfig import GeoModelCfg
from IOVDbSvc.IOVDbSvcConfig import addFolders

def LArGMCfg(configFlags):
    result=ComponentAccumulator()
    
    result.addConfig(GeoModelCfg,configFlags)

    doAlignment=configFlags("LAr.doAlign")
    
    from LArGeoAlgsNV.LArGeoAlgsNVConf import LArDetectorToolNV
    result.getService("GeoModelSvc").DetectorTools += [ LArDetectorToolNV(ApplyAlignments=doAlignment) ]

    if doAlignment:
        if configFlags.get("global.isMC"):
            #Monte Carlo case:
            result.addConfig(addFolders,configFlags,["/LAR/Align","/LAR/LArCellPositionShift"],"LAR_OFL")
        else:
            #Regular offline data processing
            result.addConfig(addFolders,configFlags,["/LAR/Align","/LAR/LArCellPositionShift"],"LAR_ONL")

        
    return result
