################################################
## Setup jobOptions CalHits.py
##
## Turns on calibration hits for LAr and Tile
################################################

from G4AtlasApps.SimFlags import simFlags
simFlags.CalibrationRun = 'LAr+Tile'
