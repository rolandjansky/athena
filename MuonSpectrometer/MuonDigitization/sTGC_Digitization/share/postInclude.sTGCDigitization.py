########################################################
#
# sTGC_Digitization/postInclude.sTGCDigitization.py
# Nektarios Chr. Benekos
# This job option should be added via the postInclude
# command line argument.
#########################################################
include.block ("sTGC_Digitization/postInclude.sTGCDigitization.py")
from Digitization.DigitizationFlags import digitizationFlags
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()
if digitizationFlags.doXingByXingPileUp():
     sTGCdigitization = job.PileUpToolsAlg.PileUpTools[ "sTgcDigitizationTool" ]
else:
     from sTGC_Digitization.sTGC_DigitizationConf import sTgcDigitizationTool as sTGCdigitization
     sTGCdigitization.RndmSvc = jobproperties.Digitization.rndmSvc.get_Value()
     sTGCdigitization.RndmEngine = "sTGC_Digitization"
     sTGCdigitization.InputObjectName = "sTGCSensitiveDetector" 
     sTGCdigitization.OutputObjectName = "sTGC_DIGITS" 
     sTGCdigitization.OutputSDOName = "sTGC_SDO" 
     sTGCdigitization.OutputLevel = VERBOSE
     sTGCdigitization.doChannelTypes = 3 #1 --> strips ; 2 --> strips / wires ; 3 --> strips / wires / pads

     sTGCdigitization.doToFCorrection = False
     #sTGCdigitization.doToFCorrection = True
     sTGCdigitization.DeadtimeElectronicsStrip = 50
     sTGCdigitization.DeadtimeElectronicsPad = 0
     sTGCdigitization.timeWindowPad = 30
     sTGCdigitization.timeWindowStrip = 30

