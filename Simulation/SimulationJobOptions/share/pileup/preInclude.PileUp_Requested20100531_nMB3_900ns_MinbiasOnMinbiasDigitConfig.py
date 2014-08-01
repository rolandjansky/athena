##########################################
# Comments
# It is intended that these job options be used with Cavern Background samples with SF=1
# They describe a similar scenario to PileUp_Requested20100428_MinBias_DigitConfig.py
##########################################
from Digitization.DigitizationFlags import digitizationFlags

digitizationFlags.bunchSpacing = 900
digitizationFlags.numberOfCavern = 1
digitizationFlags.numberOfCollisions = 3.0
if hasattr(digitizationFlags, 'numberOfLowPtMinBias'): digitizationFlags.numberOfLowPtMinBias = 3.0
if hasattr(digitizationFlags, 'numberOfHighPtMinBias'): digitizationFlags.numberOfHighPtMinBias = 0.003
if hasattr(digitizationFlags, 'numberOfNDMinBias'): digitizationFlags.numberOfNDMinBias = 3.0
if hasattr(digitizationFlags, 'numberOfSDMinBias'): digitizationFlags.numberOfSDMinBias = 0.05
if hasattr(digitizationFlags, 'numberOfDDMinBias'): digitizationFlags.numberOfDDMinBias = 0.05
digitizationFlags.initialBunchCrossing = 0
digitizationFlags.finalBunchCrossing = 0
digitizationFlags.numberOfBeamHalo = 0.05
digitizationFlags.numberOfBeamGas = 0.0003
## h2r_postInclude='SimulationJobOptions/postOptions.VeryLowLumiMinbiasOnMinbias_900ns_DigitConfig.py'
