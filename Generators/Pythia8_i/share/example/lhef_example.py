#import AthenaCommon.AtlasUnixGeneratorJob
from AthenaCommon.AppMgr import ServiceMgr as svcMgr

#--------------------------------------------------------------
# Event related parameters
#--------------------------------------------------------------
# Number of events to be processed (default is 10)
theApp.EvtMax = 10
#--------------------------------------------------------------
# Algorithms Private Options
#--------------------------------------------------------------
from AthenaServices.AthenaServicesConf import AtRndmGenSvc
ServiceMgr += AtRndmGenSvc()
ServiceMgr.AtRndmGenSvc.Seeds = ["PYTHIA8 4789899 989240512",
                                 "PYTHIA8_INIT 820021 2347532"]

svcMgr.MessageSvc.OutputLevel = INFO

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()
from Pythia8_i.Pythia8_iConf import Pythia8_i

topSequence+=Pythia8_i()
topSequence.Pythia8_i.CollisionEnergy = 7000

topSequence.Pythia8_i.LHEFile = "Powheg.ZMu.MC11.events"


from TruthExamples.TruthExamplesConf import DumpMC
topSequence += DumpMC()


