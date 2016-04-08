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

# import message levels (INFO/ERROR/...)
from AthenaCommon.Constants import *
from AthenaCommon.AppMgr    import ServiceMgr as svcMgr
from AthenaCommon.AppMgr    import theApp

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

# get a handle on the ServiceManager
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
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
    from McParticleTools.McParticleToolsConf import HepMcWriterTool
    from McParticleAlgs.McParticleAlgsConf   import GenEventAsciiWriter
    topSequence += GenEventAsciiWriter(
        McWriter = HepMcWriterTool( McEvents = "GEN_EVENT",
                                    Output   = "reaccessed.mc.event.txt" ),
        OutputLevel = INFO
        )
    svcMgr += CfgMgr.THistSvc()
    pass


#--------------------------------------------------------------
# Output options
#--------------------------------------------------------------
from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
include( "ParticleBuilderOptions/McAOD_PoolCnv_jobOptions.py")

outStream = AthenaPoolOutputStream("OutStream")
outStream.ItemList  = [ "EventInfo#*"]
outStream.ItemList += [ "McEventCollection#*" ]

if 'OUTPUT' not in dir():
    OUTPUT = os.path.join( os.path.dirname(INPUT[0]),
                           "reaccessed.%s" % os.path.basename(INPUT[0]) )
import os
outStream.OutputFile = OUTPUT
outStream.ForceRead = True  #force read of output data objs

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
