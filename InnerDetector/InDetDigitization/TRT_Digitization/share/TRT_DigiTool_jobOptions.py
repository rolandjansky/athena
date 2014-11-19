###############################################################
#
# TRT Digitization
#
#==============================================================

from AthenaCommon import CfgGetter
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()
from Digitization.DigitizationFlags import digitizationFlags
if 'doSplitDigi' in jobproperties.Digitization.experimentalDigi():
    job.PileUpToolsAlg.PileUpTools += [ CfgGetter.getPrivateTool("TRTDigitizationToolHS", checkType=True) ]
    job.PileUpToolsAlg.PileUpTools += [ CfgGetter.getPrivateTool("TRTDigitizationToolPU", checkType=True) ]
else:
    job.PileUpToolsAlg.PileUpTools += [ CfgGetter.getPrivateTool("TRTDigitizationTool", checkType=True) ]

