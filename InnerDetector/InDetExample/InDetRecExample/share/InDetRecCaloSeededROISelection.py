# ------------------------------------------------------------
#
# ----------- loading the selection tools for Calo Seeded Brem
#
# ------------------------------------------------------------

#
# --- load the tool to check the energy deposits
#
from egammaTools.egammaToolsFactories import egammaCheckEnergyDepositTool
#
# --- get the builder tool
#
from InDetCaloClusterROIBuilder.InDetCaloClusterROIBuilderConf import InDet__CaloClusterROI_Builder
InDetCaloClusterROIBuilder = InDet__CaloClusterROI_Builder(name = "InDetCaloClusterROIBuilder")

if (InDetFlags.doPrintConfigurables()):
    print InDetCaloClusterROIBuilder

#
# --- now load the algorithm
#
from InDetCaloClusterROISelector.InDetCaloClusterROISelectorConf import InDet__CaloClusterROI_Selector
InDetCaloClusterROISelector = InDet__CaloClusterROI_Selector (name                         = "InDetCaloClusterROISelector",
                                                              InputClusterContainerName    = InDetKeys.CaloClusterContainer(),    # "egammaCaloCluster"
                                                              OutputClusterContainerName   = InDetKeys.CaloClusterROIContainer(), # "InDetCaloClusterROIs"
                                                              ClusterLateralCut            = 0.8,
                                                              ClusterEMEtCut               = 1500,
                                                              ClusterEMFCut                = 0.6,
                                                              CaloClusterROIBuilder        = InDetCaloClusterROIBuilder, 
                                                              egammaCheckEnergyDepositTool = egammaCheckEnergyDepositTool())
topSequence += InDetCaloClusterROISelector
if (InDetFlags.doPrintConfigurables()):
    print InDetCaloClusterROISelector

