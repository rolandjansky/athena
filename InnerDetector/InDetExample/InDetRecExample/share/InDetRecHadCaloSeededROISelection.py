# ------------------------------------------------------------
#
# ----------- loading the selection tools for Calo Seeded Brem
#
# ------------------------------------------------------------


#
# --- load the isolation tool
#
from egammaCaloTools.egammaCaloToolsConf import egammaIso
InDetHadROIegammaIsoTool = egammaIso(name          = "InDetHadROIegammaIso")

ToolSvc+=InDetHadROIegammaIsoTool
if (InDetFlags.doPrintConfigurables()):
    print InDetHadROIegammaIsoTool

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
ToolSvc+=InDetHadROICheckEnergyDepositTool
if (InDetFlags.doPrintConfigurables()):
    print InDetHadROICheckEnergyDepositTool

#
# --- get the builder tool
#
from InDetCaloClusterROIBuilder.InDetCaloClusterROIBuilderConf import InDet__CaloClusterROI_Builder
InDetCaloClusterROIBuilder = InDet__CaloClusterROI_Builder(name = "InDetCaloClusterROIBuilder")

ToolSvc+=InDetCaloClusterROIBuilder
if (InDetFlags.doPrintConfigurables()):
    print InDetCaloClusterROIBuilder

#
# --- now load the algorithm
#
from InDetCaloClusterROISelector.InDetCaloClusterROISelectorConf import InDet__CaloClusterROI_Selector
InDetHadCaloClusterROISelector = InDet__CaloClusterROI_Selector (name                         = "InDetHadCaloClusterROISelector",
                                                              InputClusterContainerName    = InDetKeys.HadCaloClusterContainer(),    # "LArClusterEM"
                                                              CellsName                    = InDetKeys.HadCaloCellContainer(),       # "AllCalo"
                                                              OutputClusterContainerName   = InDetKeys.HadCaloClusterROIContainer(), # "InDetCaloClusterROIs"
                                                              CheckHadronicEnergy          = False,
                                                              CheckReta                    = False,
                                                              CheckEMSamples               = False, 
                                                              ClusterEtCut                 = 25000,
                                                              CaloClusterROIBuilder        = InDetCaloClusterROIBuilder, 
                                                              egammaCheckEnergyDepositTool = InDetHadROICheckEnergyDepositTool,
                                                              EMCaloIsolationTool          = InDetHadROIegammaIsoTool)
topSequence += InDetHadCaloClusterROISelector
if (InDetFlags.doPrintConfigurables()):
    print InDetHadCaloClusterROISelector

