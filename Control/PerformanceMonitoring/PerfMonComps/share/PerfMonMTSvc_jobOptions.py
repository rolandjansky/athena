###############################
# Print what we're doing
###############################
from AthenaCommon.Logging import logging
log = logging.getLogger("PerfMonMTSvc_jobOptions.py")
log.info("Setting up PerfMonMT...")

###############################
# Configure AuditorSvc if needed
###############################
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
if not hasattr(svcMgr, 'AuditorSvc'):
    from GaudiSvc.GaudiSvcConf import AuditorSvc
    svcMgr += AuditorSvc()

###############################
# Load PerfMonMTSvc
###############################
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
# Turn-on monitoring only for
# algorithms for testing
###############################
from AthenaCommon.AppMgr import theApp
theApp.AuditAlgorithms = True
theApp.AuditTools      = False
theApp.AuditServices   = False
