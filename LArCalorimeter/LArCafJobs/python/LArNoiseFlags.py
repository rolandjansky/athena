# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

def addNoiseFlags(flags):
    
    flags.Input.isMC=False

    flags.addFlag("LArNoise.outNtupLAr","")
    flags.addFlag("LArNoise.outHistLAr","")
    flags.addFlag("LArNoise.HECNoiseNtup","")
    flags.addFlag("LArNoise.SigmaCut",3.0)
    flags.addFlag("LArNoise.NumberOfBunchesInFront",30)
    flags.addFlag("LArNoise.KeepOnlyCellID",False)
    #these are for LArHECNoise alg:
    flags.addFlag("LArNoise.MinDigitADC",20)
    flags.addFlag("LArNoise.MaxDeltaT",5)
    



