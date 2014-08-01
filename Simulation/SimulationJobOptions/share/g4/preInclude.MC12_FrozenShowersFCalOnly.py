include("SimulationJobOptions/preInclude.FrozenShowersFCalOnly.py")
include("SimulationJobOptions/preInclude.BeamPipeKill.py")
from G4AtlasApps.SimFlags import simFlags
if hasattr(simFlags, 'SimulationFlavour'):
    simFlags.SimulationFlavour = "FullSimulation+FCalFrozenShowers"
