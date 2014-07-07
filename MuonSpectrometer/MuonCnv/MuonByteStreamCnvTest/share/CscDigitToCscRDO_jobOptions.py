
include.block("MuonByteStreamCnvTest/CscDigitToCscRDO_jobOptions.py")
print "DEPRECATION WARNING: please replace include(\"MuonByteStreamCnvTest/CscDigitToCscRDO_jobOptions.py\")\nwith:\nfrom AthenaCommon import CfgGetter\njob += CfgGetter.getAlgorithm(\"CscDigitToCscRDO/CscDigitToCscRDO\", tryDefaultConfigurable=True)"

from AthenaCommon.AlgSequence import AlgSequence
from AthenaCommon import CfgGetter
job = AlgSequence()
job += CfgGetter.getAlgorithm("CscDigitToCscRDO/CscDigitToCscRDO", tryDefaultConfigurable=True)
# defined in MuonCnvExample/python/MuonCnvConfig.py
