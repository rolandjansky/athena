###############################################################
#
# Job options file
#
#==============================================================
import AthenaCommon.AtlasUnixStandardJob

#--------------------------------------------------------------
# General Application Configuration options
#--------------------------------------------------------------
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

# get a handle on the ServiceManager
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

# import message levels (INFO/ERROR/...)
from AthenaCommon.Constants import *
from AthenaCommon.AppMgr    import ServiceMgr as svcMgr
from AthenaCommon.AppMgr    import theApp,AuditorSvc

###############################
# Load perf service
###############################
from PerfMonComps.PerfMonFlags import jobproperties
jobproperties.PerfMonFlags.doMonitoring = True
jobproperties.PerfMonFlags.doPersistencyMonitoring = True
jobproperties.PerfMonFlags.OutputFile = "read.mcevt.perfmon.root"
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

if 'INPUT' not in dir():
    INPUT = [ "mc.event.pool" ]

svcMgr.EventSelector.InputCollections = INPUT

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
# Load "user algorithm"
#top algorithms to be run, and the libraries that house them

####################
# Dump the event into an ASCII file for reference comparison
###################
if 'DUMP' not in dir():
    DUMP = False
    pass
if DUMP:
    from McParticleTools.McParticleToolsConf import HepMcFloatWriterTool
    from McParticleAlgs.McParticleAlgsConf   import GenEventAsciiWriter
    topSequence += GenEventAsciiWriter(
        McWriter = HepMcFloatWriterTool(McEvents = "GEN_EVENT",
                                        Output   = "reaccessed.mc.event.txt"),
        OutputLevel = INFO
        )
    pass
if 'DUMPTUPLE' not in dir():
    DUMPTUPLE = False
    pass
if DUMPTUPLE:
    if not hasattr(svcMgr, 'THistSvc'):
        svcMgr += CfgMgr.THistSvc()
    theApp.CreateSvc += [ svcMgr.THistSvc.getFullName() ]
    if not 'TUPLEFILENAME' in dir():
        TUPLEFILENAME = 'reaccessed.mc.event.root'
        pass
    from McParticleTools.McParticleToolsConf import HepMcTupleWriterTool
    from McParticleAlgs.McParticleAlgsConf   import GenEventTupleWriter
    topSequence += GenEventTupleWriter(
        McWriter = HepMcTupleWriterTool( McEvents = "GEN_EVENT",
                                         Output   = TUPLEFILENAME ),
        OutputLevel = INFO
        )
    pass

#--------------------------------------------------------------
# Output options
#--------------------------------------------------------------
from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
outStream = AthenaPoolOutputStream("StreamEvGen")
outStream.ItemList  = [ "EventInfo#McEventInfo"]
outStream.ItemList += [ "McEventCollection#GEN_EVENT" ]

OUTPUT = os.path.join( os.path.dirname(INPUT[0]),
                       "reaccessed.%s" % os.path.basename(INPUT[0]) )
import os
outStream.OutputFile = OUTPUT
outStream.ForceRead = True  #force read of output data objs

## tweak the default commit interval
svcMgr.AthenaPoolCnvSvc.CommitInterval = 100

#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
svcMgr.MessageSvc.defaultLimit = 4000000
#svcMgr.MessageSvc.OutputLevel  = ERROR

#==============================================================
#
# End of job options file
#
###############################################################
