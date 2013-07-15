###############################################################
#
# Job options file
#
#==============================================================

from AthenaCommon.AppMgr import theApp

###############################
# Load perf service
###############################
if not 'doMonitoring' in dir():
    doMonitoring = True
    pass
from PerfMonComps.PerfMonFlags import jobproperties
jobproperties.PerfMonFlags.doMonitoring = doMonitoring
jobproperties.PerfMonFlags.doPersistencyMonitoring = True
jobproperties.PerfMonFlags.doDetailedMonitoring = True

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
    EVTMAX = 10000
    pass
theApp.EvtMax = EVTMAX

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
svcMgr.MessageSvc.OutputLevel = ERROR

## schedule a basic algorithm: it just puts stuff into StoreGate
from PerfMonTests.PerfMonTestsConf import PerfMonTest__BasicAlg
for i in range( 10 ):
    topSequence += PerfMonTest__BasicAlg( "BasicAlg_%i" % i,
                                          DataSize = (i+1) * 10000 )

#==============================================================
#
# End of job options file
#
###############################################################
