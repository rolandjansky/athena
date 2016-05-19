#
# jobOptions file for CaloCellMaker to merge existing CaloCellContainer in
# a single one
#
theApp.Dlls += [ 
"CaloRec"
]
theApp.TopAlg += [
"CaloCellMaker/CaloCellContainerMerger" 
]
#--------------------------------------------------------------
# Algorithms Private Options
#--------------------------------------------------------------
CaloCellContainerMerger = Algorithm( "CaloCellContainerMerger" )
CaloCellContainerMerger.CaloCellsOutputName = "AllCalo"
# tool to merge existing cell container in the new one
#note that these global tools (and others) will be shared with other cell makers
CaloCellContainerMerger.CaloCellMakerToolNames += ["CaloCellContainerMergerTool/emCellsMergerTool"]
CaloCellContainerMerger.CaloCellMakerToolNames += ["CaloCellContainerMergerTool/HECCellsMergerTool"]
CaloCellContainerMerger.CaloCellMakerToolNames += ["CaloCellContainerMergerTool/FCalCellsMergerTool"]
CaloCellContainerMerger.CaloCellMakerToolNames += ["CaloCellContainerMergerTool/TileCellsMergerTool"]
CaloCellContainerMerger.CaloCellMakerToolNames += ["CaloCellContainerFinalizerTool"]
CaloCellContainerMerger.CaloCellMakerToolNames += ["CaloCellContainerCheckerTool"]; 
ToolSvc = Service( "ToolSvc" )
ToolSvc.emCellsMergerTool.CaloCellsName = "LArEM"
ToolSvc.HECCellsMergerTool.CaloCellsName = "LArHEC"
ToolSvc.FCalCellsMergerTool.CaloCellsName = "LArFCal"
ToolSvc.TileCellsMergerTool.CaloCellsName = "TileCellContainerKey"
