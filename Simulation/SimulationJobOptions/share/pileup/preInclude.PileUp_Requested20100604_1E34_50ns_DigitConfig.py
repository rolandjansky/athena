##########################################
# Comments
# Requested for IBL Digitization studies - to be used with InDetIBL_Example/IBL_Digitization.py
# Lumi: 1E34cm^-2s^-1 Bunch Spacing: 50ns
# Pile-Up Simulation Window: [-50ns,+50ns] (includes bunch crossing at +50ns)
##########################################

from Digitization.DigitizationFlags import digitizationFlags
## Pile-up configuration
digitizationFlags.bunchSpacing = 50
digitizationFlags.numberOfCavern = 46 
digitizationFlags.numberOfCollisions = 46.0
if hasattr(digitizationFlags, 'numberOfLowPtMinBias'): digitizationFlags.numberOfLowPtMinBias = 46.0
if hasattr(digitizationFlags, 'numberOfHighPtMinBias'): digitizationFlags.numberOfHighPtMinBias = 0.046
if hasattr(digitizationFlags, 'numberOfNDMinBias'): digitizationFlags.numberOfNDMinBias = 46.0
if hasattr(digitizationFlags, 'numberOfSDMinBias'): digitizationFlags.numberOfSDMinBias = 0.05
if hasattr(digitizationFlags, 'numberOfDDMinBias'): digitizationFlags.numberOfDDMinBias = 0.05
digitizationFlags.initialBunchCrossing = -1
digitizationFlags.finalBunchCrossing = 1
digitizationFlags.numberOfBeamHalo = 0.05
digitizationFlags.numberOfBeamGas = 0.0003
