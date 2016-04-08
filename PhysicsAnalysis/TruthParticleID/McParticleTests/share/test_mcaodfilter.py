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
job = AlgSequence()

# get a handle on the ServiceManager
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

# import message levels (INFO/ERROR/...)
from AthenaCommon import Constants as Units
from AthenaCommon import Constants as Lvl
from AthenaCommon.AppMgr    import ServiceMgr as svcMgr
from AthenaCommon.AppMgr    import theApp,AuditorSvc

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

## schedule our algorithm
job += CfgMgr.McAod__McTopAna(
    "mctopana",
    McEvent = "GEN_EVENT",
    FilteredMcEvent = "MyGEN_EVENT",
    OutputLevel=Lvl.INFO,
    )

## output ntuple configuration
if not hasattr(svcMgr, 'THistSvc'):
    svcMgr += CfgMgr.THistSvc()
    theApp.CreateSvc += [ svcMgr.THistSvc.getFullName() ]
if not 'TUPLEFILENAME' in dir():
    TUPLEFILENAME = 'mcaod.ttbar.root'
    pass
svcMgr.THistSvc.Output += [
    "mcaod DATAFILE='%s' OPT='RECREATE'" % TUPLEFILENAME
    ]

#--------------------------------------------------------------
# Output options
#--------------------------------------------------------------
from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
outStream = AthenaPoolOutputStream("StreamEvGen")
outStream.ItemList  = [
    "EventInfo#*",
    "McEventCollection#MyGEN_EVENT",
    "TruthParticleContainer#*",
    ]

OUTPUT = os.path.join( os.path.dirname(INPUT[0]),
                       "filtered.%s" % os.path.basename(INPUT[0]) )
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
