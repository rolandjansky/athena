# SUBCALO enum LAREM LARHEC LARFCAL TILE NSUBCALO NOT_VALID
from CaloIdentifier import SUBCALO

#include("CaloRec/CaloCellMaker_config.py")

theApp.Dlls += ["CaloRec"]
theApp.Dlls += ["AtlfastAlgs"]
theApp.Dlls += ["AtlfastConversionAlgs"]
theApp.Dlls += ["AtlfastConversionTools"]
theApp.Dlls += ["FastCaloSim"]

FSAllCalo="AllCalo"

theApp.TopAlg  += ["FastCaloSim::FSStoregateClean/FSStoregateClean"]
FSStoregateClean = Algorithm("FSStoregateClean")
FSStoregateClean.StoreGateKeys=[FSAllCalo]

theApp.TopAlg += [
"CaloCellMaker/CaloCellMakerFS" 
]

ToolSvc = Service( "ToolSvc" )

#--------------------------------------------------------------
# Algorithms Private Options
#--------------------------------------------------------------
CaloCellMakerFS = Algorithm( "CaloCellMakerFS" )
CaloCellMakerFS.CaloCellsOutputName = FSAllCalo
if CaloCellMakerFlags.doCompactCellInAddition:
    CaloCellMakerFS.CaloCompactCellsOutputName = "FSCompactAllCalo"

# compactified/decompactified calocellcontianer replace the normal one
if CaloCellMakerFlags.doCompactCellInstead:
    CaloCellMakerFS.CaloCellsOutputName = "FSOriginalAllCalo"
    CaloCellMakerFS.CaloCompactCellsOutputName = FSAllCalo

# make the calo cells
include ("FastCaloSim/FastShower_CellMakerTool_jobOptions.py")

# now the CaloCellContainer is completely filled with cells

#finalize container
CaloCellMakerFS.CaloCellMakerToolNames += ["CaloCellContainerFinalizerTool"]

#make lots of checks (should not be necessary eventually)
#to print the check add:
CaloCellMakerFS.CaloCellMakerToolNames += ["CaloCellContainerCheckerTool"] 
#ToolSvc.CaloCellContainerCheckerTool.OutputLevel=VERBOSE
#ToolSvc.CaloCellContainerCheckerTool.OutputLevel=DEBUG

# if necessary finalize and check the compactified/decompactified tool
if CaloCellMakerFlags.doCompactCellInAddition or CaloCellMakerFlags.doCompactCellInstead:
    theApp.Dlls+= ["CaloTools"]
    CaloCellMakerFS.CaloCompactCellMakerToolNames += ["CaloCellContainerFinalizerTool"]
    CaloCellMakerFS.CaloCompactCellMakerToolNames += ["CaloCellContainerCheckerTool"] 
