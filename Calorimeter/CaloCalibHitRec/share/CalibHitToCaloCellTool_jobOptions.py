from CaloCalibHitRec.CaloCalibHitRecConf import CalibHitToCaloCellTool
CalibHitToCaloCellTool_LAr = CalibHitToCaloCellTool(name = "CalibHitToCaloCellTool_LAr",
                                                    CalibHitContainers = [ 'LArCalibrationHitInactive', 'LArCalibrationHitActive' ],
                                                    OutputCellContainerName = "TruthLArCells",
                                                    OutputClusterContainerName = "TruthLArClusters",
                                                    WriteTruthCellContainers = True,
                                                    WriteTruthClusterContainers = True,
                                                    SingleParticleBarcode = 10001,
                                                    OutputLevel = ERROR)
ToolSvc += CalibHitToCaloCellTool_LAr

# CalibHitToCaloCellTool_Tile = CalibHitToCaloCellTool(name = "CalibHitToCaloCellTool_Tile",
#                                                   CalibHitContainers = [  'TileCalibHitInactiveCell', 'TileCalibHitActiveCell' ],
#                                                   OutputCellContainerName = "TruthTileCells",
#                                                   OutputClusterContainerName = "TruthTileClusters",
#                                                   WriteTruthCellContainers = True,
#                                                   WriteTruthClusterContainers = True,
#                                                   OutputLevel = ERROR)
# ToolSvc += CalibHitToCaloCellTool_Tile
