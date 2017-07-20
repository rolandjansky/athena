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

from AthenaCommon.CfgGetter import getPrivateTool,getService
getService('DetectorGeometrySvc').RegionCreators += [getPrivateTool('MuonPhysicsRegionTool')]
#FIXME need to add this to the list of RegionCreators, but
#currently initializing the DetectorGeometrySvc too early causes
#problems with the GeoModelSvc configuration.
