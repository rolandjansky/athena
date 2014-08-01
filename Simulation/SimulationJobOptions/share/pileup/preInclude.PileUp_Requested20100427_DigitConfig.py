##########################################
# Comments
# Requested as a result of the plans outlined here:
# http://indico.cern.ch/getFile.py/access?contribId=1&resId=0&materialId=slides&confId=72810
# It is intended that these job options be used with Cavern Background samples with SF=1
##########################################

from Digitization.DigitizationFlags import digitizationFlags

digitizationFlags.bunchSpacing = 50  
digitizationFlags.numberOfCavern = 1
digitizationFlags.numberOfCollisions = 5.0
if hasattr(digitizationFlags, 'numberOfLowPtMinBias'): digitizationFlags.numberOfLowPtMinBias = 5.0
if hasattr(digitizationFlags, 'numberOfHighPtMinBias'): digitizationFlags.numberOfHighPtMinBias = 0.005
if hasattr(digitizationFlags, 'numberOfNDMinBias'): digitizationFlags.numberOfNDMinBias = 5.0
if hasattr(digitizationFlags, 'numberOfSDMinBias'): digitizationFlags.numberOfSDMinBias = 0.05
if hasattr(digitizationFlags, 'numberOfDDMinBias'): digitizationFlags.numberOfDDMinBias = 0.05
digitizationFlags.initialBunchCrossing = -16
digitizationFlags.finalBunchCrossing = 16
digitizationFlags.numberOfBeamHalo = 0.05
digitizationFlags.numberOfBeamGas = 0.0003
