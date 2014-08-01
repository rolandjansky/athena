####################################################################################
## Double-train: A train of 24 filled bunch crossings with intra train spacings of
## 75ns, followed by a 225ns gap then a second train of 24 filled bunch crossings
## with intra train spacings of 75ns.
## Any of the filled bunch crossings can be chosen as the
## central bunch crossing, so the timing of the out-of-time pile-up will vary 
## significantly from event to event.
## Cavern Background is independent of the bunch pattern. (Compatible with 25ns cavern background.)
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
digitizationFlags.BeamIntensityPattern = [0.0, # bunch crossing zero is always empty
1.0,0.0,0.0,1.0,0.0,0.0,# filled bunch crossing  1 (75ns gap)  2 (75ns gap)
1.0,0.0,0.0,1.0,0.0,0.0,# filled bunch crossing  3 (75ns gap)  4 (75ns gap)
1.0,0.0,0.0,1.0,0.0,0.0,# filled bunch crossing  5 (75ns gap)  6 (75ns gap)
1.0,0.0,0.0,1.0,0.0,0.0,# filled bunch crossing  7 (75ns gap)  8 (75ns gap)
1.0,0.0,0.0,1.0,0.0,0.0,# filled bunch crossing  9 (75ns gap) 10 (75ns gap)
1.0,0.0,0.0,1.0,0.0,0.0,# filled bunch crossing 11 (75ns gap) 12 (75ns gap)
1.0,0.0,0.0,1.0,0.0,0.0,# filled bunch crossing 13 (75ns gap) 14 (75ns gap)
1.0,0.0,0.0,1.0,0.0,0.0,# filled bunch crossing 15 (75ns gap) 16 (75ns gap)
1.0,0.0,0.0,1.0,0.0,0.0,# filled bunch crossing 17 (75ns gap) 18 (75ns gap)
1.0,0.0,0.0,1.0,0.0,0.0,# filled bunch crossing 19 (75ns gap) 20 (75ns gap)
1.0,0.0,0.0,1.0,0.0,0.0,# filled bunch crossing 21 (75ns gap) 22 (75ns gap)
1.0,0.0,0.0,            # filled bunch crossing 23 (75ns gap)
1.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,# filled bunch crossing 24 (225ns gap)
1.0,0.0,0.0,1.0,0.0,0.0,# filled bunch crossing  1 (75ns gap)  2 (75ns gap)
1.0,0.0,0.0,1.0,0.0,0.0,# filled bunch crossing  3 (75ns gap)  4 (75ns gap)
1.0,0.0,0.0,1.0,0.0,0.0,# filled bunch crossing  5 (75ns gap)  6 (75ns gap)
1.0,0.0,0.0,1.0,0.0,0.0,# filled bunch crossing  7 (75ns gap)  8 (75ns gap)
1.0,0.0,0.0,1.0,0.0,0.0,# filled bunch crossing  9 (75ns gap) 10 (75ns gap)
1.0,0.0,0.0,1.0,0.0,0.0,# filled bunch crossing 11 (75ns gap) 12 (75ns gap)
1.0,0.0,0.0,1.0,0.0,0.0,# filled bunch crossing 13 (75ns gap) 14 (75ns gap)
1.0,0.0,0.0,1.0,0.0,0.0,# filled bunch crossing 15 (75ns gap) 16 (75ns gap)
1.0,0.0,0.0,1.0,0.0,0.0,# filled bunch crossing 17 (75ns gap) 18 (75ns gap)
1.0,0.0,0.0,1.0,0.0,0.0,# filled bunch crossing 19 (75ns gap) 20 (75ns gap)
1.0,0.0,0.0,1.0,0.0,0.0,# filled bunch crossing 21 (75ns gap) 22 (75ns gap)
1.0,0.0,0.0,            # filled bunch crossing 23 (75ns gap)
1.0,0.0,0.0,0.0,0.0,0.0,# filled bunch crossing 24 (975ns gap)
0.0,0.0,0.0,0.0,0.0,0.0,
0.0,0.0,0.0,0.0,0.0,0.0,
0.0,0.0,0.0,0.0,0.0,0.0,
0.0,0.0,0.0,0.0,0.0,0.0,
0.0,0.0,0.0,0.0,0.0,0.0,
0.0,0.0,0.0]# this gap is long enough to prevent unwanted wrap-around effects

from AthenaCommon.BeamFlags import jobproperties
jobproperties.Beam.bunchSpacing = 75 # Set this to the spacing between filled bunch-crossings within the train.
