##########################################
# Comments
# Requested as a result of the plans outlined here:
# http://indico.cern.ch/getFile.py/access?contribId=1&resId=0&materialId=slides&confId=72810
# It is intended that these job options be used with Cavern Background samples with SF=1
# It describes the alternative scenario to PileUp_Requested20100427_DigitConfig.py
##########################################

from Digitization.DigitizationFlags import digitizationFlags

digitizationFlags.bunchSpacing = 900  
digitizationFlags.numberOfCavern = 1
digitizationFlags.numberOfCollisions = 5.0
if hasattr(digitizationFlags, 'numberOfLowPtMinBias'): digitizationFlags.numberOfLowPtMinBias = 5.0
if hasattr(digitizationFlags, 'numberOfHighPtMinBias'): digitizationFlags.numberOfHighPtMinBias = 0.005
if hasattr(digitizationFlags, 'numberOfNDMinBias'): digitizationFlags.numberOfNDMinBias = 5.0
if hasattr(digitizationFlags, 'numberOfSDMinBias'): digitizationFlags.numberOfSDMinBias = 0.05
if hasattr(digitizationFlags, 'numberOfDDMinBias'): digitizationFlags.numberOfDDMinBias = 0.05
digitizationFlags.initialBunchCrossing = 0
digitizationFlags.finalBunchCrossing = 0
digitizationFlags.numberOfBeamHalo = 0.05
digitizationFlags.numberOfBeamGas = 0.0003
## h2r_postInclude="SimulationJobOptions/postOptions.AlwaysReplaceUsedMinBiasAndCavernDigitConfig.py"
