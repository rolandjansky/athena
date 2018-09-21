import AthenaPoolCnvSvc.ReadAthenaPool
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.AppMgr import ServiceMgr, ToolSvc, theApp
from AthenaCommon import CfgMgr

#filelist = ['/tmp/boliu/AOD.04607224._000001.pool.root.1']
filelist = [os.environ["ASG_TEST_FILE_MC"]]

ServiceMgr.EventSelector.InputCollections = filelist
#ServiceMgr.EventSelector.InputCollections =[] 

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

# instantiate the tool
tvatool=CfgMgr.CP__TrackVertexAssociationTool("TrackVertexAssociationTool",
                                              WorkingPoint="Nominal")
ToolSvc += tvatool

testAlg=CfgMgr.TrackVertexAssoTestAlg(
        TrackContainer="InDetTrackParticles",
        VertexContainer="PrimaryVertices",
        TVATool=tvatool
        )

topSequence += testAlg

theApp.EvtMax = 100
#testAlg.OutputLevel=DEBUG
#theApp.setOutputLevel(DEBUG)
ServiceMgr.EventSelector.SkipEvents = 0
ServiceMgr.MessageSvc.defaultLimit = 999
