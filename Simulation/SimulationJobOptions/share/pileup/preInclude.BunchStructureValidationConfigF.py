####################################################################################
## This configuration is one of those used for the validation of the bunch structure
## code. It uses a bunch pattern with a single filled bunch crossing and a single 
## empty bunch crossing and should produce a flat 50ns spacing between filled
## bunch crossings. Cavern Background is independent the bunch pattern.
####################################################################################

from Digitization.DigitizationFlags import digitizationFlags

digitizationFlags.bunchSpacing = 25 # This now sets the bunch slot length.
digitizationFlags.numberOfCavern = 5
digitizationFlags.numberOfCollisions = 5.0
digitizationFlags.initialBunchCrossing = -32
digitizationFlags.finalBunchCrossing = 32
digitizationFlags.numberOfBeamHalo = 0.05
digitizationFlags.numberOfBeamGas = 0.0003
if hasattr(digitizationFlags, 'numberOfLowPtMinBias'): digitizationFlags.numberOfLowPtMinBias = 2.2 
if hasattr(digitizationFlags, 'numberOfHighPtMinBias'): digitizationFlags.numberOfHighPtMinBias = 0.0022 
if hasattr(digitizationFlags, 'numberOfNDMinBias'): digitizationFlags.numberOfNDMinBias = 5.0
if hasattr(digitizationFlags, 'numberOfSDMinBias'): digitizationFlags.numberOfSDMinBias = 0.05
if hasattr(digitizationFlags, 'numberOfDDMinBias'): digitizationFlags.numberOfDDMinBias = 0.05
digitizationFlags.BeamIntensityPattern = [1.0,0.0]

from AthenaCommon.BeamFlags import jobproperties
jobproperties.Beam.bunchSpacing = 50 # Set this to the spacing between filled bunch-crossings within the train.
