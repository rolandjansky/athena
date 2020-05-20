################################################
##
## Job config to turn off vertex positioners
##
## Author: Andrea di Simone
##
################################################

from G4AtlasApps.SimFlags import simFlags
if simFlags.EventFilter.statusOn:
    simFlags.VertexFromCondDB.set_Off()
    simFlags.EventFilter.switchFilterOff('EtaPhiFilters')
    simFlags.EventFilter.switchFilterOff('VertexPositioner')
    simFlags.EventFilter.switchFilterOff('VertexRangeChecker')

