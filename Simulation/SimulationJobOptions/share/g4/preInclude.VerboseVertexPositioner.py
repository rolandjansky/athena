######################################################################
## SimulationJobOptions/preInclude.VerboseVertexPositioner.py
##
## Use verbose output for the VertexPositioner. (Can be easily adapted
## for use with the other EventFilters if required.)
######################################################################

def verbose_vertexpositioner():
    print "verbose VertexPositioner"
    from G4AtlasApps import AtlasG4Eng
    AtlasG4Eng.G4Eng.menu_EventFilter().getFilter('VertexPositioner').SetVerboseLevel(99)

from G4AtlasApps.SimFlags import simFlags
simFlags.InitFunctions.add_function("preInitG4", verbose_vertexpositioner)
