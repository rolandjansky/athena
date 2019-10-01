###############################
# Print what we're doing
###############################
from AthenaCommon.Logging import logging
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

log = logging.getLogger("PerfMonMTSvc_jobOptions.py")
log.info("Setting up PerfMonMT...")

if not hasattr(svcMgr, 'PerfMonMTSvc'):
    from PerfMonComps.MTJobOptCfg import PerfMonMTSvc
    svcMgr += PerfMonMTSvc("PerfMonMTSvc")
    # Set up the service as the first service to be created:
    theApp.CreateSvc.insert(0, "PerfMonMTSvc/PerfMonMTSvc")
    # Disable event loop monitoring by default
    svcMgr.PerfMonMTSvc.doEventLoopMonitoring = True
    pass
