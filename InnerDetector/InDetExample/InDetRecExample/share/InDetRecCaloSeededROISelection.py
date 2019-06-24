# ------------------------------------------------------------
#
# ----------- loading the selection tools for Calo Seeded Brem
#
# ------------------------------------------------------------

#
# --- load the tool to check the energy deposits and select clusters
#
from egammaRec.Factories import ToolFactory
from egammaCaloTools.egammaCaloToolsFactories import egammaCheckEnergyDepositTool
from egammaCaloTools import egammaCaloToolsConf

egammaCaloClusterROISelector = ToolFactory( egammaCaloToolsConf.egammaCaloClusterSelector,
                                            name = 'caloClusterROISelector',
                                            egammaCheckEnergyDepositTool = egammaCheckEnergyDepositTool,
                                            EMEtRanges = [1500.,2500.,3500.],
                                            EMFCuts = [0.9,0.85,0.8],
                                            RetaCut = [0.7,0.65,0.65]
                                            ) 
#
# --- get the builder tool
#
from InDetCaloClusterROIBuilder.InDetCaloClusterROIBuilderConf import InDet__CaloClusterROI_Builder
InDetCaloClusterROIBuilder = InDet__CaloClusterROI_Builder(name = "InDetCaloClusterROIBuilder",
                                                           EMEnergyOnly = True)

if (InDetFlags.doPrintConfigurables()):
    print InDetCaloClusterROIBuilder

#
# --- now load the algorithm
#
from InDetCaloClusterROISelector.InDetCaloClusterROISelectorConf import InDet__CaloClusterROI_Selector
InDetCaloClusterROISelector = InDet__CaloClusterROI_Selector (name                         = "InDetCaloClusterROISelector",
                                                              InputClusterContainerName    = InDetKeys.CaloClusterContainer(),    # "egammaCaloCluster"
                                                              OutputClusterContainerName   = InDetKeys.CaloClusterROIContainer(), # "InDetCaloClusterROIs"
                                                              CaloClusterROIBuilder        = InDetCaloClusterROIBuilder, 
                                                              egammaCaloClusterSelector    = egammaCaloClusterROISelector()
                                                             )

topSequence += InDetCaloClusterROISelector
if (InDetFlags.doPrintConfigurables()):
    print InDetCaloClusterROISelector

