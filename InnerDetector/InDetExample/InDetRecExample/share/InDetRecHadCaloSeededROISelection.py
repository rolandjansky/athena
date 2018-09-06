# ------------------------------------------------------------
#
# ----------- loading the selection tools for Calo Seeded Brem
#
# ------------------------------------------------------------


#
# --- load the tool to check the energy deposits
#
from egammaTools.egammaToolsConf import egammaCheckEnergyDepositTool
InDetHadROICheckEnergyDepositTool = egammaCheckEnergyDepositTool(name       = "InDetHadROICheckEnergyDepositTool",
                                                              UseThrFmax = True,
                                                              ThrF0max   = 0.9,
                                                              ThrF1max   = 0.8,
                                                              ThrF2max   = 0.98,
                                                              ThrF3max   = 0.8)
if (InDetFlags.doPrintConfigurables()):
    print InDetHadROICheckEnergyDepositTool

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
InDetHadCaloClusterROISelector = InDet__CaloClusterROI_Selector (name                         = "InDetHadCaloClusterROISelector",
                                                              InputClusterContainerName    = InDetKeys.HadCaloClusterContainer(),    # "LArClusterEM"
                                                              OutputClusterContainerName   = InDetKeys.HadCaloClusterROIContainer(), # "InDetCaloClusterROIs"
                                                              ClusterEtCut                 = 25000,
                                                              CaloClusterROIBuilder        = InDetCaloClusterROIBuilder, 
                                                              egammaCheckEnergyDepositTool = InDetHadROICheckEnergyDepositTool)

topSequence += InDetHadCaloClusterROISelector
if (InDetFlags.doPrintConfigurables()):
    print InDetHadCaloClusterROISelector

