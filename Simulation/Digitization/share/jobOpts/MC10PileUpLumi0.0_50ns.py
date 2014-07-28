#--------------------------------------------------------------
# Digitiziation and Pileup configuration
#--------------------------------------------------------------
from Digitization.DigitizationFlags import digitizationFlags
digitizationFlags.doLowPtMinBias=False
digitizationFlags.doCavern=True
digitizationFlags.doBeamGas=False
digitizationFlags.doBeamHalo=False

digitizationFlags.bunchSpacing=25 # This now sets the bunch slot length.
digitizationFlags.numberOfCavern=0
digitizationFlags.BeamIntensityPattern = [0.0,1.0]

from AthenaCommon.BeamFlags import jobproperties
jobproperties.Beam.bunchSpacing = 50 # Set this to the spacing between filled bunch-crossings within the train.

include("Digitization/mc10cavernfiles.py")
