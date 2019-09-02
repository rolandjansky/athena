# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.AthConfigFlags import AthConfigFlags

#todo? add in the explanatory text from previous implementation

def createSimConfigFlags():
    scf=AthConfigFlags()

    scf.addFlag("Sim.ISF.Run",False)
    scf.addFlag("Sim.ISF.HITSMergingRequired", True)
    scf.addFlag("Sim.ParticleID",False)
    scf.addFlag("Sim.CalibrationRun", "DeadLAr") # "LAr", "Tile", "LAr+Tile", "DeadLAr"

    scf.addFlag("Sim.CavernBG",False) #"Write" , "Read" , "Signal" , "WriteWorld" , "SignalWorld"
    scf.addFlag("Sim.ReadTR",False)
    scf.addFlag("Sim.WorldRRange", 12500) #int or float
    scf.addFlag("Sim.WorldZRange", 22031) #int or float

    # the G4 offset. It was never changed, so no need to peek in file
    scf.addFlag("Sim.SimBarcodeOffset", 200000)

    #for forward region
    scf.addFlag('Sim.TwissFileBeam1',False)
    scf.addFlag('Sim.TwissFileBeam2',False)
    scf.addFlag('Sim.TwissEnergy',8000000.)
    scf.addFlag('Sim.TwissFileBeta',550.)
    scf.addFlag('Sim.TwissFileNomReal',False) #'nominal','real' #default to one of these?!
    scf.addFlag('Sim.TwissFileVersion','v01')

    #for G4AtlasAlg
    #in simflags
    scf.addFlag('Sim.ReleaseGeoModel',True)
    scf.addFlag('Sim.RecordFlux',False)
    scf.addFlag('Sim.TruthStrategy','MC12') #todo - needs to have some extra functionality!
    scf.addFlag('Sim.G4Commands',['/run/verbose 2'])
    #in atlasflags
    scf.addFlag('Sim.FlagAbortedEvents',False)
    scf.addFlag('Sim.KillAbortedEvents',True)

    # Do full simulation + digitisation + reconstruction chain
    scf.addFlag("Sim.DoFullChain", False)

    scf.addFlag("Sim.G4Version", "geant4.10.1.patch03.atlas02")
    scf.addFlag("Sim.PhysicsList", "FTFP_BERT_ATL")

    return scf

