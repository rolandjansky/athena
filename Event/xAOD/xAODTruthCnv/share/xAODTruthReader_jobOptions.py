# Set up the reading of the input AOD:
import AthenaPoolCnvSvc.ReadAthenaPool
svcMgr.EventSelector.InputCollections = ["xAOD.pool.root"]
 
# Access the algorithm sequence:
from AthenaCommon.AlgSequence import AlgSequence
theJob = AlgSequence()
 
from xAODTruthCnv.xAODTruthCnvConf import xAODReader__xAODTruthReader
alg2 = xAODReader__xAODTruthReader()
alg2.OutputLevel = INFO
theJob += alg2
 
# Do some additional tweaking:
from AthenaCommon.AppMgr import theApp
theApp.EvtMax = -1
ServiceMgr.MessageSvc.OutputLevel = INFO
ServiceMgr.MessageSvc.defaultLimit = 1000000
