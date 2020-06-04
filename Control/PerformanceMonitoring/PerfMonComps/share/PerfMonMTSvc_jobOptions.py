###############################
# Print what we're doing
###############################
from AthenaCommon.Logging import logging
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from AthenaCommon.ConcurrencyFlags import jobproperties as jp
import os,psutil

log = logging.getLogger("PerfMonMTSvc_jobOptions.py")
log.info("Setting up PerfMonMT...")


###############################
# Load PerfMonMTSvc
###############################
if not hasattr(svcMgr, 'PerfMonMTSvc'):
    from PerfMonComps.MTJobOptCfg import PerfMonMTSvc
    svcMgr += PerfMonMTSvc("PerfMonMTSvc")
    # Report results into json by default
    svcMgr.PerfMonMTSvc.reportResultsToJSON = True
    # Enable event loop monitoring by default
    svcMgr.PerfMonMTSvc.doEventLoopMonitoring = True
    # Disable component level monitoring by default
    svcMgr.PerfMonMTSvc.doComponentLevelMonitoring = False
    # Enable detailed table printing by default
    svcMgr.PerfMonMTSvc.printDetailedTables = True
    # Print only the top 50 components (sorted by CPU time) by default
    svcMgr.PerfMonMTSvc.printNComps = 50
    # Configure the check point sequence in the event loop monitoring.
    # By default common difference is the number of threads with which the job is running
    svcMgr.PerfMonMTSvc.checkPointType = "Arithmetic" 
    svcMgr.PerfMonMTSvc.checkPointFactor = max(10,jp.ConcurrencyFlags.NumThreads())
    svcMgr.PerfMonMTSvc.wallTimeOffset = psutil.Process(os.getpid()).create_time() * 1000 # Get the job start time in ms
    # Set number of threads/slots
    svcMgr.PerfMonMTSvc.numberOfThreads = max(1,jp.ConcurrencyFlags.NumThreads())
    svcMgr.PerfMonMTSvc.numberOfSlots = max(1,jp.ConcurrencyFlags.NumConcurrentEvents())

###############################
# Load PerfMonMTAlg
###############################
from AthenaCommon.AlgSequence import AthSequencer
topSequence = AthSequencer("AthAlgSeq")
if not hasattr(topSequence, "PerfMonMTSvcAlg"):
    from PerfMonComps.PerfMonCompsConf import PerfMonMTAlg
    topSequence += PerfMonMTAlg("PerfMonMTAlg")
    pass
