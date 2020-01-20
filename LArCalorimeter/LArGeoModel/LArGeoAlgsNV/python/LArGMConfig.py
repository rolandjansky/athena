# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AtlasGeoModel.GeoModelConfig import GeoModelCfg
from AthenaConfiguration.ComponentFactory import CompFactory
from IOVDbSvc.IOVDbSvcConfig import addFolders

def LArGMCfg(configFlags):
    
    result=GeoModelCfg(configFlags)

    doAlignment=configFlags.LAr.doAlign
    
    LArDetectorToolNV=CompFactory.LArDetectorToolNV
    result.getPrimary().DetectorTools += [ LArDetectorToolNV(ApplyAlignments=doAlignment) ]
    if not configFlags.Detector.SimulateCalo:
        result.getPrimary().DetectorTools["LArDetectorToolNV"].GeometryConfig = "RECO"

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

    acc = LArGMCfg(ConfigFlags)
    f=open('LArGMCfg.pkl','wb')
    acc.store(f)
    f.close()
