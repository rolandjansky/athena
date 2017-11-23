#########################################################
#
# SimulationJobOptions/Cavern.py
# Andrea Dell'Acqua
#
# Setting special range cuts in G4 for use with cavern
# background production.
#########################################################

from G4AtlasApps.SimFlags import simFlags
simFlags.CalibrationRun.set_Off()
simFlags.CavernCuts = True
simFlags.LArParameterization = 0

