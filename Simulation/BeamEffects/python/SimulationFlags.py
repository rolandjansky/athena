#Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.AthConfigFlags import AthConfigFlags


def _createCfgFlags():

    acf=AthConfigFlags()

    #combine the 3 previous flags into one
    # e.g. 'CondDB', 'VertexOverrideEventFile.txt', 'VertexOverride.txt',"LongBeamspot" 
    acf.addFlag('Vertex.Source', 'CondDB' )

    # Name of the random number service to use
    # of ("dSFMT", "Ranlux64", "Ranecu",)
    acf.addFlag("Random.Engine", "dSFMT")

    return acf


ConfigFlagsSimulation=_createCfgFlags()

del _createCfgFlags
