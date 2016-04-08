###############################################################
#
# Job options file
#
#==============================================================

# basic job configuration
import AthenaCommon.AtlasUnixStandardJob

# get a handle on the job main sequence
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
jobproperties.PerfMonFlags.OutputFile = "read.mcaod.perfmon.pmon.gz"
from PerfMonComps.JobOptCfg import PerfMonSvc
svcMgr += PerfMonSvc( "PerfMonSvc", OutputLevel = INFO )

#--------------------------------------------------------------
# Load POOL support
#--------------------------------------------------------------
import AthenaPoolCnvSvc.ReadAthenaPool

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
if not 'EVTMAX' in dir():
    EVTMAX = -1
    pass
theApp.EvtMax = EVTMAX

if not 'INPUT' in dir():
    INPUT = [ "mc.aod.pool" ]
svcMgr.EventSelector.InputCollections = INPUT

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
    topSequence.McAodValidation.SpclMcValidation.McEvents       = "GEN_EVENT"
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
        TUPLEFILENAME = 'reaccessed.mc.aod.root'
        pass
    from McParticleTools.McParticleToolsConf import McAodTupleWriterTool
    from McParticleAlgs.McParticleAlgsConf   import McAodTupleWriter
    topSequence += McAodTupleWriter(
        McWriter = McAodTupleWriterTool( TruthParticles = "SpclMC",
                                         Output         = TUPLEFILENAME ),
        OutputLevel = DEBUG
        )
    pass
   
#--------------------------------------------------------------
# Output options
#--------------------------------------------------------------
from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
outStream = AthenaPoolOutputStream("StreamAOD")
import os
outStream.OutputFile = os.path.join( \
    os.path.dirname(INPUT[0]),
    "reaccessed.%s" % os.path.basename(INPUT[0]) )
outStream.ForceRead = True

outStream.ItemList += [ "EventInfo#McEventInfo" ]
outStream.ItemList += [ "McEventCollection#GEN_EVENT" ]
outStream.ItemList += [ "McEventCollection#GEN_AOD" ]
outStream.ItemList += [ "TruthParticleContainer#SpclMC" ]
outStream.ItemList += [ "TruthEtIsolationsContainer#TruthEtIsol_GEN_EVENT" ]
outStream.ItemList += [ "TruthEtIsolationsContainer#TruthEtIsol_GEN_AOD" ]

## tweak the default commit interval
svcMgr.AthenaPoolCnvSvc.CommitInterval = 100

svcMgr.MessageSvc.defaultLimit = 4000000
svcMgr.MessageSvc.OutputLevel  = ERROR

#==============================================================
#
# End of job options file
#
###############################################################
