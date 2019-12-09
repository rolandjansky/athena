###############################
# Print what we're doing
###############################
from AthenaCommon.Logging import logging
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from AthenaCommon.ConcurrencyFlags import jobproperties as jp

log = logging.getLogger("PerfMonMTSvc_jobOptions.py")
log.info("Setting up PerfMonMT...")


###############################
# Load PerfMonMTSvc
###############################
if not hasattr(svcMgr, 'PerfMonMTSvc'):
    from PerfMonComps.MTJobOptCfg import PerfMonMTSvc
    svcMgr += PerfMonMTSvc("PerfMonMTSvc")
    # Disable event loop monitoring by default
    svcMgr.PerfMonMTSvc.doEventLoopMonitoring = False 
    # Disable detailed table printing by default
    svcMgr.PerfMonMTSvc.printDetailedTables = False
    # Configure the check point sequence in the event loop monitoring.
    # By default common difference is the number of threads with which the job is running
    svcMgr.PerfMonMTSvc.checkPointType = "Arithmetic" 
    svcMgr.PerfMonMTSvc.checkPointFactor = jp.ConcurrencyFlags.NumThreads()

    svcMgr.PerfMonMTSvc.nThreads = jp.ConcurrencyFlags.NumThreads()
    pass

###############################
# Load PerfMonMTAlg
###############################
from AthenaCommon.AlgSequence import AthSequencer
topSequence = AthSequencer("AthAlgSeq")
if not hasattr(topSequence, "PerfMonMTSvcAlg"):
    from PerfMonComps.PerfMonCompsConf import PerfMonMTAlg
    topSequence += PerfMonMTAlg("PerfMonMTAlg")
    pass
