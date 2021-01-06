# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from CaloIdentifier import SUBCALO

def makeTileCellFilterAlg( CellsName = "AllCalo",
                           OutputCellsName="SelectedCells",
                           MaxNCells=200000,
                           CellSigmaCut=4) :

    from CaloD3PDMaker.CaloD3PDMakerConf import CaloCellFilterAlg
    tileCellFilter = CaloCellFilterAlg(OutputCellsName)
    tileCellFilter.CaloNums=[SUBCALO.TILE]

    tileCellFilter.CellsName = CellsName
    tileCellFilter.OutputCellsName = OutputCellsName
    tileCellFilter.MaxNCells = MaxNCells
    tileCellFilter.CellSigmaCut = CellSigmaCut

    return tileCellFilter

