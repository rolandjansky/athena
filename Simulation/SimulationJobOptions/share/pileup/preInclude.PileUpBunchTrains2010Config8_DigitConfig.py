####################################################################################
## For comparison with PileUpBunchTrains2010Config7_DigitConfig.py
## Double-train: A train of 8 filled bunch crossings with intra train spacings of
## 150ns, followed by a 225ns gap then a second train of 8 filled bunch crossings
## with intra train spacings of 150ns.
## Any of the filled bunch crossings can be chosen as the
## central bunch crossing, so the timing of the out-of-time will vary significantly
## from event to event.
## Cavern Background is independent of the bunch pattern. (Compatible with 150ns cavern background.)
####################################################################################

from Digitization.DigitizationFlags import digitizationFlags

digitizationFlags.bunchSpacing = 25 # This now sets the bunch slot length.
digitizationFlags.numberOfCavern = 1 #FIXME need to check on appropriate values
digitizationFlags.numberOfCollisions = 2.2
digitizationFlags.initialBunchCrossing = -32
digitizationFlags.finalBunchCrossing = 32
digitizationFlags.numberOfBeamHalo = 0.05 #FIXME need to check on appropriate values
digitizationFlags.numberOfBeamGas = 0.0003 #FIXME need to check on appropriate values
if hasattr(digitizationFlags, 'numberOfLowPtMinBias'): digitizationFlags.numberOfLowPtMinBias = 2.2 
if hasattr(digitizationFlags, 'numberOfHighPtMinBias'): digitizationFlags.numberOfHighPtMinBias = 0.0022 
if hasattr(digitizationFlags, 'numberOfNDMinBias'): digitizationFlags.numberOfNDMinBias = 2.2
if hasattr(digitizationFlags, 'numberOfSDMinBias'): digitizationFlags.numberOfSDMinBias = 0.05
if hasattr(digitizationFlags, 'numberOfDDMinBias'): digitizationFlags.numberOfDDMinBias = 0.05
digitizationFlags.BeamIntensityPattern = [0.0,
1.0,0.0,0.0,0.0,0.0,0.0,# filled bunch crossing  1 (150ns gap)
1.0,0.0,0.0,0.0,0.0,0.0,# filled bunch crossing  2 (150ns gap)
1.0,0.0,0.0,0.0,0.0,0.0,# filled bunch crossing  3 (150ns gap)
1.0,0.0,0.0,0.0,0.0,0.0,# filled bunch crossing  4 (150ns gap)
1.0,0.0,0.0,0.0,0.0,0.0,# filled bunch crossing  5 (150ns gap)
1.0,0.0,0.0,0.0,0.0,0.0,# filled bunch crossing  6 (150ns gap)
1.0,0.0,0.0,0.0,0.0,0.0,# filled bunch crossing  7 (150ns gap)
1.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,# filled bunch crossing 8  (225ns gap)
1.0,0.0,0.0,0.0,0.0,0.0,# filled bunch crossing  9 (150ns gap)
1.0,0.0,0.0,0.0,0.0,0.0,# filled bunch crossing 10 (150ns gap)
1.0,0.0,0.0,0.0,0.0,0.0,# filled bunch crossing 11 (150ns gap)
1.0,0.0,0.0,0.0,0.0,0.0,# filled bunch crossing 12 (150ns gap)
1.0,0.0,0.0,0.0,0.0,0.0,# filled bunch crossing 13 (150ns gap)
1.0,0.0,0.0,0.0,0.0,0.0,# filled bunch crossing 14 (150ns gap)
1.0,0.0,0.0,0.0,0.0,0.0,# filled bunch crossing 15 (150ns gap)
1.0,0.0,0.0,0.0,0.0,0.0,# filled bunch crossing 16 (975ns gap)
0.0,0.0,0.0,0.0,0.0,0.0,
0.0,0.0,0.0,0.0,0.0,0.0,
0.0,0.0,0.0,0.0,0.0,0.0,
0.0,0.0,0.0,0.0,0.0,0.0,
0.0,0.0,0.0,0.0,0.0,0.0,
0.0,0.0,0.0]# this gap is long enough to prevent unwanted wrap-around effects
digitizationFlags.cavernIgnoresBeamInt=False

from AthenaCommon.BeamFlags import jobproperties
jobproperties.Beam.bunchSpacing = 150 # Set this to the spacing between filled bunch-crossings within the train.
