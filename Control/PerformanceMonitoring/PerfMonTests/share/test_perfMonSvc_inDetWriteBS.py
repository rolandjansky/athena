###############################################################
#
# Job options file
#
#==============================================================

from AthenaCommon.AppMgr import theApp

if not 'doMonitoring' in dir():
    doMonitoring = True
    pass
from PerfMonComps.PerfMonFlags import jobproperties
jobproperties.PerfMonFlags.doMonitoring = doMonitoring
jobproperties.PerfMonFlags.doPersistencyMonitoring = True
jobproperties.PerfMonFlags.doDetailedMonitoring = True
jobproperties.PerfMonFlags.OutputFile = "perfmon.indetWriteBS.root"

###############################
# Load perf service
###############################
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from PerfMonComps.JobOptCfg import PerfMonSvc
svcMgr += PerfMonSvc(
    "PerfMonSvc",
    OutputLevel  = INFO
    )

################################################################
# Steer InDetRec job
include( "InDetRecExample/WriteInDetBS_jobOptions.py")
###############################################################

from GaudiSvc.GaudiSvcConf import MessageSvc
svcMgr += MessageSvc( defaultLimit = 4000000 )
svcMgr.MessageSvc.OutputLevel  = ERROR

#==============================================================
#
# End of job options file
#
###############################################################
