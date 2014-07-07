include.block("MuonByteStreamCnvTest/TgcDigitToTgcRDO_jobOptions.py")

from AthenaCommon.AlgSequence import AlgSequence
from AthenaCommon import CfgGetter
job = AlgSequence()
job += CfgGetter.getAlgorithm("TgcDigitToTgcRDO/TgcDigitToTgcRDO", tryDefaultConfigurable=True)
job.TgcDigitToTgcRDO.isNewTgcDigit = True
    
