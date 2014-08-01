from Digitization.DigitizationFlags import digitizationFlags

digitizationFlags.bunchSpacing = 900  
digitizationFlags.numberOfCavern = 0
digitizationFlags.numberOfCollisions = 0.002 #altered to give a ratio of N(2MB vtxs)/N(1MB vtx)=0.001
if hasattr(digitizationFlags, 'numberOfLowPtMinBias'): digitizationFlags.numberOfLowPtMinBias = 0.002
if hasattr(digitizationFlags, 'numberOfHighPtMinBias'): digitizationFlags.numberOfHighPtMinBias = 0.00002
if hasattr(digitizationFlags, 'numberOfNDMinBias'): digitizationFlags.numberOfNDMinBias = 0.002
if hasattr(digitizationFlags, 'numberOfSDMinBias'): digitizationFlags.numberOfSDMinBias = 0.0005
if hasattr(digitizationFlags, 'numberOfDDMinBias'): digitizationFlags.numberOfDDMinBias = 0.0005
digitizationFlags.initialBunchCrossing = 0
digitizationFlags.finalBunchCrossing = 0
digitizationFlags.numberOfBeamHalo = 0.05
digitizationFlags.numberOfBeamGas = 0.0003
## h2r_postInclude='SimulationJobOptions/postOptions.VeryLowLumiMinbiasOnMinbias_900ns_DigitConfig.py'
