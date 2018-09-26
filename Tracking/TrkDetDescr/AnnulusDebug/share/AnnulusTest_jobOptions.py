from AthenaCommon.AlgSequence import AlgSequence
from AthenaCommon.AppMgr import ToolSvc,theApp,ServiceMgr
ServiceMgr.MessageSvc.defaultLimit = 100000
topSequence = AlgSequence()

from AnnulusDebug.AnnulusDebugConf import AnnulusTestAlg

topSequence += AnnulusTestAlg(OutputLevel=VERBOSE)

theApp.EvtMax = 1
