###############################
# Print what we're doing
###############################
from AthenaCommon.Logging import logging
log = logging.getLogger("PerfMonMTSvc_jobOptions.py")
log.info("Setting up PerfMonMT...")

from AthenaCommon.ConcurrencyFlags import jobproperties as jp


from AthenaCommon.AppMgr import ServiceMgr as svcMgr
if not hasattr(svcMgr, 'PerfMonMTSvc'):
    from PerfMonComps.MTJobOptCfg import PerfMonMTSvc
    svcMgr += PerfMonMTSvc( "PerfMonMTSvc"  )
    # Set up the service as the first service to be created:
    theApp.CreateSvc.insert( 0, "PerfMonMTSvc/PerfMonMTSvc" )
    # Disable event loop monitoring by default
    svcMgr.PerfMonMTSvc.doEventLoopMonitoring = False
    # Configure the check point sequence in the event loop monitoring.
    # By default common difference is the number of threads with which the job is running
    svcMgr.PerfMonMTSvc.checkPointType = "Geometric" 
    svcMgr.PerfMonMTSvc.checkPointFactor = jp.ConcurrencyFlags.NumThreads()

    svcMgr.PerfMonMTSvc.nThreads = jp.ConcurrencyFlags.NumThreads()
    pass



