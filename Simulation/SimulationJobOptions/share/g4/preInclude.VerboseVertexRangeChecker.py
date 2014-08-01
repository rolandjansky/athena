def verbose_vertexrangechecker():
    print "verbose Vertexrangechecker"
    from G4AtlasApps import AtlasG4Eng
    AtlasG4Eng.G4Eng.menu_EventFilter().getFilter('VertexRangeChecker').SetVerboseLevel(99)

from G4AtlasApps.SimFlags import simFlags
simFlags.InitFunctions.add_function("preInitG4", verbose_vertexrangechecker)
