###############################################################
#
# Job options file
#
#==============================================================

#--------------------------------------------------------------
# General Application Configuration options
#--------------------------------------------------------------
import AthenaCommon.AtlasUnixGeneratorJob
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

# import message levels (INFO/ERROR/...)
from AthenaCommon.Constants import *
from AthenaCommon.AppMgr    import ServiceMgr as svcMgr
from AthenaCommon.AppMgr    import theApp,AuditorSvc

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------

###############################
# Load perf service
###############################
from PerfMonComps.PerfMonFlags import jobproperties
jobproperties.PerfMonFlags.doMonitoring = True
jobproperties.PerfMonFlags.doPersistencyMonitoring = True
jobproperties.PerfMonFlags.OutputFile = "mcevt.perfmon.pmon.gz"
from PerfMonComps.JobOptCfg import PerfMonSvc
svcMgr += PerfMonSvc( "PerfMonSvc", OutputLevel = INFO )

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
if not 'EVTMAX' in dir():
    EVTMAX = 5
    pass
theApp.EvtMax = EVTMAX

from RngComps.RngCompsConf import AtRndmGenSvc
svcMgr += AtRndmGenSvc(
    Seeds = [ "PYTHIA 4789899 989240512", "PYTHIA_INIT 820021 2347532",
              "JIMMY 390020611 821000366", "JIMMY_INIT 820021 2347532",
              "HERWIG 390020611 821000366", "HERWIG_INIT 820021 2347532"
              ]
    )

####################
# Generate the event
####################
if not 'GENERATOR' in dir():
    GENERATOR = "Pythia"
if not 'PROCESS' in dir():
    PROCESS = "ttbar"
from McParticleTests.tests import makeGenEvents
topSequence += makeGenEvents( genName    = GENERATOR,
                              genProcess = PROCESS,
                              cfgGenName = "EvGen" )

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
        McWriter = HepMcFloatWriterTool( McEvents = "GEN_EVENT",
                                         Output   = "mc.event.txt" ),
        OutputLevel = INFO
        )

if 'DUMPTUPLE' not in dir():
    DUMPTUPLE = False
    pass
if DUMPTUPLE:
    if not hasattr(svcMgr, 'THistSvc'):
        svcMgr += CfgMgr.THistSvc()
    theApp.CreateSvc += [ svcMgr.THistSvc.getFullName() ]
    if not 'TUPLEFILENAME' in dir():
        TUPLEFILENAME = 'mc.event.root'
        pass
    from McParticleTools.McParticleToolsConf import HepMcTupleWriterTool
    from McParticleAlgs.McParticleAlgsConf   import GenEventTupleWriter
    topSequence += GenEventTupleWriter(
        McWriter = HepMcTupleWriterTool( McEvents = "GEN_EVENT",
                                         Output   = TUPLEFILENAME ),
        OutputLevel = DEBUG
        )
    pass

#---------------------------------------------------------------
# Pool Persistency
#---------------------------------------------------------------
from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
outStream = AthenaPoolOutputStream("StreamEvGen", noTag=True)
outStream.ItemList  = [ "EventInfo#McEventInfo"]
outStream.ItemList += [ "McEventCollection#GEN_EVENT" ]

if not 'OUTPUT' in dir():
  OUTPUT = "mc.event.pool"
  pass
outStream.OutputFile = OUTPUT

svcMgr.MessageSvc.defaultLimit = 4000000
svcMgr.MessageSvc.OutputLevel  = INFO

#==============================================================
#
# End of job options file
#
###############################################################
