from Valkyrie.JobOptCfg import ValgrindSvc
from AthenaCommon.AppMgr import ServiceMgr as svcMgr
svcMgr+=ValgrindSvc(OutputLevel=DEBUG,ProfiledAlgs=[],IgnoreFirstNEvents=1)
