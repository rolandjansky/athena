# switch on fast calorimeter simulation with FastCaloSim
from FastSimulationConfig.FastSimulationFlags import jobproperties
jobproperties.FastSimulation.doFastCaloSim=True

include('FastSimulationJobTransforms/FastCaloSimObjectCalibration.py')
