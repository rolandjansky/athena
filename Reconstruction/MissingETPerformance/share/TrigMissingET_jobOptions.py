include.block( "MissingETPerformance/TrigMissingET_jobOptions.py" )

from MissingETPerformance.MissingETPerformanceConf import \
    TrigMissingETTool as ConfiguredTrigMissingETTool
TrigMissingETTool = ConfiguredTrigMissingETTool()

ToolSvc += TrigMissingETTool

TrigMissingETTool.OutputLevel = INFO

print   TrigMissingETTool
