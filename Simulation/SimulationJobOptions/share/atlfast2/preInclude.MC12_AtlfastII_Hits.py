include("SimulationJobOptions/preInclude.FastIDKiller.py")
from G4AtlasApps.SimFlags import simFlags
if hasattr(simFlags, 'SimulationFlavour'):
    simFlags.SimulationFlavour = "AtlfastII"
