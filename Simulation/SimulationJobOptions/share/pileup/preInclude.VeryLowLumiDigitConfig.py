from Digitization.DigitizationFlags import digitizationFlags

digitizationFlags.bunchSpacing = 50  
digitizationFlags.numberOfCavern = 0
digitizationFlags.numberOfCollisions = 0.002
if hasattr(digitizationFlags, 'numberOfLowPtMinBias'): digitizationFlags.numberOfLowPtMinBias = 0.002
if hasattr(digitizationFlags, 'numberOfHighPtMinBias'): digitizationFlags.numberOfHighPtMinBias = 0.00002
if hasattr(digitizationFlags, 'numberOfNDMinBias'): digitizationFlags.numberOfNDMinBias = 0.002
if hasattr(digitizationFlags, 'numberOfSDMinBias'): digitizationFlags.numberOfSDMinBias = 0.0005
if hasattr(digitizationFlags, 'numberOfDDMinBias'): digitizationFlags.numberOfDDMinBias = 0.0005
digitizationFlags.initialBunchCrossing = -16
digitizationFlags.finalBunchCrossing = 16
digitizationFlags.numberOfBeamHalo = 0.05
digitizationFlags.numberOfBeamGas = 0.0003
## h2r_postInclude='SimulationJobOptions/postOptions.Truth.pTmin100MeV.py'
