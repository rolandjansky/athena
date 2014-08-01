####################################################################################
## Triple-Train: A train made up of a sub-trains of 36 filled bunch crossings 
## with intra train spacings of 50ns, followed by a 225ns gap then a second sub-train 
## of 36 filled bunch crossings with intra train spacings of 50s, followed by a 225ns 
## gap then a third sub-train of 36 filled bunch crossings with intra train spacings of 50s.
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
1.0,0.0,1.0,0.0,1.0,0.0,# filled bunch crossing  1 (50ns gap)  2 (50ns gap)  3 (50ns gap)
1.0,0.0,1.0,0.0,1.0,0.0,# filled bunch crossing  4 (50ns gap)  5 (50ns gap)  6 (50ns gap)
1.0,0.0,1.0,0.0,1.0,0.0,# filled bunch crossing  7 (50ns gap)  8 (50ns gap)  9 (50ns gap)
1.0,0.0,1.0,0.0,1.0,0.0,# filled bunch crossing 10 (50ns gap) 11 (50ns gap) 12 (50ns gap)
1.0,0.0,1.0,0.0,1.0,0.0,# filled bunch crossing 13 (50ns gap) 14 (50ns gap) 15 (50ns gap) 
1.0,0.0,1.0,0.0,1.0,0.0,# filled bunch crossing 16 (50ns gap) 17 (50ns gap) 18 (50ns gap)
1.0,0.0,1.0,0.0,1.0,0.0,# filled bunch crossing 19 (50ns gap) 20 (50ns gap) 21 (50ns gap)
1.0,0.0,1.0,0.0,1.0,0.0,# filled bunch crossing 22 (50ns gap) 23 (50ns gap) 24 (50ns gap)
1.0,0.0,1.0,0.0,1.0,0.0,# filled bunch crossing 25 (50ns gap) 26 (50ns gap) 27 (50ns gap) 
1.0,0.0,1.0,0.0,1.0,0.0,# filled bunch crossing 28 (50ns gap) 29 (50ns gap) 30 (50ns gap)
1.0,0.0,1.0,0.0,1.0,0.0,# filled bunch crossing 31 (50ns gap) 32 (50ns gap) 33 (50ns gap)
1.0,0.0,1.0,0.0,        # filled bunch crossing 34 (50ns gap) 35 (50ns gap)
1.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,# filled bunch crossing 36 (225ns gap)
1.0,0.0,1.0,0.0,1.0,0.0,# filled bunch crossing  1 (50ns gap)  2 (50ns gap)  3 (50ns gap)
1.0,0.0,1.0,0.0,1.0,0.0,# filled bunch crossing  4 (50ns gap)  5 (50ns gap)  6 (50ns gap)
1.0,0.0,1.0,0.0,1.0,0.0,# filled bunch crossing  7 (50ns gap)  8 (50ns gap)  9 (50ns gap)
1.0,0.0,1.0,0.0,1.0,0.0,# filled bunch crossing 10 (50ns gap) 11 (50ns gap) 12 (50ns gap)
1.0,0.0,1.0,0.0,1.0,0.0,# filled bunch crossing 13 (50ns gap) 14 (50ns gap) 15 (50ns gap) 
1.0,0.0,1.0,0.0,1.0,0.0,# filled bunch crossing 16 (50ns gap) 17 (50ns gap) 18 (50ns gap)
1.0,0.0,1.0,0.0,1.0,0.0,# filled bunch crossing 19 (50ns gap) 20 (50ns gap) 21 (50ns gap)
1.0,0.0,1.0,0.0,1.0,0.0,# filled bunch crossing 22 (50ns gap) 23 (50ns gap) 24 (50ns gap)
1.0,0.0,1.0,0.0,1.0,0.0,# filled bunch crossing 25 (50ns gap) 26 (50ns gap) 27 (50ns gap) 
1.0,0.0,1.0,0.0,1.0,0.0,# filled bunch crossing 28 (50ns gap) 29 (50ns gap) 30 (50ns gap)
1.0,0.0,1.0,0.0,1.0,0.0,# filled bunch crossing 31 (50ns gap) 32 (50ns gap) 33 (50ns gap)
1.0,0.0,1.0,0.0,        # filled bunch crossing 34 (50ns gap) 35 (50ns gap)
1.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,# filled bunch crossing 36 (225ns gap)
1.0,0.0,1.0,0.0,1.0,0.0,# filled bunch crossing  1 (50ns gap)  2 (50ns gap)  3 (50ns gap)
1.0,0.0,1.0,0.0,1.0,0.0,# filled bunch crossing  4 (50ns gap)  5 (50ns gap)  6 (50ns gap)
1.0,0.0,1.0,0.0,1.0,0.0,# filled bunch crossing  7 (50ns gap)  8 (50ns gap)  9 (50ns gap)
1.0,0.0,1.0,0.0,1.0,0.0,# filled bunch crossing 10 (50ns gap) 11 (50ns gap) 12 (50ns gap)
1.0,0.0,1.0,0.0,1.0,0.0,# filled bunch crossing 13 (50ns gap) 14 (50ns gap) 15 (50ns gap) 
1.0,0.0,1.0,0.0,1.0,0.0,# filled bunch crossing 16 (50ns gap) 17 (50ns gap) 18 (50ns gap)
1.0,0.0,1.0,0.0,1.0,0.0,# filled bunch crossing 19 (50ns gap) 20 (50ns gap) 21 (50ns gap)
1.0,0.0,1.0,0.0,1.0,0.0,# filled bunch crossing 22 (50ns gap) 23 (50ns gap) 24 (50ns gap)
1.0,0.0,1.0,0.0,1.0,0.0,# filled bunch crossing 25 (50ns gap) 26 (50ns gap) 27 (50ns gap) 
1.0,0.0,1.0,0.0,1.0,0.0,# filled bunch crossing 28 (50ns gap) 29 (50ns gap) 30 (50ns gap)
1.0,0.0,1.0,0.0,1.0,0.0,# filled bunch crossing 31 (50ns gap) 32 (50ns gap) 33 (50ns gap)
1.0,0.0,1.0,0.0,        # filled bunch crossing 34 (50ns gap) 35 (50ns gap)
1.0,0.0,0.0,0.0,0.0,0.0,# filled bunch crossing 36 (975ns gap)
0.0,0.0,0.0,0.0,0.0,0.0,
0.0,0.0,0.0,0.0,0.0,0.0,
0.0,0.0,0.0,0.0,0.0,0.0,
0.0,0.0,0.0,0.0,0.0,0.0,
0.0,0.0,0.0,0.0,0.0,0.0,
0.0,0.0,0.0]# this gap is long enough to prevent unwanted wrap-around effects

from AthenaCommon.BeamFlags import jobproperties
jobproperties.Beam.bunchSpacing = 50 # Set this to the spacing between filled bunch-crossings within the train.
