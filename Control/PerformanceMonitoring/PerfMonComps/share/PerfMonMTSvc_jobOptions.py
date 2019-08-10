###############################
# Print what we're doing
###############################
from AthenaCommon.Logging import logging
log = logging.getLogger("PerfMonMTSvc_jobOptions.py")
log.info("Setting up PerfMonMT...")


from AthenaCommon.AppMgr import ServiceMgr as svcMgr
if not hasattr(svcMgr, 'PerfMonMTSvc'):
    from PerfMonComps.MTJobOptCfg import PerfMonMTSvc
    svcMgr += PerfMonMTSvc( "PerfMonMTSvc"  )
    # To disable event loop monitoring, make false the following property 
    svcMgr.PerfMonMTSvc.doEventLoopMonitoring = True
    pass


