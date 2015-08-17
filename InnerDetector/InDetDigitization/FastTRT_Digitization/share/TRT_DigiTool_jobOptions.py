###############################################################
#
# Fast TRT Digitization
#
#==============================================================

from Digitization.DigitizationFlags import digitizationFlags
from AthenaCommon import CfgGetter
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()
job.PileUpToolsAlg.PileUpTools[ CfgGetter.getPrivateTool("TRTFastDigitizationTool", checkType=True) ]
