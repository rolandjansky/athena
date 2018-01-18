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
     MMdigitization.RndmSvc = jobproperties.Digitization.rndmSvc.getValue()
     MMdigitization.RndmEngine = "MM_Digitization"
     MMdigitization.InputObjectName = "MicromegasSensitiveDetector"
     MMdigitization.OutputObjectName = "MM_DIGITS"
     MMdigitization.OutputSDOName = "MM_SDO"
     MMdigitization.OutputLevel = DEBUG
