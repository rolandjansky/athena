###############################################################
#
# Job options file
#
#==============================================================
#
import AthenaCommon.AtlasUnixStandardJob

from AthenaCommon.Constants   import VERBOSE,DEBUG,INFO,WARNING,ERROR
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

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
   INPUT = ["my.data.pool"]
svcMgr.EventSelector.InputCollections = INPUT

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------
from AthenaServices.AthenaServicesConf import AtRndmGenSvc
svcMgr += AtRndmGenSvc()

if 'ALGMODE' not in dir():
    ALGMODE = 'cpp'
    
#####################################################
# WriteThinnedData
#####################################################
if ALGMODE == 'cpp':
   SlimmerAlg = CfgMgr.AthExThinning__SlimmerAlg
elif ALGMODE == 'py':
   from AthExThinning.Lib import PySlimmerAlg as SlimmerAlg
topSequence += SlimmerAlg(
   "Slimmer",
   Particles = "Particles_test1",
   FatObject = "FatObject_test1",
   OutputLevel = DEBUG )

#--------------------------------------------------------------
# POOL Persistency
#--------------------------------------------------------------
import AthenaPoolCnvSvc.WriteAthenaPool as wap

outStreams = []
for i in xrange(2):
   _outStreamName = "StreamUSR_%i" % i
   topSequence += CfgMgr.AthenaOutputStream(
      _outStreamName,
      WritingTool = "AthenaPoolOutputStreamTool"
      )
   outStreams += [getattr(topSequence, _outStreamName)]
   outStream   = outStreams[i]
   
   # Event Info
   outStream.ItemList  = [ "EventInfo#*" ]
   outStream.ItemList += [
      ##
      "AthExParticles#Particles_test1",
      ##
      "AthExDecay#TwoBodyDecay_test1",
      ##
      "AthExElephantino#PinkElephantino_test1",
      ##
      "AthExFatObject#FatObject_test1",
      ]

if 'OUTPUT' not in dir():
   OUTPUT = "slimmed.%s" % INPUT[0]
svcMgr.PoolSvc.CheckDictionary = True

# Stream's output file
outStreams[0].OutputFile = OUTPUT
outStreams[1].OutputFile = os.path.join(
   os.path.dirname(OUTPUT),
   'non.%s' % os.path.basename(OUTPUT)
   )
svcMgr.AthenaPoolCnvSvc.CommitInterval = 10

###############################
# Load thinning service
###############################
from AthenaServices.Configurables import ThinningSvc, createThinningSvc
## svcMgr += ThinningSvc( OutputLevel = VERBOSE,
##                        Streams = ['OutStream_0'] )
svcMgr += createThinningSvc(svcName="ThinningSvc", outStreams=[outStreams[0]])

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

