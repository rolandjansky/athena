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
    EVTMAX = 1000
    pass
theApp.EvtMax = EVTMAX

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------

####################
# Generate the event
####################
if not 'GENERATOR' in dir(): GENERATOR = "Pythia"
if not 'PROCESS' in dir():   PROCESS   = "ttbar"
from McParticleTests.tests import makeGenEvents
topSequence += makeGenEvents( genName    = GENERATOR,
                              genProcess = PROCESS,
                              cfgGenName = "EvGen" )

##########################
# Configure McAod options
from McParticleAlgs.McAodFlags import jobproperties as jp
jp.McAodFlags.doTruthEtIsolations = True

########
# Create the AOD McEventCollection
#
from McParticleAlgs.JobOptCfg import McAodBuilder
from McParticleTools.McParticleToolsConf import NoopFilterTool
from McParticleTools.McParticleToolsConf import TruthParticleCnvTool
topSequence += McAodBuilder(
    "McAodBuilder",
    OutputLevel = INFO,
    FilterTool = NoopFilterTool(
                   McEvents = "GEN_EVENT",
                   DoEtIsolations = jp.McAodFlags.doTruthEtIsolations()
                   ),
    CnvTool = TruthParticleCnvTool(
                   McEvents = "GEN_AOD",
                   TruthParticlesOutput = "SpclMC",
                   DoEtIsolations = jp.McAodFlags.doTruthEtIsolations()
                   )
    )

#---------------------------------------------------------------
# Pool Persistency
#---------------------------------------------------------------
from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
outStream = AthenaPoolOutputStream("OutStream")
outStream.ItemList  = [ "EventInfo#McEventInfo"]
outStream.ItemList += [ "McEventCollection#GEN_EVENT" ]
outStream.ItemList += [ "McEventCollection#GEN_AOD" ]
outStream.ItemList += [ "TruthParticleContainer#SpclMC" ]
outStream.ItemList += [ "TruthEtIsolationsContainer#TruthEtIsol_GEN_EVENT" ]

if not 'OUTPUT' in dir():
  OUTPUT = "my.data.pool"
  pass
outStream.OutputFile = OUTPUT

from GaudiSvc.GaudiSvcConf import MessageSvc
svcMgr += MessageSvc( defaultLimit = 4000000 )
svcMgr.MessageSvc.OutputLevel  = WARNING

#==============================================================
#
# End of job options file
#
###############################################################
