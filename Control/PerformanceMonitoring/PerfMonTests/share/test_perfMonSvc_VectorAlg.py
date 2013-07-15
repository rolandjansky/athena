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
jobproperties.PerfMonFlags.doPersistencyMonitoring = True

###############################
# Load perf service
###############################
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from PerfMonComps.JobOptCfg import PerfMonSvc
svcMgr += PerfMonSvc( "PerfMonSvc", OutputLevel = VERBOSE )

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
    EVTMAX = 3
    pass
theApp.EvtMax = EVTMAX

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------

## schedule a noop algorithm: it just print where it is
from PerfMonTests.PerfMonTestsConf import PerfMonTest__VectorAlg
pva = PerfMonTest__VectorAlg()
pva.VectorSize=10*1024*1024
pva.ToBeReserved=10*1024*1024
topSequence += pva
svcMgr.MessageSvc.OutputLevel = INFO

from PerfMonComps.PerfMonCompsConf import PerfMon__CallGraphBuilderSvc
svcMgr += PerfMon__CallGraphBuilderSvc( "CallGraphSvc" )
theApp.CreateSvc += [ svcMgr.CallGraphSvc.getFullJobOptName() ]
svcMgr.AuditorSvc.Auditors += [ "PerfMon::CallGraphAuditor" ]
#==============================================================
#
# End of job options file
#
###############################################################
