# ------------------------------------------------------------
#
# ----------- loading the selection tools for Calo Seeded Brem
#
# ------------------------------------------------------------

#
# --- load the tool to check the energy deposits and select clusters
#
from egammaRec.Factories import ToolFactory
from egammaCaloTools import egammaCaloToolsConf
egammaCaloClusterHadROISelector = ToolFactory( egammaCaloToolsConf.egammaCaloClusterSelector,
                                               name = 'caloClusterHadROISelector',
                                               ClusterEtCut = 25000
                                             ) 

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
                                                              CaloClusterROIBuilder        = InDetCaloClusterROIBuilder
                                                              egammaCaloClusterSelector    = egammaCaloClusterHadROISelector()
                                                             )

topSequence += InDetHadCaloClusterROISelector
if (InDetFlags.doPrintConfigurables()):
    print InDetHadCaloClusterROISelector

