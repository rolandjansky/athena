
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

LucidMon = AthenaMonManager(name="LucidMonManager",
                            FileKey             = DQMonFlags.monManFileKey(),
                            Environment         = DQMonFlags.monManEnvironment(),
                            ManualDataTypeSetup = DQMonFlags.monManManualDataTypeSetup(),
                            DataType            = DQMonFlags.monManDataType())

topSequence += LucidMon

from LUCID_Monitoring.LUCID_MonitoringConf import LUCID_HitsMoniTool

LUCID_TestTool = LUCID_HitsMoniTool (name = "LUCID_HitsMoniTool")

ToolSvc += LUCID_TestTool

LucidMon.AthenaMonTools += [ LUCID_TestTool ]
