# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.AthConfigFlags import AthConfigFlags

def createSimConfigFlags():
    scf=AthConfigFlags()


    scf.addFlag("Sim.ISF.Run",False)
    scf.addFlag("Sim.ISF.HITSMergingRequired", True)
    scf.addFlag('Sim.ParticleID',False)
    scf.addFlag("Sim.CalibrationRun", "DeadLAr") # 'LAr', 'Tile', 'LAr+Tile', 'DeadLAr'


    return scf
