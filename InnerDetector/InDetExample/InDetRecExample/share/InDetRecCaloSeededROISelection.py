# ------------------------------------------------------------
#
# ----------- loading the selection tools for Calo Seeded Brem
#
# ------------------------------------------------------------

#
# --- load the tool to check the energy deposits
#
from egammaTools.egammaToolsConf import egammaCheckEnergyDepositTool
InDetROICheckEnergyDepositTool = egammaCheckEnergyDepositTool(name       = "InDetROICheckEnergyDepositTool",
                                                              UseThrFmax = True,
                                                              ThrF0max   = 0.9,
                                                              ThrF1max   = 0.8,
                                                              ThrF2max   = 0.98,
                                                              ThrF3max   = 0.8)
if (InDetFlags.doPrintConfigurables()):
    print InDetROICheckEnergyDepositTool

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
                                                              ClusterR2Cut                 = 8000,
                                                              ClusterEMEtCut               = 1000,
                                                              CaloClusterROIBuilder        = InDetCaloClusterROIBuilder, 
                                                              egammaCheckEnergyDepositTool = InDetROICheckEnergyDepositTool)
topSequence += InDetCaloClusterROISelector
if (InDetFlags.doPrintConfigurables()):
    print InDetCaloClusterROISelector

