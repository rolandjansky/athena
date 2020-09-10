import AthenaPoolCnvSvc.ReadAthenaPool
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.AppMgr import ServiceMgr, ToolSvc, theApp
from AthenaCommon import CfgMgr

filelist = [os.environ["ASG_TEST_FILE_MC"]]
ServiceMgr.EventSelector.InputCollections = filelist

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

# Instantiate the tool
testMVATool = False
if testMVATool:
    tvaTool = None
else:
    tvaTool = CfgMgr.CP__TrackVertexAssociationTool("TrackVertexAssociationTool",
                                                    WorkingPoint = "Nominal")
ToolSvc += tvaTool

# Instantiate the alg
testAlg = CfgMgr.TrackVertexAssoTestAlg(TrackContainer  = "InDetTrackParticles",
                                        VertexContainer = "PrimaryVertices",
                                        TVATool         = tvaTool)
topSequence += testAlg

if not hasattr(svcMgr, 'THistSvc'):
    from GaudiSvc.GaudiSvcConf import THistSvc
    svcMgr += THistSvc()
svcMgr.THistSvc.Output += [("ANALYSIS DATAFILE='TrackVertexAssoTestAlg.outputs.root' OPT='RECREATE'")] # Default stream: "ANALYSIS"
svcMgr.THistSvc.MaxFileSize = -1

theApp.EvtMax = 100
# testAlg.OutputLevel = DEBUG
# theApp.setOutputLevel(DEBUG)
ServiceMgr.EventSelector.SkipEvents = 0
ServiceMgr.MessageSvc.defaultLimit = 999
