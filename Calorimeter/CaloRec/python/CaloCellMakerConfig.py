# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
#from LArCellRec.LArCellRecConf import LArCellBuilderFromLArRawChannelTool
from CaloRec.CaloRecConf import CaloCellMaker, CaloCellContainerFinalizerTool     
from LArCellRec.LArCellBuilderConfig import LArCellBuilderCfg,LArCellCorrectorCfg
from TileRecUtils.TileCellBuilderConfig import TileCellBuilderCfg
from CaloCellCorrection.CaloCellCorrectionConfig import CaloCellPedestalCorrCfg, CaloCellNeighborsAverageCorrCfg, CaloCellTimeCorrCfg, CaloEnergyRescalerCfg

def CaloCellMakerCfg(configFlags):
    result=ComponentAccumulator()
   
    from LArGeoAlgsNV.LArGMConfig import LArGMCfg
    from TileGeoModel.TileGMConfig import TileGMCfg
    
    result.merge(LArGMCfg(configFlags))
    result.merge(TileGMCfg(configFlags))

    larCellBuilder=LArCellBuilderCfg(configFlags)

    larCellCorrectors=LArCellCorrectorCfg(configFlags)
    theTileCellBuilder = TileCellBuilderCfg(configFlags)

    caloCellCorrections=[]
    #Corrections tools that are not LAr or Tile specific:
    if configFlags.Calo.Cell.doPileupOffsetBCIDCorr or configFlags.Cell.doPedestalCorr:
        theCaloCellPedestalCorr=CaloCellPedestalCorrCfg(configFlags)
        caloCellCorrections.append(result.popToolsAndMerge(theCaloCellPedestalCorr))

    #LAr HV scale corr must come after pedestal corr
    if configFlags.LAr.doHVCorr:
        from LArCellRec.LArCellBuilderConfig import LArHVCellContCorrCfg
        theLArHVCellContCorr=LArHVCellContCorrCfg(configFlags)
        caloCellCorrections.append(result.popToolsAndMerge(theLArHVCellContCorr))


    if configFlags.Calo.Cell.doDeadCellCorr:
        theCaloCellNeighborAvg=CaloCellNeighborsAverageCorrCfg(configFlags)
        caloCellCorrections.append(result.popToolsAndMerge(theCaloCellNeighborAvg))

    if configFlags.Calo.Cell.doEnergyCorr:
        theCaloCellEnergyRescaler=CaloEnergyRescalerCfg(configFlags)
        caloCellCorrections.append(result.popToolsAndMerge(theCaloCellEnergyRescaler))


    if configFlags.Calo.Cell.doTimeCorr:
        theCaloTimeCorr=CaloCellTimeCorrCfg(configFlags)
        caloCellCorrections.append(result.popToolsAndMerge(theCaloTimeCorr))


    #Old Config:
    #CaloCellMakerToolNames': PrivateToolHandleArray(['LArCellBuilderFromLArRawChannelTool/LArCellBuilderFromLArRawChannelTool','TileCellBuilder/TileCellBuilder','CaloCellContainerFinalizerTool/CaloCellContainerFinalizerTool','LArCellNoiseMaskingTool/LArCellNoiseMaskingTool','CaloCellPedestalCorr/CaloCellPedestalCorr','CaloCellNeighborsAverageCorr/CaloCellNeighborsAverageCorr','CaloCellContainerCheckerTool/CaloCellContainerCheckerTool']),

    cellAlgo=CaloCellMaker(CaloCellMakerToolNames=[larCellBuilder.popPrivateTools(),theTileCellBuilder.popPrivateTools(),CaloCellContainerFinalizerTool()]+larCellCorrectors.popPrivateTools()+caloCellCorrections,
                           CaloCellsOutputName="AllCalo")


    result.merge(larCellBuilder)
    result.merge(larCellCorrectors)
    result.merge(theTileCellBuilder)

    result.addEventAlgo(cellAlgo,primary=True)
    return result

 
                                      
if __name__=="__main__":
    from AthenaCommon.Logging import log
    from AthenaCommon.Constants import DEBUG
    log.setLevel(DEBUG)

    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1

    from AthenaConfiguration.AllConfigFlags import ConfigFlags
    from AthenaConfiguration.TestDefaults import defaultTestFiles

    ConfigFlags.Input.Files = defaultTestFiles.RDO
    ConfigFlags.lock()

    cfg=ComponentAccumulator()

    from xAODEventInfoCnv.xAODEventInfoCreator import xAODMaker__EventInfoCnvAlg
    cfg.addEventAlgo(xAODMaker__EventInfoCnvAlg())

    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    cfg.merge(PoolReadCfg(ConfigFlags))
    
    acc=CaloCellMakerCfg(ConfigFlags)
    acc.getPrimary().CaloCellsOutputName="AllCaloNew"
    cfg.merge(acc)
    
    f=open("CaloCellMaker.pkl","w")
    cfg.store(f)
    f.close()

    #ConfigFlags.dump()
