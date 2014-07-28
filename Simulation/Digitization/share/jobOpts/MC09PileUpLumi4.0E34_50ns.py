#--------------------------------------------------------------
# Digitiziation and Pileup configuration
#--------------------------------------------------------------
from Digitization.DigitizationFlags import digitizationFlags
digitizationFlags.doLowPtMinBias=True
digitizationFlags.doCavern=True
#digitizationFlags.doBeamGas=True
#digitizationFlags.doBeamHalo=True

digitizationFlags.bunchSpacing=25 # This now sets the bunch slot length.
digitizationFlags.numberOfCavern=92
digitizationFlags.numberOfLowPtMinBias=184.0
digitizationFlags.initialBunchCrossing=-32 #default
digitizationFlags.finalBunchCrossing=32 #default
digitizationFlags.numberOfBeamHalo=0.05
digitizationFlags.numberOfBeamGas=0.0003
digitizationFlags.BeamIntensityPattern = [0.0,1.0]

from AthenaCommon.BeamFlags import jobproperties
jobproperties.Beam.bunchSpacing = 50 # Set this to the spacing between filled bunch-crossings within the train.

include("Digitization/mc09minbiasfiles.py")

include("Digitization/mc09cavernfiles.py")
