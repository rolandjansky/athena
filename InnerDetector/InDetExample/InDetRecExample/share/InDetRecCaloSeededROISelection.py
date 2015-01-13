# ------------------------------------------------------------
#
# ----------- loading the selection tools for Calo Seeded Brem
#
# ------------------------------------------------------------

#
# --- load the isolation tool
#
from egammaCaloTools.egammaCaloToolsConf import egammaIso
InDetROIegammaIsoTool = egammaIso(name          = "InDetROIegammaIso")

ToolSvc+=InDetROIegammaIsoTool
if (InDetFlags.doPrintConfigurables()):
    print InDetROIegammaIsoTool

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
ToolSvc+=InDetROICheckEnergyDepositTool
if (InDetFlags.doPrintConfigurables()):
    print InDetROICheckEnergyDepositTool

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
InDetCaloClusterROISelector = InDet__CaloClusterROI_Selector (name                         = "InDetCaloClusterROISelector",
                                                              InputClusterContainerName    = InDetKeys.CaloClusterContainer(),    # "LArClusterEM"
                                                              CellsName                    = InDetKeys.CaloCellContainer(),       # "AllCalo"
                                                              OutputClusterContainerName   = InDetKeys.CaloClusterROIContainer(), # "InDetCaloClusterROIs"
                                                              CheckHadronicEnergy          = True,
                                                              CheckReta                    = True,
                                                              HadRatioCut                  = 0.1,
                                                              RetaCut                      = 0.65,
                                                              CaloClusterROIBuilder        = InDetCaloClusterROIBuilder, 
                                                              egammaCheckEnergyDepositTool = InDetROICheckEnergyDepositTool,
                                                              EMCaloIsolationTool          = InDetROIegammaIsoTool)
topSequence += InDetCaloClusterROISelector
if (InDetFlags.doPrintConfigurables()):
    print InDetCaloClusterROISelector

