# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.AthConfigFlags import AthConfigFlags

def createMETConfigFlags():
    metConfigFlags=AthConfigFlags()
    metConfigFlags.addFlag("MET.UseTracks",True)
    metConfigFlags.addFlag("MET.DoPFlow",True)
    metConfigFlags.addFlag("MET.UseFELinks",True)
    metConfigFlags.addFlag("MET.WritetoAOD",False)
    metConfigFlags.addFlag("MET.DecorateSoftConst",False)
    return metConfigFlags