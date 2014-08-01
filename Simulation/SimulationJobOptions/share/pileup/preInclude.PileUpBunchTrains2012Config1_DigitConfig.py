####################################################################################
## For comparison with PileUpBunchTrains2012Config1_DigitConfig.py
## Quadruple-Train 25ns intra-train spacing: A train made up of a
## sub-train of 72 filled bunch crossings with intra-train spacings
## of 25ns, followed by a 225ns gap then a second sub-train of 72
## filled bunch crossings with intra train spacings of 25s, followed
## by a 225ns gap then a third sub-train of 72 filled bunch crossings
## with intra train spacings of 25s, followed by a 225ns gap then a
## fourth sub-train of 72 filled bunch crossings with intra train
## spacings of 25s.  Any of the filled bunch crossings can be chosen
## as the central bunch crossing, so the timing of the out-of-time
## pile-up will vary significantly from event to event.
## Cavern Background is independent of the bunch pattern. (Compatible
## with 25ns cavern background.)
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
1.0,1.0,1.0,1.0,1.0,1.0,# filled bunch crossings 1 -  6 (25ns gaps)
1.0,1.0,1.0,1.0,1.0,1.0,# filled bunch crossing  7 - 12 (25ns gaps)
1.0,1.0,1.0,1.0,1.0,1.0,# filled bunch crossing 13 - 18 (25ns gaps)
1.0,1.0,1.0,1.0,1.0,1.0,# filled bunch crossing 19 - 24 (25ns gaps)
1.0,1.0,1.0,1.0,1.0,1.0,# filled bunch crossing 25 - 30 (25ns gaps) 
1.0,1.0,1.0,1.0,1.0,1.0,# filled bunch crossing 31 - 36 (25ns gaps)
1.0,1.0,1.0,1.0,1.0,1.0,# filled bunch crossing 37 - 42 (25ns gaps)
1.0,1.0,1.0,1.0,1.0,1.0,# filled bunch crossing 43 - 48 (25ns gaps)
1.0,1.0,1.0,1.0,1.0,1.0,# filled bunch crossing 49 - 54 (25ns gaps) 
1.0,1.0,1.0,1.0,1.0,1.0,# filled bunch crossing 55 - 60 (25ns gaps)
1.0,1.0,1.0,1.0,1.0,1.0,# filled bunch crossing 61 - 66 (25ns gaps)
1.0,1.0,1.0,1.0,1.0,    # filled bunch crossing 67 - 71 (25ns gaps)
1.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,# filled bunch crossing 72 (225ns gap)
1.0,1.0,1.0,1.0,1.0,1.0,# filled bunch crossings 1 -  6 (25ns gaps)
1.0,1.0,1.0,1.0,1.0,1.0,# filled bunch crossing  7 - 12 (25ns gaps)
1.0,1.0,1.0,1.0,1.0,1.0,# filled bunch crossing 13 - 18 (25ns gaps)
1.0,1.0,1.0,1.0,1.0,1.0,# filled bunch crossing 19 - 24 (25ns gaps)
1.0,1.0,1.0,1.0,1.0,1.0,# filled bunch crossing 25 - 30 (25ns gaps) 
1.0,1.0,1.0,1.0,1.0,1.0,# filled bunch crossing 31 - 36 (25ns gaps)
1.0,1.0,1.0,1.0,1.0,1.0,# filled bunch crossing 37 - 42 (25ns gaps)
1.0,1.0,1.0,1.0,1.0,1.0,# filled bunch crossing 43 - 48 (25ns gaps)
1.0,1.0,1.0,1.0,1.0,1.0,# filled bunch crossing 49 - 54 (25ns gaps) 
1.0,1.0,1.0,1.0,1.0,1.0,# filled bunch crossing 55 - 60 (25ns gaps)
1.0,1.0,1.0,1.0,1.0,1.0,# filled bunch crossing 61 - 66 (25ns gaps)
1.0,1.0,1.0,1.0,1.0,    # filled bunch crossing 67 - 71 (25ns gaps)
1.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,# filled bunch crossing 72 (225ns gap)
1.0,1.0,1.0,1.0,1.0,1.0,# filled bunch crossings 1 -  6 (25ns gaps)
1.0,1.0,1.0,1.0,1.0,1.0,# filled bunch crossing  7 - 12 (25ns gaps)
1.0,1.0,1.0,1.0,1.0,1.0,# filled bunch crossing 13 - 18 (25ns gaps)
1.0,1.0,1.0,1.0,1.0,1.0,# filled bunch crossing 19 - 24 (25ns gaps)
1.0,1.0,1.0,1.0,1.0,1.0,# filled bunch crossing 25 - 30 (25ns gaps) 
1.0,1.0,1.0,1.0,1.0,1.0,# filled bunch crossing 31 - 36 (25ns gaps)
1.0,1.0,1.0,1.0,1.0,1.0,# filled bunch crossing 37 - 42 (25ns gaps)
1.0,1.0,1.0,1.0,1.0,1.0,# filled bunch crossing 43 - 48 (25ns gaps)
1.0,1.0,1.0,1.0,1.0,1.0,# filled bunch crossing 49 - 54 (25ns gaps) 
1.0,1.0,1.0,1.0,1.0,1.0,# filled bunch crossing 55 - 60 (25ns gaps)
1.0,1.0,1.0,1.0,1.0,1.0,# filled bunch crossing 61 - 66 (25ns gaps)
1.0,1.0,1.0,1.0,1.0,    # filled bunch crossing 67 - 71 (25ns gaps)
1.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,# filled bunch crossing 72 (225ns gap)
1.0,1.0,1.0,1.0,1.0,1.0,# filled bunch crossings 1 -  6 (25ns gaps)
1.0,1.0,1.0,1.0,1.0,1.0,# filled bunch crossing  7 - 12 (25ns gaps)
1.0,1.0,1.0,1.0,1.0,1.0,# filled bunch crossing 13 - 18 (25ns gaps)
1.0,1.0,1.0,1.0,1.0,1.0,# filled bunch crossing 19 - 24 (25ns gaps)
1.0,1.0,1.0,1.0,1.0,1.0,# filled bunch crossing 25 - 30 (25ns gaps) 
1.0,1.0,1.0,1.0,1.0,1.0,# filled bunch crossing 31 - 36 (25ns gaps)
1.0,1.0,1.0,1.0,1.0,1.0,# filled bunch crossing 37 - 42 (25ns gaps)
1.0,1.0,1.0,1.0,1.0,1.0,# filled bunch crossing 43 - 48 (25ns gaps)
1.0,1.0,1.0,1.0,1.0,1.0,# filled bunch crossing 49 - 54 (25ns gaps) 
1.0,1.0,1.0,1.0,1.0,1.0,# filled bunch crossing 55 - 60 (25ns gaps)
1.0,1.0,1.0,1.0,1.0,1.0,# filled bunch crossing 61 - 66 (25ns gaps)
1.0,1.0,1.0,1.0,1.0,    # filled bunch crossing 67 - 71 (25ns gaps)
1.0,0.0,0.0,0.0,0.0,0.0,# filled bunch crossing 72 (975ns gap)
0.0,0.0,0.0,0.0,0.0,0.0,
0.0,0.0,0.0,0.0,0.0,0.0,
0.0,0.0,0.0,0.0,0.0,0.0,
0.0,0.0,0.0,0.0,0.0,0.0,
0.0,0.0,0.0,0.0,0.0,0.0,
0.0,0.0,0.0]# this gap is long enough to prevent unwanted wrap-around effects
digitizationFlags.cavernIgnoresBeamInt=False

from AthenaCommon.BeamFlags import jobproperties
jobproperties.Beam.bunchSpacing = 25 # Set this to the spacing between filled bunch-crossings within the train.
