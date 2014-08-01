################################################
##
## Job config to turn off vertex positioners
##
## Author: Andrea di Simone
##
################################################

from G4AtlasApps.SimFlags import simFlags
if simFlags.EventFilter.statusOn:
    simFlags.EventFilter.switchFilterOff('VertexPositioner')
simFlags.VertexFromCondDB = False # required for consistency
