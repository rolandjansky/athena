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
    job += CfgGetter.getAlgorithm("TRTDigitizationHS")
    job += CfgGetter.getAlgorithm("TRTDigitizationPU")
else:
    job += CfgGetter.getAlgorithm("TRTDigitization/TRTDigitization", tryDefaultConfigurable=True)
