###############################################################
#
# Job options file
#
#==============================================================
#

from AthenaCommon.AppMgr      import ServiceMgr as svcMgr
from AthenaCommon.Constants   import VERBOSE,DEBUG,INFO,WARNING,ERROR
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

from PartPropSvc.PartPropSvcConf import PartPropSvc
svcMgr += PartPropSvc()

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
# Number of events to be processed
if 'EVTMAX' not in dir() :
   EVTMAX = -1
   pass
theApp.EvtMax = EVTMAX

#--------------------------------------------------------------
# Load POOL support
#--------------------------------------------------------------
import AthenaPoolCnvSvc.ReadAthenaPool
if 'INPUT' not in dir():
   INPUT = ["thinned.my.data.pool"]
svcMgr.EventSelector.InputCollections = INPUT

svcMgr.PoolSvc.ReadCatalog = [ 'xmlcatalog_file:PoolFileCatalog.xml' ]
svcMgr.PoolSvc.WriteCatalog = 'xmlcatalog_file:PoolFileCatalog.xml'

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
from RngComps.RngCompsConf import AtRndmGenSvc
svcMgr += AtRndmGenSvc()

#####################################################
# ReadThinnedData
# --> read "swiss" DataVector
#####################################################
from AthExThinning.AthExThinningAlgsConf import AthExThinning__ReadThinnedData
topSequence += AthExThinning__ReadThinnedData(
   "ReadThinnedData",
   Particles   = "Particles",
   Decay       = "TwoBodyDecay",
   Elephantino = "PinkElephantino",
   OutputLevel = DEBUG
   )

#--------------------------------------------------------------
# POOL Persistency
#--------------------------------------------------------------
import AthenaPoolCnvSvc.WriteAthenaPool as wap
outStream = wap.AthenaPoolOutputStream("StreamUSR", noTag=True)

if 'OUTPUT' not in dir():
   OUTPUT = "reaccessed.%s" % INPUT[0]

# Stream's output file
outStream.OutputFile = OUTPUT

# Event Info
outStream.ItemList  = [ "EventInfo#*" ]
outStream.ItemList += [ "AthExParticles#*" ]
outStream.ItemList += [ "AthExDecay#*" ]
outStream.ItemList += [ "AthExElephantino#*" ]

##############################################################
#
#            Customise OutputLevel
#
##############################################################

# OUTPUT PRINTOUT LEVEL
# Set output level threshold (2=DEBUG, 3=INFO, 4=WARNING, 5=ERROR, 6=FATAL )
svcMgr.MessageSvc.useColors                 = False
svcMgr.MessageSvc.defaultLimit              = 4000000
svcMgr.MessageSvc.OutputLevel               = ERROR

#==============================================================
#
# End of job options file
#
###############################################################

