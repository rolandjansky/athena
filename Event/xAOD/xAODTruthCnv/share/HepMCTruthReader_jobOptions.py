# Set up the reading of the input AOD:
import AthenaPoolCnvSvc.ReadAthenaPool
svcMgr.EventSelector.InputCollections = ["evnt.pool.root"]
 
# Access the algorithm sequence:
from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()
 
from xAODTruthCnv.xAODTruthCnvConf import HepMCTruthReader
alg2 = HepMCTruthReader()
alg2.OutputLevel = INFO
alg2.HepMCContainerName = "GEN_EVENT"
theJob += alg2
 
# Do some additional tweaking:
from AthenaCommon.AppMgr import theApp
theApp.EvtMax = -1
ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.defaultLimit = 1000000
