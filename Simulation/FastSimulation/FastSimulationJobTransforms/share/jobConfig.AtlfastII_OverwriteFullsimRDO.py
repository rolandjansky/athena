include("FastSimulationJobTransforms/jobConfig.AtlfastII.py")

from FastSimulationConfig.FastSimulationFlags import jobproperties
jobproperties.FastSimulation.doFastCaloSimCaloAddCells = False

from CaloRec.CaloCellFlags import jobproperties
jobproperties.CaloCellFlags.doFastCaloSimAddCells = False
