#jobOption file for CaloCellMaker to make CaloCell from Hit (through RawChannel) on G3

#make Tile Raw Channels
if DetFlags.makeRIO.Tile_on():
    include( "TileSimAlgs/TileSimAlgs_jobOptions.py" )


#LAr hit to LAr Digit to Raw Channel
if DetFlags.makeRIO.LAr_on():
    include( "LArROD/LArROD_MC_jobOptions.py" )
    digitmaker1 = Algorithm( "digitmaker1" )
    digitmaker1.NoiseOnOff = FALSE ; # switch noise off
    digitmaker1.CrossTalk  = FALSE ; # switch crosstalk off


#
theApp.Dlls += [ 
"CaloRec"
]
theApp.TopAlg += [
"CaloCellMaker" 
]
#--------------------------------------------------------------
# Algorithms Private Options
#--------------------------------------------------------------
CaloCellMaker = Algorithm( "CaloCellMaker" )
CaloCellMaker.CaloCellsOutputName = "AllCalo"

#make the calo cells (LArEM, HEC and FCal
if DetFlags.makeRIO.LAr_on():
    include ("LArCellRec/LArCellFromLArRawTool_MC_jobOptions.py")


ToolSvc = Service( "ToolSvc" )


if DetFlags.makeRIO.LAr_on():
    # apply correction to LAr Cells
    CaloCellMaker.CaloCellMakerToolNames += ["CaloCellContainerCorrectorTool/LArCellCorrectorTool"]
    # should use python enum
    # 0 LArEM
    # 1 LArHEC
    # 2 LArFCal
    # 3 Tile
    # 4 All
    # assume only lar cell so far. More efficient than checking on calo type.

    ToolSvc.LArCellCorrectorTool.CaloNums = [ 4 ] 
    ToolSvc.LArCellCorrectorTool.CellCorrectionToolNames= [ "LArG3Escale" ] 

if DetFlags.makeRIO.Tile_on():
    include( "TileRecUtils/TileCellBuilder_jobOptions.py" )

#finalize container
CaloCellMaker.CaloCellMakerToolNames += ["CaloCellContainerFinalizerTool"]

#make lots of checks (should not be necessary eventually)
CaloCellMaker.CaloCellMakerToolNames += ["CaloCellContainerCheckerTool"] 

