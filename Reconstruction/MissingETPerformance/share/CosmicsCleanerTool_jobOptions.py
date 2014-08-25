include.block( "MissingETPerformance/CosmicsCleanerTool_jobOptions.py" )

from MissingETPerformance.MissingETPerformanceConf import CosmicsCleanerTool as ConfiguredCosmicsCleanerTool
CosmicsCleanerTool = ConfiguredCosmicsCleanerTool(
    )

ToolSvc += CosmicsCleanerTool
print      CosmicsCleanerTool
