# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

from AtlasGeoModel.GeoModelConfig import GeoModelCfg
from AthenaConfiguration.ComponentFactory import CompFactory
from AthenaConfiguration.Enums import ProductionStep
from IOVDbSvc.IOVDbSvcConfig import addFolders

def LArGMCfg(configFlags):
    
    result=GeoModelCfg(configFlags)

    doAlignment=configFlags.LAr.doAlign

    tool = CompFactory.LArDetectorToolNV(ApplyAlignments=doAlignment, EnableMBTS=configFlags.Detector.GeometryMBTS)
    if configFlags.Common.ProductionStep != ProductionStep.Simulation and configFlags.Common.ProductionStep != ProductionStep.FastChain:
        tool.GeometryConfig = "RECO"

    result.getPrimary().DetectorTools += [ tool ]

    if doAlignment:
        if configFlags.Input.isMC:
            #Monte Carlo case:
            result.merge(addFolders(configFlags,["/LAR/Align","/LAR/LArCellPositionShift"],"LAR_OFL"))
        else:
            if configFlags.Overlay.DataOverlay:
                #Data overlay
                result.merge(addFolders(configFlags, ["/LAR/Align"], "LAR_ONL"))
                result.merge(addFolders(configFlags, ["/LAR/LArCellPositionShift"], "LAR_OFL", tag="LArCellPositionShift-ideal", db="OFLP200"))
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
