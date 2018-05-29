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
    result.addConfig(LArGMCfg,configFlags)
    result.addConfig(TileGMCfg,configFlags)    

    theLArCellMaker=result.addConfig(LArCellBuilderCfg,configFlags)

    theLArCellCorrectors=result.addConfig(LArCellCorrectorCfg,configFlags)

    theTileCellBuilder = TileCellBuilder()
    result.addEventAlgo(CaloCellMaker(CaloCellMakerToolNames=theLArCellMaker+
                                                             [CaloCellContainerFinalizerTool()]+theLArCellCorrectors,
                                      CaloCellsOutputName="AllCalo"))
    return result


                                      
if __name__=="__main__":
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import DEBUG

    log.setLevel(DEBUG)
    from AthenaConfiguration.AllConfigFlags import ConfigFlags

    ConfigFlags.set("global.InputFiles",["myRDO.pool.root"])
    ConfigFlags.lock()

    cfg=ComponentAccumulator()

    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg.addConfig(PoolReadCfg,ConfigFlags)
    
    cfg.addConfig(CaloCellMakerCfg,ConfigFlags)
    cfg.getEventAlgo("CaloCellMaker").CaloCellsOutputName="AllCaloNew"

    f=open("CaloCellMaker.pkl","w")
    cfg.store(f)
    f.close()

    #ConfigFlags.dump()
