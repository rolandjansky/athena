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
svcMgr += PerfMonSvc( "PerfMonSvc",
                      OutputLevel  = INFO,
                      IoContainers = [ "EventInfo#McEventInfo",
                                       "AthExParticles#Particles",
                                       "AthExDecay#TwoBodyDecay",
                                       "AthExElephantino#PinkElephantino" ]
                      )

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

###############################
# Create some 4-vectors
###############################
from AthExThinning.AthExThinningAlgsConf import AthExThinning__CreateData
topSequence += AthExThinning__CreateData(
    "CreateData",
    ParticlesOutput = "Particles",
    NbrOfParticles  = 10,
    OutputLevel = ERROR )

#---------------------------------------------------------------
# Pool Persistency
#---------------------------------------------------------------
from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
outStream = AthenaPoolOutputStream("OutStream", noTag=True)
outStream.ItemList  = [ "EventInfo#McEventInfo"]
outStream.ItemList += [ "AthExParticles#Particles" ]
outStream.ItemList += [ "AthExDecay#TwoBodyDecay" ]
outStream.ItemList += [ "AthExElephantino#PinkElephantino" ]

if not 'OUTPUT' in dir():
  OUTPUT = "my.data.pool"
  pass
outStream.OutputFile = OUTPUT

## somehow better configure the AthenaPoolCnvSvc for our small
## persistency output job

svcMgr += CfgMgr.MessageSvc( defaultLimit = 4000000,
                             OutputLevel = ERROR )

#==============================================================
#
# End of job options file
#
###############################################################
