# switch on fast calorimeter simulation with FastCaloSim; add to existing cells in RDO file
include('FastSimulationJobTransforms/FastCaloSimRecConfig.py')

from FastSimulationConfig.FastSimulationFlags import jobproperties
jobproperties.FastSimulation.doFastCaloSimCaloAddCells=True

from CaloRec.CaloCellFlags import jobproperties
jobproperties.CaloCellFlags.doFastCaloSimAddCells = True
