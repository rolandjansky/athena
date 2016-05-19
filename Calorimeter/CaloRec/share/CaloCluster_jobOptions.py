#
# jobOptions file for Combined Cluster Reconstruction
# (Sliding Window with CaloTowers)
#

#theApp.Dlls += [ 
#"CaloRec", "LArClusterRec","TileRecAlgs"
#]

#
# -- switch on some monitoring
if not 'doCaloTowerMonitoring' in dir():
    doCaloTowerMonitoring = False

from CaloRec.CaloRecConf import CaloClusterMaker
from CaloRec.CaloRecConf import CaloTowerAlgorithm

CmbTowerBldr = CaloTowerAlgorithm("CmbTowerBldr")
CaloSWClusterMaker = CaloClusterMaker("CaloSWClusterMaker")
# -- do monitoring
if doCaloTowerMonitoring:
#    theApp.TopAlg += [
#        "CaloTowerAlgorithm/CmbTowerBldr",
#        "CaloTowerMonitor/TowerSpy",
#        "CaloClusterMaker/CaloSWClusterMaker" 
#        ]
     from CaloRec.CaloRecConf import CaloTowerMonitor
     TowerSpy = CaloTowerMonitor("TowerSpy")
#else:
#    theApp.TopAlg += [
#        "CaloTowerAlgorithm/CmbTowerBldr",
#        "CaloClusterMaker/CaloSWClusterMaker" 
#        ]
     
    
#--------------------------------------------------------------
# Algorithms Private Options
#--------------------------------------------------------------
#tower Maker:
#CmbTowerBldr.OutputLevel = 2;
#CmbTowerBldr = Algorithm( "CmbTowerBldr" )
CmbTowerBldr.TowerContainerName="CombinedTower"
CmbTowerBldr.NumberOfPhiTowers=64
CmbTowerBldr.NumberOfEtaTowers=100
CmbTowerBldr.EtaMin=-5.0
CmbTowerBldr.EtaMax=5.0
# input to LArTowerBuilder:  cells in LArEM and LARHEC 
from LArRecUtils.LArRecUtilsConf import LArTowerBuilderTool
LArCmbTwrBldr = LArTowerBuilderTool("LArCmbTwrBldr")
LArCmbTwrBldr.CellContainerName = "AllCalo"
LArCmbTwrBldr.IncludedCalos     = [ "LAREM", "LARHEC" ]
ToolSvc += LArCmbTwrBldr
# input to FCALTowerBuilder : cells in FCAL 
from LArRecUtils.LArRecUtilsConf import LArFCalTowerBuilderTool
FCalCmbTwrBldr = LArFCalTowerBuilderTool("FCalCmbTwrBldr")
FCalCmbTwrBldr.CellContainerName = "AllCalo"
FCalCmbTwrBldr.MinimumEt         = 0.*MeV
ToolSvc += FCalCmbTwrBldr
#input to  TileTowerBuilder:  cells in TILE
from TileRecUtils.TileRecUtilsConf import TileTowerBuilderTool
TileCmbTwrBldr = TileTowerBuilderTool("TileCmbTwrBldr")
TileCmbTwrBldr.CellContainerName = "AllCalo"
TileCmbTwrBldr.DumpTowers        = FALSE
TileCmbTwrBldr.DumpWeightMap     = FALSE
ToolSvc += TileCmbTwrBldr

CmbTowerBldr.TowerBuilderTools=[ TileCmbTwrBldr, LArCmbTwrBldr, FCalCmbTwrBldr ]

# monitoring optional
if doCaloTowerMonitoring:
#    TowerSpy = Algorithm( "TowerSpy" )
    TowerSpy.OutputLevel = INFO
    TowerSpy.InputTowerCollections = [ "CombinedTower" ]
# CaloSlidingWindow Properties
#CaloSWClusterMaker = Algorithm( "CaloSWClusterMaker" )
CaloSWClusterMaker.ClustersOutputName="CombinedCluster"
from CaloRec.CaloRecConf import CaloClusterBuilderSW
SWCluster = CaloClusterBuilderSW("SWCluster")
SWCluster.TowerContainer="CombinedTower"
SWCluster.eta_size=5
SWCluster.phi_size=5
SWCluster.eta_sizep=3
SWCluster.phi_sizep=3
SWCluster.e_threshold=15.*GeV
SWCluster.FillClusterCells=TRUE #fill cells in ClusterBuilderSW
SWCluster.nextra=0
SWCluster.eta_SeedGrid=5
SWCluster.phi_SeedGrid=5
SWCluster.eta_Duplicate=5
SWCluster.phi_Duplicate=5
ToolSvc += SWCluster
CaloSWClusterMaker.ClusterMakerTools=[ ToolSvc.SWCluster.getFullName() ]

topSequence += CmbTowerBldr
topSequence += CaloSWClusterMaker
if doCaloTowerMonitoring:
   topSequence += TowerSpy
