# Apply ICellWeightTools to each cell in  the new Container
CaloCellMaker.CaloCellMakerToolNames += ["CaloCellContainerCorrectorTool/FakeProblemsTool"]
ToolSvc.FakeProblemsTool.CellCorrectionToolNames = [ "CaloCellWeightCorrection/LoopWeightTool" ]

# Fake HV problems and readout problems
ToolSvc.LoopWeightTool.CellWeightToolNames = ["LArCellFakeProbHV/fakehv", "LArCellFakeProbElectronics/fakeEle","TileCellFakeProb/faketile"]

# ToolSvc.LoopWeightTool.CellWeightToolNames = ["LArCellFakeProbElectronics/fakeEle"]
# ToolSvc.FakeProblemsTool.CaloNums = [ SUBCALO.LAREM, SUBCALO.LARHEC, SUBCALO.LARFCAL ]
ToolSvc.FakeProblemsTool.CaloNums = [ SUBCALO.NSUBCALO ]

#
# LArCellFakeProbHV - weights cells du to given HV lines
# arguments: HV line - partition canline cannode line weight
ToolSvc.fakehv.DeadHVs=["3 1 48 1 0","3 1 50 1 0","3 1 6 10 0","3 1 44 4 0","3 1 2 2  0","3 2 2 2 0","3 2 18 12 0","1 1 24 2 0","1 1 6 6 0"]



# LArCellFakeProbElectronics.Dead_FEC_FEB_CHAN - simulates problems in LAr readout
# arguments: isbarrel side feedthrough feb channel weight
# slot = 999 applies the weight to  all channels in all slots in the given feedthrough
# channel = 999 applies the weight to  all channels in the given slot
ToolSvc.fakeEle.Dead_FEC_FEB_CHAN=["0 0 10 999 13 0","0 0 11 999 13 0","1 1 2 999 9 0","1 1 3 999 1231 0"]


# For TileCal dead drawers are simulated:
# ROS DRAWER WEIGHT
ToolSvc.faketile.DeadDrawerList=[ "0 11  0","4 8 0"]
