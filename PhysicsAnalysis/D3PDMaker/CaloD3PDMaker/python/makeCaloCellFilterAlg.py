# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from CaloIdentifier import SUBCALO

def makeCaloCellFilterAlg( CellsName = "AllCalo",
                           OutputCellsName="SelectedCells",
                           MaxNCells=200000,
                           CellSigmaCut=4,
                           CaloNums=[SUBCALO.LAREM, SUBCALO.LARHEC, SUBCALO.LARFCAL, SUBCALO.TILE],
                           CaloSamplings=[],
                           CellEnergyThreshold=0.) :

    from CaloD3PDMaker.CaloD3PDMakerConf import CaloCellFilterAlg
    caloCellFilter = CaloCellFilterAlg(OutputCellsName)
    caloCellFilter.CaloNums=CaloNums
    caloCellFilter.CaloSamplings=CaloSamplings

    noiseType = "totalNoise"
    from CaloTools.CaloNoiseCondAlg import CaloNoiseCondAlg
    CaloNoiseCondAlg(noisetype=noiseType)
    caloCellFilter.CaloNoise = noiseType

    caloCellFilter.CellsName = CellsName
    caloCellFilter.OutputCellsName = OutputCellsName 
    caloCellFilter.MaxNCells = MaxNCells
    caloCellFilter.CellSigmaCut = CellSigmaCut
    caloCellFilter.CellEnergyThreshold = CellEnergyThreshold

    return caloCellFilter


    



