#########################################################
#
# MM_Digitization/postInclude.MicroMegasDigitization.py
# Nektarios Chr. Benekos
# This job option should be added via the postInclude
# command line argument.
#########################################################
include.block ("MM_Digitization/postInclude.MicroMegasDigitization.py")
from Digitization.DigitizationFlags import digitizationFlags
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()
if digitizationFlags.doXingByXingPileUp():
     MMdigitization = job.PileUpToolsAlg.PileUpTools[ "MmDigitizationTool" ]
else:
     from MM_Digitization.MM_DigitizationConf import MmDigitizationTool as MMdigitization
     MMdigitization.RndmSvc = jobproperties.Digitization.rndmSvc.get_Value()
     MMdigitization.RndmEngine = "MM_Digitization"
     MMdigitization.InputObjectName = "MicromegasSensitiveDetector" 
     MMdigitization.OutputObjectName = "MM_DIGITS"
     MMdigitization.OutputSDOName = "MM_SDO" 
     # MMdigitization.CheckSimHits = True
     # MMdigitization.ValidationSetup = False
     ## Strip response
     # MMdigitization.qThreshold = 0.00000000001
     # MMdigitization.qThresholdForTrigger = 2.
     # MMdigitization.DiffusSigma = 0.036
     # MMdigitization.LogitundinalDiffusSigma = 0.019
     # MMdigitization.DriftVelocity = 0.047
     # MMdigitization.crossTalk1 = 0.0
     # MMdigitization.crossTalk2 = 0.0
     ## Chip/electronics response
     # MMdigitization.peakTime = 50.
     # MMdigitization.SaveInternalHistos = True
     # MMdigitization.EnergyThreshold = 50
     # MMdigitization.OutputLevel = DEBUG
     # MMdigitization.electronicsThreshold = 5000.
