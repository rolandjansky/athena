###############################################################
#
# Job options file
#
#==============================================================

#--------------------------------------------------------------
# General Application Configuration options
#--------------------------------------------------------------
import AthenaCommon.AtlasUnixGeneratorJob

from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

# import message levels (INFO/ERROR/...)
from AthenaCommon.Constants import *

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
    INPUT = [ "mc.event.pool" ]
svcMgr.EventSelector.InputCollections = INPUT

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
# Load "user algorithm"
#top algorithms to be run, and the libraries that house them

########
# Read the McEventCollection
#
if not 'MCEVENTKEY' in dir():
    MCEVENTKEY = "GEN_EVENT"
    pass
if 'DUMP' in dir():
    from TruthExamples.TruthExamplesConf import DumpMC
    topSequence += DumpMC( "ReadGenEvent", McEventKey = MCEVENTKEY )

########
# Dump the McEventCollection into a dumb ASCII file
#
if not 'OUTPUT' in dir():
    OUTPUT = "hepmc.root"
    pass
from McParticleTools.McParticleToolsConf import HepMcTupleWriterTool
from McParticleAlgs.McParticleAlgsConf   import GenEventTupleWriter
topSequence += GenEventTupleWriter(
    McReader = HepMcTupleWriterTool( OutputFile = OUTPUT,
                                     McEvents   = MCEVENTKEY )
    )

#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
#svcMgr.MessageSvc.OutputLevel = DEBUG

#==============================================================
#
# End of job options file
#
###############################################################
