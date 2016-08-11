from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

## add an AthenaMonManager algorithm to the list of algorithms to be ran
from AthenaMonitoring.AthenaMonitoringConf import AthenaMonManager
AFPMonManager = AthenaMonManager( name = "AFPHitsMonitoringManager",
                                 FileKey             = DQMonFlags.monManFileKey(),
                                 Environment         = DQMonFlags.monManEnvironment(),
                                 ManualDataTypeSetup = DQMonFlags.monManManualDataTypeSetup(),
                                 DataType            = DQMonFlags.monManDataType()
                                 )

topSequence +=  AFPMonManager

from AFP_Monitoring.AFP_MonitoringConf import AFPHitsMonitorTool
AFPSiHitsMonTool = AFPHitsMonitorTool (
name           = "AFPSiHitsMonitoring",
PreScale       = 0,
EnableLumi = True
)

ToolSvc += AFPSiHitsMonTool
AFPMonManager.AthenaMonTools += [ AFPSiHitsMonTool ]

from AFP_Monitoring.AFP_MonitoringConf import AFPTechnicalMonitorTool
AFPTechnicalMonTool = AFPTechnicalMonitorTool (
name           = "AFPTechnicalMonitoring",
PreScale       = 0,
EnableLumi = True
)

ToolSvc += AFPTechnicalMonTool
AFPMonManager.AthenaMonTools += [ AFPTechnicalMonTool ]
