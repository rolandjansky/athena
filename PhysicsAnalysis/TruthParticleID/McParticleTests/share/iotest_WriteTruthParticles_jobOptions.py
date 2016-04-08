###############################################################
#
# Job options file
#
#==============================================================

#--------------------------------------------------------------
# General Application Configuration options
#--------------------------------------------------------------
import AthenaCommon.AtlasUnixStandardJob

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

# import message levels (INFO/ERROR/...)
from AthenaCommon.Constants import *
from AthenaCommon.AppMgr    import ServiceMgr as svcMgr
from AthenaCommon.AppMgr    import theApp

###############################
# Load perf service
###############################
from PerfMonComps.PerfMonFlags import jobproperties
jobproperties.PerfMonFlags.doMonitoring = True
jobproperties.PerfMonFlags.doPersistencyMonitoring = True
jobproperties.PerfMonFlags.OutputFile = "mcaod.perfmon.pmon.gz"
from PerfMonComps.JobOptCfg import PerfMonSvc
svcMgr += PerfMonSvc(
    "PerfMonSvc",
    OutputLevel  = INFO
    )

if not 'EVTMAX' in dir():
    EVTMAX = 5
    pass
theApp.EvtMax = EVTMAX

#--------------------------------------------------------------
# Load POOL support
#--------------------------------------------------------------
import AthenaPoolCnvSvc.ReadAthenaPool

if not 'INPUT' in dir():
    INPUT = [ "mc.event.pool" ]
svcMgr.EventSelector.InputCollections = INPUT

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
if not 'EVTMAX' in dir():
    EVTMAX = 5
    pass
theApp.EvtMax = EVTMAX

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
    OutputLevel = DEBUG,
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

########
# Read the TruthParticleContainer
# Validates some aspects of both TPContainer and McEventCollections
#
if 'DUMP' not in dir():
    DUMP = False
    pass
if DUMP:
    from McParticleTools.McParticleToolsConf import SpclMcValidationTool
    from McParticleAlgs.McParticleAlgsConf import McAodValidationAlg
    topSequence += McAodValidationAlg(
        "McAodValidation",
        ValidationTools = [ "SpclMcValidationTool/SpclMcValidation" ]
        )
    topSequence.McAodValidation += SpclMcValidationTool("SpclMcValidation")
    topSequence.McAodValidation.SpclMcValidation.McEvents       = "GEN_AOD"
    topSequence.McAodValidation.SpclMcValidation.TruthParticles = "SpclMC"
    topSequence.McAodValidation.SpclMcValidation.OutputLevel    = INFO
   
if 'DUMPTUPLE' not in dir():
    DUMPTUPLE = False
    pass
if DUMPTUPLE:
    if not hasattr(svcMgr, 'THistSvc'):
        svcMgr += CfgMgr.THistSvc()
    theApp.CreateSvc += [ svcMgr.THistSvc.getFullName() ]
    if not 'TUPLEFILENAME' in dir():
        TUPLEFILENAME = 'mc.aod.root'
        pass
    from McParticleTools.McParticleToolsConf import McAodTupleWriterTool
    from McParticleAlgs.McParticleAlgsConf   import McAodTupleWriter
    topSequence += McAodTupleWriter(
        McWriter = McAodTupleWriterTool( TruthParticles = "SpclMC",
                                         Output         = TUPLEFILENAME ),
        OutputLevel = DEBUG
        )
    pass
   
#---------------------------------------------------------------
# Pool Persistency
#---------------------------------------------------------------
from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
outStream = AthenaPoolOutputStream("StreamAOD")
outStream.ItemList  = [ "EventInfo#McEventInfo" ]
outStream.ItemList += [ "McEventCollection#GEN_EVENT" ]
outStream.ItemList += [ "McEventCollection#GEN_AOD" ]
outStream.ItemList += [ "TruthParticleContainer#SpclMC" ]
outStream.ItemList += [ "TruthEtIsolationsContainer#TruthEtIsol_GEN_EVENT" ]
outStream.ItemList += [ "TruthEtIsolationsContainer#TruthEtIsol_GEN_AOD" ]

if 'OUTPUT' not in dir():
  OUTPUT = "mc.aod.pool"
  pass
outStream.OutputFile = OUTPUT
outStream.ForceRead        = True  #force read of output data objs

## tweak the default commit interval
svcMgr.AthenaPoolCnvSvc.CommitInterval = 100

svcMgr.MessageSvc.defaultLimit = 4000000
svcMgr.MessageSvc.OutputLevel  = ERROR

#==============================================================
#
# End of job options file
#
###############################################################
