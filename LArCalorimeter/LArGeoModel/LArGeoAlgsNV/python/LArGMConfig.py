from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AtlasGeoModel.GeoModelConfig import GeoModelCfg
from IOVDbSvc.IOVDbSvcConfig import addFolders

def LArGMCfg(configFlags):
    
    result,gms=GeoModelCfg(configFlags)

    doAlignment=configFlags("LAr.doAlign")
    
    from LArGeoAlgsNV.LArGeoAlgsNVConf import LArDetectorToolNV
    gms.DetectorTools += [ LArDetectorToolNV(ApplyAlignments=doAlignment) ]

    result.addService(gms)

    if doAlignment:
        if configFlags.get("global.isMC"):
            #Monte Carlo case:
            result.merge(addFolders(configFlags,["/LAR/Align","/LAR/LArCellPositionShift"],"LAR_OFL")[0])
        else:
            #Regular offline data processing
            result.merge(addFolders(configFlags,["/LAR/Align","/LAR/LArCellPositionShift"],"LAR_ONL")[0])

            
    return result
