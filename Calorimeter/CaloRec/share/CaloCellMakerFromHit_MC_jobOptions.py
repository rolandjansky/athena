#jobOption file for CaloCellMaker to make CaloCell from Hit (directly)  on G3

if DetFlags.makeRIO.Tile_on():
    include( "TileSimAlgs/TileSimAlgs_jobOptions.py" )
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
    include ("LArCellRec/LArCellFromLArHitTool_MC_jobOptions.py")



ToolSvc = Service( "ToolSvc" )

#apply correction to LAr Cells
CaloCellMaker.CaloCellMakerToolNames += ["CaloCellContainerCorrectorTool/LArCellCorrectorTool"]
#should use python enum
#0 LArEM
#1 LArHEC
#2 LArFCal
#3 Tile
#4 All
# assume only lar cell so far. More efficient than checking on calo type.
ToolSvc.LArCellCorrectorTool.CaloNums = [ 4 ] 
ToolSvc.LArCellCorrectorTool.CellCorrectionToolNames= [ "LArG3Escale" ] 


if DetFlags.makeRIO.Tile_on():
    include( "TileRecUtils/TileCellBuilder_jobOptions.py" )

#finalize container
CaloCellMaker.CaloCellMakerToolNames += ["CaloCellContainerFinalizerTool"]

#make lots of checks (should not be necessary eventually)
CaloCellMaker.CaloCellMakerToolNames += ["CaloCellContainerCheckerTool"] 

