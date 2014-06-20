
from AthenaCommon.AlgSequence import AlgSequence
job = AlgSequence()

from LUCID_RawDataByteStreamCnv.LUCID_RawDataByteStreamCnvConf import LUCID_DigitRawDataCnv
job += LUCID_DigitRawDataCnv( "LUCID_DigitRawDataCnv" )   

job.LUCID_DigitRawDataCnv = INFO
