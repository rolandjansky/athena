import AthenaPoolCnvSvc.ReadAthenaPool
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
from AthenaCommon.AppMgr import ServiceMgr, ToolSvc, theApp
from AthenaCommon import CfgMgr

filelist = ['/tmp/boliu/AOD.04607224._000001.pool.root.1']

ServiceMgr.EventSelector.InputCollections = filelist
#ServiceMgr.EventSelector.InputCollections =[] 

from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

## declare tighttrackvertexassotool by do first PV dz recovery(match the association procedure of JVF and JVT, default would be for each PV).
tighttrackvertexassotool=CfgMgr.CP__TightTrackVertexAssociationTool("TightTrackVertexAssociationTool", dzSinTheta_cut=3, doPV=True)

loosetrackvertexassotool=CfgMgr.CP__LooseTrackVertexAssociationTool("LooseTrackVertexAssociationTool", dzSinTheta_cut=3, d0_cut=2)



#tighttrackvertexassotool.OutputLevel=DEBUG

ToolSvc += tighttrackvertexassotool
ToolSvc += loosetrackvertexassotool

testAlg=CfgMgr.TrackVertexAssoTestAlg(
        TrackContainer="InDetTrackParticles",
        VertexContainer="PrimaryVertices",
        TightTrackVertexAssoTool=tighttrackvertexassotool,
        LooseTrackVertexAssoTool=loosetrackvertexassotool
        )

#testAlg.OutputLevel=DEBUG


#theApp.setOutputLevel(DEBUG)

topSequence += testAlg

theApp.EvtMax = 100
ServiceMgr.EventSelector.SkipEvents = 0
ServiceMgr.MessageSvc.defaultLimit = 999
