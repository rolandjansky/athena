#
# jobOptions to make CaloCell from raw (H6 test beam)
#

#make tile raw channels
#include( "TileSimAlgs/TileSimAlgs_jobOptions.py" )
#
theApp.Dlls += [ 
"CaloRec"
]
#theApp.TopAlg += [
#"CaloCellMaker" 
#]
#--------------------------------------------------------------
# Algorithms Private Options
#--------------------------------------------------------------
CaloCellMaker = Algorithm( "CaloCellMaker" )
CaloCellMaker.CaloCellsOutputName = "AllCalo"

#make the calo cells (LArEM, HEC and FCal

include ("LArCellRec/LArCellFromLArRawTool_H6_jobOptions.py")


ToolSvc = Service( "ToolSvc" )


#apply correction to LAr Cells
#CaloCellMaker.CaloCellMakerToolNames += ["CaloCellContainerCorrectorTool/LArCellCorrectorTool"]
#should use python enum
#0 LArEM
#1 LArHEC
#2 LArFCal
#3 Tile
#4 All
# assume only lar cell so far. More efficient than checking on calo type.
#ToolSvc.LArCellCorrectorTool.CaloNums = [ 4 ] 
#ToolSvc.LArCellCorrectorTool.CellCorrectionToolNames= [ "LArG3Escale" ] 

#include( "TileRecUtils/TileCellBuilder_jobOptions.py" )

#finalize container
CaloCellMaker.CaloCellMakerToolNames += ["CaloCellContainerFinalizerTool"]

#make lots of checks (should not be necessary eventually)
CaloCellMaker.CaloCellMakerToolNames += ["CaloCellContainerCheckerTool"] 

