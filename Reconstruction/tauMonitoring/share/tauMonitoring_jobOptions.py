monManTau = AthenaMonManager(
             name                = "tauMonManager",
             FileKey             = DQMonFlags.monManFileKey(),
             Environment         = DQMonFlags.monManEnvironment(),
             ManualDataTypeSetup = DQMonFlags.monManManualDataTypeSetup(),
             DataType            = DQMonFlags.monManDataType())
topSequence += monManTau


from tauMonitoring.tauMonitoringConf import *
from AthenaMonitoring.BadLBFilterTool import GetLArBadLBFilterTool
monbadlb = GetLArBadLBFilterTool()

tauMonTool           = tauMonTool (name = "tauMonTool")
tauMonTool.doTrigger = DQMonFlags.useTrigger()


tauMonTool.FilterTools = [ monAtlasReadyFilterTool ]
tauMonTool.FilterTools.append(monbadlb)
ToolSvc += tauMonTool
monManTau.AthenaMonTools += [ tauMonTool ]

