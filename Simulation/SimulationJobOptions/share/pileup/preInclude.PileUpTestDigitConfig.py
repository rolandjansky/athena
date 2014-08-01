from Digitization.DigitizationFlags import digitizationFlags

digitizationFlags.bunchSpacing = 50 # This now sets the bunch slot length.  
digitizationFlags.numberOfCavern = 1
digitizationFlags.numberOfCollisions = 1.2
digitizationFlags.initialBunchCrossing = -16
digitizationFlags.finalBunchCrossing = 16
digitizationFlags.numberOfBeamHalo = 0.05
digitizationFlags.numberOfBeamGas = 0.0003
if hasattr(digitizationFlags, 'numberOfLowPtMinBias'): digitizationFlags.numberOfLowPtMinBias = 2.2 
if hasattr(digitizationFlags, 'numberOfHighPtMinBias'): digitizationFlags.numberOfHighPtMinBias = 0.0022 
if hasattr(digitizationFlags, 'numberOfNDMinBias'): digitizationFlags.numberOfNDMinBias = 1.1
if hasattr(digitizationFlags, 'numberOfSDMinBias'): digitizationFlags.numberOfSDMinBias = 0.05
if hasattr(digitizationFlags, 'numberOfDDMinBias'): digitizationFlags.numberOfDDMinBias = 0.05
digitizationFlags.BeamIntensityPattern = [1.0,1.0,1.0,0.0,0.0,1.0,0.0,0.0,1.0]
digitizationFlags.cavernIgnoresBeamInt=False
digitizationFlags.FixedT0BunchCrossing=1

from AthenaCommon.BeamFlags import jobproperties
jobproperties.Beam.bunchSpacing = 50 # Set this to the spacing between filled bunch-crossings within the train.
