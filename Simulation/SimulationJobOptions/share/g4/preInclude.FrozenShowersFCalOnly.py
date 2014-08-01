#==============================================================
# SimulationJobOptions/preInclude.FrozenShowersFCalOnly.py
# - Turns on GFlash shower parametrization for FCAL
#==============================================================

from G4AtlasApps.SimFlags import simFlags
simFlags.LArParameterization=3
simFlags.CalibrationRun.set_Off()
