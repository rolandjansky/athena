###############################################################
#
# Job options file
#
#==============================================================

###############################################################
###############################################################
__doc__ = """
Note: This job options assumes you have run the following commands:
 1) create an EVGEN file (typically done in a previous/next release
athena -c'EVTMAX=10' McParticleTests/iotest_WriteGenEvent_jobOptions.py

 2) create an ASCII-EVGEN file
athena -c'OUTPUT="/tmp/hepmc.1.ascii"' McParticleAlgs/GenEventAsciiWriter_jobOptions.py

 3) create another ASCII-EVGEN file
athena -c'OUTPUT="/tmp/hepmc.2.ascii"' McParticleAlgs/GenEventAsciiWriter_jobOptions.py

 4) read all these ASCII files (ie: run this jobO)
athena McAsciiEventSelector/Example_McAsciiReader_jobOptions.py 
"""
###############################################################
###############################################################

#--------------------------------------------------------------
# General Application Configuration options
#--------------------------------------------------------------
from AthenaCommon.AppMgr import (ServiceMgr as svcMgr,
                                 theApp)

## configure the special Ascii event selector
import McAsciiEventSelector.ReadMcAscii

###################
## User parameters
if not 'MCEVENTKEY' in dir():
    MCEVENTKEY = 'GEN_EVENT'
    pass

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
if not 'EVTMAX' in dir():
    EVTMAX = -1
    pass
theApp.EvtMax = EVTMAX

if not 'INPUT' in dir():
    INPUT = [ 
    "/tmp/hepmc.1.ascii",
    "/tmp/hepmc.2.ascii",
    ]
svcMgr.EventSelector.InputCollections = INPUT
svcMgr.EventSelector.OutputLevel = VERBOSE
svcMgr.McAsciiCnvSvc.OutputLevel = VERBOSE
svcMgr.McAsciiCnvSvc.McEventsOutput = MCEVENTKEY

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
# Load "user algorithm"
#top algorithms to be run, and the libraries that house them

from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

########
# Read the McEventCollection
#
if not 'DUMP' in dir():
    DUMP = True
    pass
if DUMP:
    job += CfgMgr.DumpMC( "ReadGenEvent",
                          McEventKey = MCEVENTKEY )

#---------------------------------------------------------------
# Pool Persistency
#---------------------------------------------------------------
from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
outStream = AthenaPoolOutputStream("OutStream", noTag=True)
outStream.ItemList  = [
    "EventInfo#McEventInfo",
    "McEventCollection#"+MCEVENTKEY
    ]

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
