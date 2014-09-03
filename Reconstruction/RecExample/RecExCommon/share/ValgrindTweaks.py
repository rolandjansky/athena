#Tweaks to run with valgrind
#Valgrind interferes with PerfMon and our RecoTiming code
#The following lines turn these features off

from RecExConfig.RecFlags import rec
rec.doPerfMon.set_Value_and_Lock(False)
rec.doDetailedPerfMon.set_Value_and_Lock(False)
rec.doSemiDetailedPerfMon.set_Value_and_Lock(False)
#and to kill it even in merging jobs:
from PerfMonComps.PerfMonFlags import jobproperties as pmon_properties
pmon_properties.PerfMonFlags.doMonitoring.set_Value_and_Lock(False)
pmon_properties.PerfMonFlags.doDetailedMonitoring.set_Value_and_Lock(False)
pmon_properties.PerfMonFlags.doSemiDetailedMonitoring.set_Value_and_Lock(False)


rec.doRecoTiming.set_Value_and_Lock(False)
