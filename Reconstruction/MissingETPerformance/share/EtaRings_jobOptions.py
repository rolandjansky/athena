include.block( "MissingETPerformance/EtaRings_jobOptions.py" )

from MissingETPerformance.MissingETPerformanceConf import \
     EtaRingsTool as ConfiguredEtaRingsTool
EtaRingsTool = ConfiguredEtaRingsTool(
    )

ToolSvc += EtaRingsTool
printfunc (     EtaRingsTool)
