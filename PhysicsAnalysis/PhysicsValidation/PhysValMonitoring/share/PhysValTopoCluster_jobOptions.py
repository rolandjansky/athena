from PFODQA.PFODQAConf import PhysValCluster

PhysValCluster_LC = PhysValCluster("PhysValCluster_LC")
PhysValCluster_LC.OutputLevel = VERBOSE
PhysValCluster_LC.DetailLevel = 10
PhysValCluster_LC.EnableLumi = False
PhysValCluster_LC.ClusterContainerName="CaloCalTopoCluster"

from AthenaCommon.AppMgr import ToolSvc
ToolSvc += PhysValCluster_LC
monMan.AthenaMonTools += [ PhysValCluster_LC ]
