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

    acc,theLArCellMaker=LArCellBuilderCfg(configFlags)
    result.merge(acc)

    acc,theLArCellCorrectors=LArCellCorrectorCfg(configFlags)
    result.merge(acc)

    theTileCellBuilder = TileCellBuilder()
    cellAlgo=(CaloCellMaker(CaloCellMakerToolNames=[theLArCellMaker,CaloCellContainerFinalizerTool()]+theLArCellCorrectors,
                            CaloCellsOutputName="AllCalo"))
    return result,cellAlgo


                                      
if __name__=="__main__":
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import DEBUG
    log.setLevel(DEBUG)

    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1

    from AthenaConfiguration.AllConfigFlags import ConfigFlags

    ConfigFlags.set("global.InputFiles",["myRDO.pool.root"])
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
