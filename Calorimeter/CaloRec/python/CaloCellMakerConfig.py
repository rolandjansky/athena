# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
#from LArCellRec.LArCellRecConf import LArCellBuilderFromLArRawChannelTool
from CaloRec.CaloRecConf import CaloCellMaker, CaloCellContainerFinalizerTool     
from LArCellRec.LArCellBuilderConfig import LArCellBuilderCfg,LArCellCorrectorCfg
from TileRecUtils.TileRecUtilsConf import TileCellBuilder

def CaloCellMakerCfg(configFlags):
    result=ComponentAccumulator()
   
    from LArGeoAlgsNV.LArGMConfig import LArGMCfg
    from TileGeoModel.TileGMConfig import TileGMCfg
    
    result.merge(LArGMCfg(configFlags))
    result.merge(TileGMCfg(configFlags))

    larCellBuilder=LArCellBuilderCfg(configFlags)


    
    larCellCorrectors=LArCellCorrectorCfg(configFlags)

    theTileCellBuilder = TileCellBuilder()

    cellAlgo=CaloCellMaker(CaloCellMakerToolNames=[larCellBuilder.popPrivateTools(),CaloCellContainerFinalizerTool()]+larCellCorrectors.popPrivateTools(),
                            CaloCellsOutputName="AllCalo")
    result.merge(larCellBuilder)
    result.merge(larCellCorrectors)
    return result,cellAlgo


                                      
if __name__=="__main__":
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import DEBUG
    log.setLevel(DEBUG)

    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1

    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.TestDefaults import defaultTestFiles

    ConfigFlags.Input.Files = defaultTestFiles.RAW
    ConfigFlags.lock()

    cfg=ComponentAccumulator()

    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg.merge(PoolReadCfg(ConfigFlags))
    
    acc,cellMakerAlg=CaloCellMakerCfg(ConfigFlags)
    cfg.merge(acc)

    cellMakerAlg.CaloCellsOutputName="AllCaloNew"
    cfg.addEventAlgo(cellMakerAlg)
    
    f=open("CaloCellMaker.pkl","w")
    cfg.store(f)
    f.close()

    #ConfigFlags.dump()
