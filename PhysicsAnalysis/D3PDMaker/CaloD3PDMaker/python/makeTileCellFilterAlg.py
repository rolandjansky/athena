# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

from CaloIdentifier import SUBCALO
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

def makeTileCellFilterAlg( CellsName = "AllCalo",
                           OutputCellsName="SelectedCells",
                           MaxNCells=200000,
                           CellSigmaCut=4) :

    print " makeTileCellFilterAlg: CellsName = ", CellsName
    print " makeTileCellFilterAlg: OutputCellsName = ", OutputCellsName
    print " makeTileCellFilterAlg: CaloNums = ", [SUBCALO.TILE]

    from CaloD3PDMaker.CaloD3PDMakerConf import CaloCellFilterAlg
    tileCellFilter = CaloCellFilterAlg(OutputCellsName)
    tileCellFilter.CaloNums=[SUBCALO.TILE]

    tileCellFilter.CellsName = CellsName
    tileCellFilter.OutputCellsName = OutputCellsName
    tileCellFilter.MaxNCells = MaxNCells
    tileCellFilter.CellSigmaCut = CellSigmaCut

    return tileCellFilter

