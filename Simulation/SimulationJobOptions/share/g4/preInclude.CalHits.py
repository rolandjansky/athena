################################################
## Setup jobOptions CalHits.py
##
## Turns on calibration hits for LAr and Tile
################################################

from G4AtlasApps.SimFlags import simFlags
simFlags.CalibrationRun = 'LAr+Tile'

include("CaloG4Sim/CalibrationDefaultProcessing_jobOptions.py") #FIXME temporarily required for release 20.3.5
