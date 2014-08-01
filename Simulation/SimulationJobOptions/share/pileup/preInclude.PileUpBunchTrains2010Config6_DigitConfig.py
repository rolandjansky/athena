####################################################################################
## For comparison with PileUpBunchTrains2010Config5_DigitConfig.py
## Trains of 8 filled bunch crossings with intra train spacings of 150ns.
## Any of the filled bunch crossings can be chosen as the
## central bunch crossing, so the timing of the out-of-time will vary significantly
## from event to event.
## Cavern Background is *dependent* on the bunch pattern. (Compatible with 150ns Cavern background.)
####################################################################################

from Digitization.DigitizationFlags import digitizationFlags

digitizationFlags.bunchSpacing = 25 # This now sets the bunch slot length.
digitizationFlags.numberOfCavern = 1 #FIXME need to check on appropriate values
digitizationFlags.numberOfCollisions = 2.0
digitizationFlags.initialBunchCrossing = -32
digitizationFlags.finalBunchCrossing = 32
digitizationFlags.numberOfBeamHalo = 0.05 #FIXME need to check on appropriate values
digitizationFlags.numberOfBeamGas = 0.0003 #FIXME need to check on appropriate values
if hasattr(digitizationFlags, 'numberOfLowPtMinBias'): digitizationFlags.numberOfLowPtMinBias = 2.2 
if hasattr(digitizationFlags, 'numberOfHighPtMinBias'): digitizationFlags.numberOfHighPtMinBias = 0.0022 
if hasattr(digitizationFlags, 'numberOfNDMinBias'): digitizationFlags.numberOfNDMinBias = 2.0
if hasattr(digitizationFlags, 'numberOfSDMinBias'): digitizationFlags.numberOfSDMinBias = 0.05
if hasattr(digitizationFlags, 'numberOfDDMinBias'): digitizationFlags.numberOfDDMinBias = 0.05
digitizationFlags.BeamIntensityPattern = [
    1.0,0.0,0.0,0.0,0.0,0.0,# filled bunch crossing  1 (150ns gap)
    1.0,0.0,0.0,0.0,0.0,0.0,# filled bunch crossing  2 (150ns gap)
    1.0,0.0,0.0,0.0,0.0,0.0,# filled bunch crossing  3 (150ns gap)
    1.0,0.0,0.0,0.0,0.0,0.0,# filled bunch crossing  4 (150ns gap)
    1.0,0.0,0.0,0.0,0.0,0.0,# filled bunch crossing  5 (150ns gap)
    1.0,0.0,0.0,0.0,0.0,0.0,# filled bunch crossing  6 (150ns gap)
    1.0,0.0,0.0,0.0,0.0,0.0,# filled bunch crossing  7 (150ns gap)
    1.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,# filled bunch crossing 8 (900ns gap i.e. beyond atlas exposure time)
    0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,
    0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0]
digitizationFlags.cavernIgnoresBeamInt=False

from AthenaCommon.BeamFlags import jobproperties
jobproperties.Beam.bunchSpacing = 150 # Set this to the spacing between filled bunch-crossings within the train.
