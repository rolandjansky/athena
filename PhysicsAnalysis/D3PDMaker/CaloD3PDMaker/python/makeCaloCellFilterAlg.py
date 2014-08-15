# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from CaloIdentifier import SUBCALO
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

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

    from CaloTools.CaloNoiseToolDefault import CaloNoiseToolDefault
    theCaloNoiseTool = CaloNoiseToolDefault()
    svcMgr.ToolSvc+=theCaloNoiseTool
    caloCellFilter.NoiseTool = theCaloNoiseTool

    caloCellFilter.CellsName = CellsName
    caloCellFilter.OutputCellsName = OutputCellsName 
    caloCellFilter.MaxNCells = MaxNCells
    caloCellFilter.CellSigmaCut = CellSigmaCut
    caloCellFilter.CellEnergyThreshold = CellEnergyThreshold

    return caloCellFilter


    



