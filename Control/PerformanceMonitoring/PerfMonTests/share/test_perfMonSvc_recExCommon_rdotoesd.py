#readESD = True
doESD = True
doAOD = False
doWriteESD = False
doWriteAOD = False
doWriteTAG = False 

doMuGirl  = False
doTrigger = False
doStaco   = False
doTauRec  = False

from PerfMonComps.PerfMonFlags import jobproperties
jobproperties.PerfMonFlags.doMonitoring = True
jobproperties.PerfMonFlags.doPersistencyMonitoring = True
jobproperties.PerfMonFlags.doDetailedMonitoring = True
jobproperties.PerfMonFlags.OutputFile = "rdotoesd.perfmon.root"

from AthenaCommon.AppMgr import ServiceMgr as svcMgr

###############################
# Load perf service
###############################
from PerfMonComps.JobOptCfg import PerfMonSvc
svcMgr += PerfMonSvc( "PerfMonSvc" )

EvtMax = -1

include ("RecExCommon/RecExCommon_flags.py")
# Flags that are defined in python are best set here
# switch off ID and muons
#DetFlags.ID_setOff()
#DetFlags.Calo_setOff()
DetFlags.Muon_setOff()
# switch off xkalman
# InDetFlags.doxKalman=False

# main jobOption
include ("RecExCommon/RecExCommon_topOptions.py")


