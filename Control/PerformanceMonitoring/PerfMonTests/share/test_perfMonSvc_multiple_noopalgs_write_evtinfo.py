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
    EVTMAX = 50000
    pass
theApp.EvtMax = EVTMAX

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------

from PerfMonComps.PerfMonCompsConf import PerfMon__CallGraphBuilderSvc
svcMgr += PerfMon__CallGraphBuilderSvc( "CallGraphSvc", OutputLevel = VERBOSE )
theApp.CreateSvc += [ svcMgr.CallGraphSvc.getFullJobOptName() ]
theAuditorSvc.Auditors += [ "PerfMon::CallGraphAuditor" ]
svcMgr.AuditorSvc.Auditors += [ "PerfMon::CallGraphAuditor" ]

## schedule a noop algorithm: it just print where it is
from PerfMonTests.PerfMonTestsConf import PerfMonTest__NoopAlg
for i in range( 100 ):
    topSequence += PerfMonTest__NoopAlg( "NoopAlg_%i" % i )

#---------------------------------------------------------------
# Pool Persistency
#---------------------------------------------------------------
from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
outStream = AthenaPoolOutputStream("OutStream")
outStream.ItemList  = [ "EventInfo#McEventInfo"]

if not 'OUTPUT' in dir():
  OUTPUT = "my.data.pool"
  pass
outStream.OutputFile = OUTPUT

## somehow better configure the AthenaPoolCnvSvc for our small
## persistency output job
svcMgr.AthenaPoolCnvSvc.CommitInterval = 1000

svcMgr.MessageSvc.OutputLevel = ERROR
#==============================================================
#
# End of job options file
#
###############################################################
