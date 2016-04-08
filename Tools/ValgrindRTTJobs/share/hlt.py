
from RecExConfig.RecFlags import rec
rec.doPerfMon.set_Value_and_Lock(False)

from TriggerJobOpts.TriggerFlags import TriggerFlags
if 'doLVL2' in dir():
  TriggerFlags.doEF = False
elif 'doEF' in dir():
  TriggerFlags.doLVL2 = False

# old, outdated: include("TriggerTest/testAthenaTrigRDO.py")
include("TriggerTest/testValgrindTrigBS_standalone.py")
TriggerFlags.enableMonitoring = [ 'Time' ]

jobproperties.PerfMonFlags.doMonitoring = False
jobproperties.PerfMonFlags.doFastMon = False
