###############################################################
#
# Job options file
#
#==============================================================

from AthenaCommon.AppMgr import theApp

if not 'doMonitoring' in dir():
    doMonitoring = True
    pass
from PerfMonComps.PerfMonFlags import jobproperties
jobproperties.PerfMonFlags.doMonitoring = doMonitoring
jobproperties.PerfMonFlags.doPersistencyMonitoring = True
jobproperties.PerfMonFlags.doDetailedMonitoring = True
jobproperties.PerfMonFlags.OutputFile = "truthjets.pmon.gz"

###############################
# Load perf service
###############################
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
from PerfMonComps.JobOptCfg import PerfMonSvc
svcMgr += PerfMonSvc( "PerfMonSvc", OutputLevel = INFO )

#--------------------------------------------------------------
# General Application Configuration options
#--------------------------------------------------------------
import AthenaCommon.AtlasUnixGeneratorJob

from AthenaCommon.Constants import VERBOSE,DEBUG,INFO,WARNING,ERROR
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
if not 'EVTMAX' in dir():
    EVTMAX = 1000
    pass
theApp.EvtMax = EVTMAX

#--------------------------------------------------------------
# Private Application Configuration options
#--------------------------------------------------------------

## import AthenaPoolCnvSvc.ReadAthenaPool
## if 'INPUT' not in dir():
##     INPUT = [ "mc.evt.pool" ]
## svcMgr.EventSelector.InputCollections = INPUT

####################
# Generate the event
####################
if not 'GENERATOR' in dir(): GENERATOR = "Pythia"
if not 'PROCESS' in dir():   PROCESS   = "ttbar"
from McParticleTests.tests import makeGenEvents
topSequence += makeGenEvents( genName    = GENERATOR,
                              genProcess = PROCESS,
                              cfgGenName = "EvGen" )
########################
# Build Jets from Truth
########################
from AthenaCommon.SystemOfUnits import MeV
from JetRec.JetRecConf import JetSpyAlgorithm
from JetSimTools.JetSimToolsConf import JetsFromTruthTool
def makeTruthJet( jetName ):
    jetAlg = JetSpyAlgorithm( "%sJetBuilder" % jetName )
    truthTool = JetsFromTruthTool(
        "%sTool" % jetName,
        MinPt        = 0.*MeV,
        MaxEta       = 5.,
        IncludeMuons = False,
        TruthCollectionName  = 'GEN_EVENT',
        OutputCollectionName = "%sJets" % jetName
        )
    jetAlg += truthTool
    jetAlg.AlgTools = [ truthTool.getFullName() ]
    return jetAlg

topSequence += makeTruthJet( "Kt4Truth" )
topSequence += makeTruthJet( "Kt6Truth" )
topSequence += makeTruthJet( "Cone4Truth" )
topSequence += makeTruthJet( "Cone7Truth" )

#---------------------------------------------------------------
# Pool Persistency
#---------------------------------------------------------------
from AthenaPoolCnvSvc.WriteAthenaPool import AthenaPoolOutputStream
outStream = AthenaPoolOutputStream("OutStream")
outStream.ItemList  = [ "EventInfo#McEventInfo"]
outStream.ItemList += [ "McEventCollection#GEN_EVENT" ]
outStream.ItemList += [ "JetCollection#Kt4TruthJets" ]
outStream.ItemList += [ "JetCollection#Kt6TruthJets" ]
outStream.ItemList += [ "JetCollection#Cone4TruthJets" ]
outStream.ItemList += [ "JetCollection#Cone7TruthJets" ]

if not 'OUTPUT' in dir():
  OUTPUT = "mc.jets.pool"
  pass
outStream.OutputFile = OUTPUT

from GaudiSvc.GaudiSvcConf import MessageSvc
svcMgr += MessageSvc( defaultLimit = 4000000 )
svcMgr.MessageSvc.OutputLevel  = ERROR

#==============================================================
#
# End of job options file
#
###############################################################
