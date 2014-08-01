####################################################################################
## Base for comparison with PileUpBunchTrains2010Config3_DigitConfig.py
## Constant 150ns spacing between filled bunch crossings.
## Cavern Background is *dependent* on the bunch pattern. (Compatible with 150ns cavern background.)
####################################################################################

from Digitization.DigitizationFlags import digitizationFlags

digitizationFlags.bunchSpacing = 25  # This now sets the bunch slot length.
digitizationFlags.numberOfCavern = 1 #FIXME need to check on appropriate values
digitizationFlags.numberOfCollisions = 1.5
digitizationFlags.initialBunchCrossing = -32
digitizationFlags.finalBunchCrossing = 32
digitizationFlags.numberOfBeamHalo = 0.05 #FIXME need to check on appropriate values
digitizationFlags.numberOfBeamGas = 0.0003 #FIXME need to check on appropriate values
if hasattr(digitizationFlags, 'numberOfLowPtMinBias'): digitizationFlags.numberOfLowPtMinBias = 2.2 
if hasattr(digitizationFlags, 'numberOfHighPtMinBias'): digitizationFlags.numberOfHighPtMinBias = 0.0022 
if hasattr(digitizationFlags, 'numberOfNDMinBias'): digitizationFlags.numberOfNDMinBias = 1.5
if hasattr(digitizationFlags, 'numberOfSDMinBias'): digitizationFlags.numberOfSDMinBias = 0.05
if hasattr(digitizationFlags, 'numberOfDDMinBias'): digitizationFlags.numberOfDDMinBias = 0.05
digitizationFlags.BeamIntensityPattern = [0.0,1.0,0.0,0.0,0.0,0.0]# filled bunch crossing  1 (150ns gap)
digitizationFlags.cavernIgnoresBeamInt=False

from AthenaCommon.BeamFlags import jobproperties
jobproperties.Beam.bunchSpacing = 150 # Set this to the spacing between filled bunch-crossings within the train.
