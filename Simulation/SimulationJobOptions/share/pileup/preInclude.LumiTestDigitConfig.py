from Digitization.DigitizationFlags import digitizationFlags

digitizationFlags.bunchSpacing = 25
digitizationFlags.numberOfCavern = 5
digitizationFlags.numberOfCollisions = 4.6
if hasattr(digitizationFlags, 'numberOfLowPtMinBias'): digitizationFlags.numberOfLowPtMinBias = 4.6
if hasattr(digitizationFlags, 'numberOfHighPtMinBias'): digitizationFlags.numberOfHighPtMinBias = 0.0046
if hasattr(digitizationFlags, 'numberOfNDMinBias'): digitizationFlags.numberOfNDMinBias = 4.6
if hasattr(digitizationFlags, 'numberOfSDMinBias'): digitizationFlags.numberOfSDMinBias = 0.023
if hasattr(digitizationFlags, 'numberOfDDMinBias'): digitizationFlags.numberOfDDMinBias = 0.023
digitizationFlags.initialBunchCrossing = -10
digitizationFlags.finalBunchCrossing = 10
digitizationFlags.numberOfBeamHalo = 0.05
digitizationFlags.numberOfBeamGas = 0.0003

