#==============================================================
# Setup jobOptions ShowerParam.py
# - Turns on GFlash shower parametrization
#==============================================================

from G4AtlasApps.SimFlags import simFlags
simFlags.LArParameterization = 2
simFlags.CalibrationRun.set_Off()
