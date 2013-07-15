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
jobproperties.PerfMonFlags.doDetailedMonitoring = True

###############################
# Load perf service
###############################
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from PerfMonComps.JobOptCfg import PerfMonSvc
svcMgr += PerfMonSvc( "PerfMonSvc", OutputLevel = INFO )

#--------------------------------------------------------------
# General Application Configuration options
#--------------------------------------------------------------
import AthenaCommon.AtlasUnixGeneratorJob

from AthenaCommon.Constants import VERBOSE,DEBUG,INFO,WARNING,ERROR
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
if not 'EVTMAX' in dir():
    EVTMAX = 30
    pass
theApp.EvtMax = EVTMAX

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
svcMgr.MessageSvc.OutputLevel = ERROR

## schedule a noop algorithm: it just print where it is
from PerfMonTests.PerfMonTestsConf import PerfMonTest__MallocAlg
topSequence += PerfMonTest__MallocAlg( 
    "MallocAlg",
    OutputLevel = INFO
    )

#==============================================================
#
# End of job options file
#
###############################################################
