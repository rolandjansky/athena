###############################
# Load PerfMonMTSvc
###############################
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
if not hasattr(svcMgr, "PerfMonMTSvc"):
    from PerfMonComps.PerfMonCompsConf import PerfMonMTSvc
    svcMgr += PerfMonMTSvc("PerfMonMTSvc")
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

###############################
# Print what we did 
###############################
from AthenaCommon.Logging import logging
log = logging.getLogger("PerfMonMTSvc_jobOptions.py")
log.info("Setting up PerfMonMT...")
