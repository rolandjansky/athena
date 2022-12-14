from PFODQA.PFODQAConf import PhysValCluster

PhysValCluster_LC = PhysValCluster("PhysValCluster_LC")
PhysValCluster_LC.OutputLevel = INFO
PhysValCluster_LC.DetailLevel = 10
PhysValCluster_LC.EnableLumi = False
PhysValCluster_LC.ClusterContainerName="CaloCalTopoClusters"

monMan.AthenaMonTools += [ PhysValCluster_LC ]

# new origin corrected clusters

PhysValCluster_origin = PhysValCluster("PhysValCluster_origin")
PhysValCluster_origin.OutputLevel = INFO
PhysValCluster_origin.DetailLevel = 10
PhysValCluster_origin.EnableLumi = False
PhysValCluster_origin.ClusterContainerName="LCOriginTopoClusters"

monMan.AthenaMonTools += [ PhysValCluster_origin ]
