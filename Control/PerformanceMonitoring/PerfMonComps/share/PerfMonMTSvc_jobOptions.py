###############################
# Print what we're doing
###############################
from AthenaCommon.Logging import logging
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

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
