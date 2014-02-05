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

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
if not 'EVTMAX' in dir():
    EVTMAX = -1
    pass
theApp.EvtMax = EVTMAX

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
# Load "user algorithm"
#top algorithms to be run, and the libraries that house them

########
# Create a McEventCollection from a dumb ASCII file
#
if not 'MCEVENTKEY' in dir():
    MCEVENTKEY = "GEN_EVENT"
    pass
if not 'INPUT' in dir():
    INPUT = "hepmc.ascii"
    pass
from McParticleTools.McParticleToolsConf import HepMcReaderTool
from McParticleAlgs.McParticleAlgsConf   import GenEventAsciiReader
topSequence += GenEventAsciiReader(
    McReader = HepMcReaderTool( Input          = INPUT,
                                McEventsOutput = MCEVENTKEY )
    )

########
# Read the McEventCollection
#
from TruthExamples.TruthExamplesConf import DumpMC
topSequence += DumpMC( "ReadGenEvent", McEventKey = MCEVENTKEY )

#---------------------------------------------------------------
# Pool Persistency
#---------------------------------------------------------------
from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream

outStream = AthenaPoolOutputStream( "OutStream" )
outStream.ItemList  = [ "EventInfo#*" ]
outStream.ItemList += [ "McEventCollection#*" ]

if not "OUTPUT" in dir():
    OUTPUT = "mc.event.pool"
outStream.OutputFile = OUTPUT

#--------------------------------------------------------------
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
#--------------------------------------------------------------
#svcMgr.MessageSvc.OutputLevel = DEBUG

#==============================================================
#
# End of job options file
#
###############################################################
