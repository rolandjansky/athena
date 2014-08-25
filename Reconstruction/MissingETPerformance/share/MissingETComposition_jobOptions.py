include.block( "MissingETPerformance/MissingETComposition_jobOptions.py" )

from MissingETPerformance.MissingETPerformanceConf import \
     MissingETCompositionTool as ConfiguredMissingETCompositionTool
MissingETCompositionTool = ConfiguredMissingETCompositionTool(
    MakeEventDisplays = False,
    EventInfoKey     = "ByteStreamEventInfo",
    #EventInfoKey     = "McEventInfo",
    FolderName       = "",
    )

ToolSvc += MissingETCompositionTool
print      MissingETCompositionTool
