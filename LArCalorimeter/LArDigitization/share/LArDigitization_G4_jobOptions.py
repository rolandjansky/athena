################################################################
#
# job Options fragment to create LArDigits  from LArHits which
#  should be already in StoreGate
# to be used on G4 output
#
################################################################

from AthenaCommon.AlgSequence import AlgSequence 
topSequence = AlgSequence()

from Digitization.DigitizationFlags import jobproperties

# flag to use old event/run number seeding for backwards compatibility
if not 'LArDigitUseOldSeed' in dir():
    LArDigitUseOldSeed = False

# Number of samples option (could be set by top JobOptions)
if not 'LArNSamples' in dir():
    LArNSamples = 5

# We also need the conditions svc for MC constants:
include("LArConditionsCommon/LArConditionsCommon_MC_jobOptions.py")

#
# --------------------------------------------------------
# ............ declare the used top algo.
# --------------------------------------------------------
from LArDigitization.LArDigitizationConf import LArDigitMaker
digitmaker1 = LArDigitMaker("digitmaker1")
topSequence += digitmaker1

# Number of samples
digitmaker1.Nsamples = LArNSamples
# Calorimeter Noise
digitmaker1.NoiseOnOff = jobproperties.Digitization.doCaloNoise.get_Value()

# ADC2MeVTool
from LArRecUtils.LArADC2MeVToolDefault import LArADC2MeVToolDefault
theADC2MeVTool = LArADC2MeVToolDefault()
from AthenaCommon.AppMgr import ToolSvc
ToolSvc += theADC2MeVTool
digitmaker1.ADC2MeVTool = theADC2MeVTool

# Tool for noise autocorrelation generation
from LArRecUtils.LArAutoCorrNoiseToolDefault import LArAutoCorrNoiseToolDefault
theAutoCorrNoiseTool = LArAutoCorrNoiseToolDefault()
theAutoCorrNoiseTool.NSamples = LArNSamples
ToolSvc += theAutoCorrNoiseTool
digitmaker1.AutoCorrNoiseTool = theAutoCorrNoiseTool

#########################################################################
#
# end of LArDigitization job Options fragment
#
##########################################################################"
