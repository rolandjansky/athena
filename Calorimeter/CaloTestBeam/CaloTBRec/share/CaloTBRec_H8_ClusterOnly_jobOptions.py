# Job Option fragemnt to produce CaloClusters out of LArCells.
# Currently only used for Monitoring


#--- topo cluster maker ---------------------------

theApp.TopAlg += ["CaloClusterMaker/CaloTopoClusterEMMaker"]

CaloTopoClusterEMMaker = Algorithm( "CaloTopoClusterEMMaker" )
CaloTopoClusterEMMaker.ClustersOutputName="CaloTopoClusterEM"
CaloTopoClusterEMMaker.ClusterMakerTools=[
"CaloTopoClusterMaker/TopoCluster"
]
CaloTopoClusterEMMaker.TopoCluster.CellsNames=["AllCalo"]
CaloTopoClusterEMMaker.TopoCluster.CalorimeterNames=[
"LAREM"
]
CaloTopoClusterEMMaker.TopoCluster.SeedSamplingNames = [
	  "PreSamplerB", "EMB1", "EMB2", "EMB3",
	  "PreSamplerE", "EME1", "EME2", "EME3"]
CaloTopoClusterEMMaker.TopoCluster.UseCaloNoiseTool=FALSE
CaloTopoClusterEMMaker.TopoCluster.UsePileUpNoise=FALSE
CaloTopoClusterEMMaker.TopoCluster.NeighborOption                 = "all3D"
CaloTopoClusterEMMaker.TopoCluster.NoiseSigma                     = 200.0*MeV
CaloTopoClusterEMMaker.TopoCluster.CellThresholdOnAbsEinSigma     =   0.0
CaloTopoClusterEMMaker.TopoCluster.NeighborThresholdOnAbsEinSigma =   3.0
CaloTopoClusterEMMaker.TopoCluster.SeedThresholdOnEinSigma        =   4.0
CaloTopoClusterEMMaker.TopoCluster.CellThresholdOnAbsEt           =   -1
CaloTopoClusterEMMaker.TopoCluster.NeighborThresholdOnAbsEt       =   -1
CaloTopoClusterEMMaker.TopoCluster.SeedThresholdOnEt              =   -1
CaloTopoClusterEMMaker.TopoCluster.OutputLevel = INFO
