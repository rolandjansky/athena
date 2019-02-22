# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.AthConfigFlags import AthConfigFlags

def createSimConfigFlags():
    scf=AthConfigFlags()


    scf.addFlag("Sim.ISF.Run",False)
    scf.addFlag("Sim.ISF.HITSMergingRequired", True)

    return scf
