# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

def LArConfigRun2(flags):
   flags.LAr.ROD.DoOFCPileupOptimization = flags.Digitization.PileUp
   flags.LAr.ROD.UseHighestGainAutoCorr = flags.Digitization.PileUp
   flags.LAr.ROD.FirstSample = 0  # default
   flags.LAr.ROD.nSamples = 4     # default
   flags.LAr.ROD.NumberOfCollisions = 20 if flags.Digitization.PileUp else 0  # Run-2 default
   
   flags.Digitization.HighGainEMECIW = not flags.Digitization.PileUp
   flags.Digitization.HighGainFCal = not flags.Digitization.PileUp
