include.block( "MissingETPerformance/Zee_jobOptions.py" )

from MissingETPerformance.MissingETPerformanceConf import \
     ZeeTool as ConfiguredZeeTool
ZeeTool = ConfiguredZeeTool(

    Zboson_lower             = 0,
    Zboson_upper             = 180,
    Zboson_NSplits           = 100,
    HadActivity_lower           = 0,
    HadActivity_upper           = 650,
    HadActivity_NSplits         = 100,

    DiLepton_lower              = 0,
    DiLepton_upper              = 80,
    DiLepton_NSplits            = 30,

    Pt_Res_NBins                 = 100,
    Pt_Res_lower                 = -100,
    Pt_Res_upper                 = 100,
    Pt_lower                     = 0,
    Pt_upper                     = 60,
    Pt_NSplits                   = 20,
    )

ToolSvc += ZeeTool
print      ZeeTool
