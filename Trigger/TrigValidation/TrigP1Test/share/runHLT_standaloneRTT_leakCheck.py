# This is just a wrapper job option made for RTT running

from RecExConfig.RecFlags import rec
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags as acf

from AthenaCommon.Include import include

include("TriggerRelease/runHLT_standalone.py")

if hasattr(jobproperties,"PerfMonFlags"):
   # We don't need this for athenaMT/PT
   jobproperties.PerfMonFlags.doPersistencyMonitoring = False

   if hasattr(jobproperties.PerfMonFlags,"doHephaestusMon"):
      jobproperties.PerfMonFlags.doHephaestusMon = True

   # Minimal performance monitoring
   # To avoid increase in memory usage due to perfmon itself
   if 'perfmonNoDetail' in dir():
      jobproperties.PerfMonFlags.doFastMon = True
      jobproperties.PerfMonFlags.doDetailedMonitoring = False

include("TrigValTools/leakCheck.py")

