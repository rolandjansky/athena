#--------------------------------------------------------------
# Digitiziation and Pileup configuration
#--------------------------------------------------------------
from Digitization.DigitizationFlags import digitizationFlags
digitizationFlags.doLowPtMinBias=True
digitizationFlags.doCavern=True
digitizationFlags.doBeamGas=False
digitizationFlags.doBeamHalo=False

digitizationFlags.bunchSpacing=25
digitizationFlags.numberOfCavern=2
digitizationFlags.numberOfLowPtMinBias=2.3
digitizationFlags.initialBunchCrossing=-32 #default
digitizationFlags.finalBunchCrossing=32 #default
digitizationFlags.numberOfBeamHalo=0.05
digitizationFlags.numberOfBeamGas=0.0003

include("Digitization/mc09minbiasfiles.py")

include("Digitization/mc09cavernfiles.py")

