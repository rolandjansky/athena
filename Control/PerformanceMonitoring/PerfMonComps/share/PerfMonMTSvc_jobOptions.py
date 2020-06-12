###############################
# Print what we're doing
###############################
from AthenaCommon.Logging import logging
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from AthenaCommon.ConcurrencyFlags import jobproperties as jp
import os,psutil

log = logging.getLogger("PerfMonMT")
log.info("Setting up PerfMonMT...")

###############################
# Load PerfMonMTSvc
###############################
if not hasattr(svcMgr, 'PerfMonMTSvc'):
    from PerfMonComps.MTJobOptCfg import PerfMonMTSvc
    svcMgr += PerfMonMTSvc("PerfMonMTSvc")
    # Set the job start time
    svcMgr.PerfMonMTSvc.wallTimeOffset = psutil.Process(os.getpid()).create_time() * 1000 # Get the job start time in ms
    # Set number of threads/slots
    svcMgr.PerfMonMTSvc.numberOfThreads = max(1,jp.ConcurrencyFlags.NumThreads())
    svcMgr.PerfMonMTSvc.numberOfSlots = max(1,jp.ConcurrencyFlags.NumConcurrentEvents())

###############################
# Load PerfMonMTAlg
###############################
from AthenaCommon.AlgSequence import AthSequencer
topSequence = AthSequencer("AthAlgSeq")
if not hasattr(topSequence, "PerfMonMTAlg"):
    from PerfMonComps.PerfMonCompsConf import PerfMonMTAlg
    topSequence += PerfMonMTAlg("PerfMonMTAlg")
    pass
