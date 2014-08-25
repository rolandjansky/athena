include.block( "MissingETPerformance/CellsCleanerTool_jobOptions.py" )

from MissingETPerformance.MissingETPerformanceConf import CellsCleanerTool as ConfiguredCellsCleanerTool
CellsCleanerTool = ConfiguredCellsCleanerTool(
    )

ToolSvc += CellsCleanerTool
print      CellsCleanerTool

