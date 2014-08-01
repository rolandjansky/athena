##########################################
# Comments
# Requested 6th May 2010
# Intended to simulate beam conditions in early 2010.
# For use only with minbias signal samples.
# It is intended that these job options be used with Cavern Background samples with SF=1
##########################################

from Digitization.DigitizationFlags import digitizationFlags

digitizationFlags.bunchSpacing = 900  
digitizationFlags.numberOfCavern = 1
digitizationFlags.numberOfCollisions = 0.3
if hasattr(digitizationFlags, 'numberOfLowPtMinBias'): digitizationFlags.numberOfLowPtMinBias = 0.3
if hasattr(digitizationFlags, 'numberOfHighPtMinBias'): digitizationFlags.numberOfHighPtMinBias = 0.0003
if hasattr(digitizationFlags, 'numberOfNDMinBias'): digitizationFlags.numberOfNDMinBias = 0.3
if hasattr(digitizationFlags, 'numberOfSDMinBias'): digitizationFlags.numberOfSDMinBias = 0.05
if hasattr(digitizationFlags, 'numberOfDDMinBias'): digitizationFlags.numberOfDDMinBias = 0.05
digitizationFlags.initialBunchCrossing = 0
digitizationFlags.finalBunchCrossing = 0
digitizationFlags.numberOfBeamHalo = 0.05
digitizationFlags.numberOfBeamGas = 0.0003
## h2r_postInclude=SimulationJobOptions/postOptions.AlwaysReplaceUsedMinBiasAndCavernDigitConfig.py,SimulationJobOptions/postOptions.MinbiasPileUpOnMinbiasSignal.py

