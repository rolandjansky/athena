from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AtlasGeoModel.GeoModelConfig import GeoModelCfg
from IOVDbSvc.IOVDbSvcConfig import addFolders

def LArGMCfg(configFlags):
    
    result,gms=GeoModelCfg(configFlags)

    doAlignment=configFlags.LAr.doAlign
    
    from LArGeoAlgsNV.LArGeoAlgsNVConf import LArDetectorToolNV
    gms.DetectorTools += [ LArDetectorToolNV(ApplyAlignments=doAlignment) ]

    result.addService(gms)

    if doAlignment:
        if configFlags.Input.isMC:
            #Monte Carlo case:
            result.merge(addFolders(configFlags,["/LAR/Align","/LAR/LArCellPositionShift"],"LAR_OFL"))
        else:
            #Regular offline data processing
            result.merge(addFolders(configFlags,["/LAR/Align","/LAR/LArCellPositionShift"],"LAR_ONL"))

            
    return result

if __name__ == "__main__":
    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1
    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.TestDefaults import defaultTestFiles

    ConfigFlags.Input.Files = defaultTestFiles.RAW
    ConfigFlags.lock()

    from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
    acc = LArGMCfg(ConfigFlags)
    f=open('LArGMCfg.pkl','w')
    acc.store(f)
    f.close()
