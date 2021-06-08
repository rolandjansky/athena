# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

def LArConfigRun2Pileup(flags):
   flags.LAr.ROD.DoOFCPileupOptimization = True # default
   flags.LAr.ROD.useHighestGainAutoCorr = True # default
   flags.LAr.ROD.FirstSample = 0  # default
   flags.LAr.ROD.nSamples = 4     # default
   flags.LAr.ROD.NumberOfCollisions = 20 # Run-2 default
   
   flags.Digitization.HighGainEMECIW = False  # default
   flags.Digitization.HighGainFCal = False    # default

def LArConfigRun2NoPileup(flags):
   flags.LAr.ROD.DoOFCPileupOptimization = False # no pileup optimisation for OFC
   flags.LAr.ROD.useHighestGainAutoCorr = False # default
   flags.LAr.ROD.FirstSample = 0  # default
   flags.LAr.ROD.nSamples = 4     # default
   flags.LAr.ROD.NumberOfCollisions = 0 #  no pileup
   
   flags.Digitization.HighGainEMECIW = True  # use high gain in EMEC IW
   flags.Digitization.HighGainFCal = True    # use high gain in Fcal

def LArConfigRun2(flags):
   if flags.Digitization.PileUp:
     LArConfigRun2Pileup(flags)
   else:
     LArConfigRun2NoPileup(flags)

