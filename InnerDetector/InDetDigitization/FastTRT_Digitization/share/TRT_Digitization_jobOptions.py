###############################################################
#
# TRT Digitization
#
#==============================================================

from Digitization.DigitizationFlags import jobproperties

from AthenaCommon.AlgSequence import AlgSequence

job = AlgSequence()
from AthenaCommon.CfgGetter import getAlgorithm
job += getAlgorithm("TRTFastDigitization/TRTFastDigitization", tryDefaultConfigurable=True)
