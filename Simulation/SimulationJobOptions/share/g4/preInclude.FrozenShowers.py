######################################################
##
## Turn on GFlash shower parametrization in the LAr
##
######################################################

from G4AtlasApps.SimFlags import simFlags
simFlags.LArParameterization = 1
simFlags.CalibrationRun.set_Off()
